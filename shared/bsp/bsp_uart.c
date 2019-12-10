/****************************************************************************
 *                                                                          *
 *  Copyright (C) 2020 RoboMaster.                                          *
 *  Illini RoboMaster @ University of Illinois at Urbana-Champaign          *
 *                                                                          *
 *  This program is free software: you can redistribute it and/or modify    *
 *  it under the terms of the GNU General Public License as published by    *
 *  the Free Software Foundation, either version 3 of the License, or       *
 *  (at your option) any later version.                                     *
 *                                                                          *
 *  This program is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *  GNU General Public License for more details.                            *
 *                                                                          *
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                          *
 ****************************************************************************/

#include <string.h>

#include "bsp_uart.h"
#include "bsp_error_handler.h"

#include "cmsis_os.h"

#define MAX_NUM_UARTS 5

static uart_t *uarts[MAX_NUM_UARTS] = { 0 };
static size_t num_uarts = 0;

static uint8_t uart_handle_exists(UART_HandleTypeDef *huart) {
  for (size_t i = 0; i < num_uarts; ++i) {
    if (huart == uarts[i]->huart) {
      return 1;
    }
  }
  return 0;
}

static uart_t* find_uart_instance(UART_HandleTypeDef *huart) {
  for (size_t i = 0; i < num_uarts; ++i) {
    if (huart == uarts[i]->huart) {
      return uarts[i];
    }
  }
  return NULL;
}

static void uart_tx_complete_callback(UART_HandleTypeDef *huart) {
  uart_t *uart = find_uart_instance(huart);
  if (!uart)
    return;

  uint8_t *tmp;
  UBaseType_t isrflags = taskENTER_CRITICAL_FROM_ISR();
  if (uart->tx_pending) {
    tmp = uart->tx_read;
    uart->tx_read = uart->tx_write;
    uart->tx_write = tmp;
    HAL_UART_Transmit_DMA(huart, uart->tx_read, uart->tx_pending);
    uart->tx_pending = 0;
  }
  taskEXIT_CRITICAL_FROM_ISR(isrflags);

  if (uart->tx_callback)
    uart->tx_callback(uart);
}

static HAL_StatusTypeDef uart_receive_dma_double_buffer(
    UART_HandleTypeDef *huart, uint8_t *data0, uint8_t *data1, uint16_t size) {
  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY) {
    if ((data0 == NULL) || (data1 == NULL) || (size == 0U))
      return HAL_ERROR;

    /* Process Locked */
    __HAL_LOCK(huart);

    huart->RxXferSize = size;
    huart->ErrorCode = HAL_UART_ERROR_NONE;

    /* Enable the DMA stream */
    HAL_DMAEx_MultiBufferStart(
        huart->hdmarx, (uint32_t)&huart->Instance->DR, (uint32_t)data0, (uint32_t)data1, size);

    /* Clear the Overrun flag just before enabling the DMA Rx request */
    __HAL_UART_CLEAR_OREFLAG(huart);

    /* Process Unlocked */
    __HAL_UNLOCK(huart);

    /* Enable the UART Parity Error Interrupt */
    SET_BIT(huart->Instance->CR1, USART_CR1_PEIE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    SET_BIT(huart->Instance->CR3, USART_CR3_EIE);

    /* Enable the DMA transfer for the receiver request by setting the DMAR bit
    in the UART CR3 register */
    SET_BIT(huart->Instance->CR3, USART_CR3_DMAR);

    return HAL_OK;
  }
  else {
    return HAL_BUSY;
  }
}

void RM_UART_IRQHandler(UART_HandleTypeDef *huart) {
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) && __HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE)) {
    __HAL_UART_CLEAR_IDLEFLAG(huart);
    uart_t *uart = find_uart_instance(huart);
    if (!uart)
      return;

    if (uart->rx_callback)
      uart->rx_callback(uart);
  }
}

uart_t* uart_init(uart_t *uart, UART_HandleTypeDef *huart, uint32_t rx_size, uint32_t tx_size) {
  if (!huart || uart_handle_exists(huart) || num_uarts == MAX_NUM_UARTS) {
    return NULL;
  }

  if (!uart) {
    uart = pvPortMalloc(sizeof(uart_t));
  }

  uarts[num_uarts++] = uart;

  uart->huart = huart;
  uart->rx_size = rx_size;
  uart->rx_data0 = pvPortMalloc(uart->rx_size * sizeof(uint8_t));
  uart->rx_data1 = pvPortMalloc(uart->rx_size * sizeof(uint8_t));
  uart->rx_callback = NULL;
  uart->tx_size = tx_size;
  uart->tx_pending = 0;
  uart->tx_read = pvPortMalloc(uart->tx_size * sizeof(uint8_t));
  uart->tx_write = pvPortMalloc(uart->tx_size * sizeof(uint8_t));
  uart->tx_callback = NULL;

  HAL_UART_RegisterCallback(huart, HAL_UART_TX_COMPLETE_CB_ID, uart_tx_complete_callback);

  __HAL_UART_CLEAR_FLAG(huart, UART_FLAG_IDLE);
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);

  uart_receive_dma_double_buffer(huart, uart->rx_data0, uart->rx_data1, uart->rx_size);

  return uart;
}

uint8_t* uart_read(uart_t *uart, uint32_t *length) {
  if (!uart || !length)
    return NULL;
  // capture pending bytes and perform hardware buffer switch
  taskENTER_CRITICAL();
  __HAL_DMA_DISABLE(uart->huart->hdmarx);
  *length = uart->rx_size - __HAL_DMA_GET_COUNTER(uart->huart->hdmarx);
  __HAL_DMA_SET_COUNTER(uart->huart->hdmarx, uart->rx_size);
  uart->huart->hdmarx->Instance->CR ^= DMA_SxCR_CT;
  __HAL_DMA_ENABLE(uart->huart->hdmarx);
  taskEXIT_CRITICAL();

  if (uart->huart->hdmarx->Instance->CR & DMA_SxCR_CT)
    return uart->rx_data0;
  else
    return uart->rx_data1;
}

int32_t uart_write(uart_t *uart, uint8_t *data, uint32_t length) {
  if (!uart)
    return -1;

  taskENTER_CRITICAL();
  if (uart->huart->gState == HAL_UART_STATE_BUSY_TX || uart->tx_pending) {
    if (length + uart->tx_pending > uart->tx_size)
      length = uart->tx_size - uart->tx_pending;
    memcpy(uart->tx_write + uart->tx_pending, data, length);
    uart->tx_pending += length;
    taskEXIT_CRITICAL();
  }
  else {
    taskEXIT_CRITICAL();
    if (length > uart->tx_size)
      length = uart->tx_size;
    memcpy(uart->tx_read, data, length);
    HAL_UART_Transmit_DMA(uart->huart, uart->tx_read, length);
  }

  return length;
}
