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

/* maximum number of uart allowed to use at the same time */
static uart_t *uarts[MAX_NUM_UARTS] = { 0 };
/* number of uarts initialized and in use */
static size_t num_uarts = 0;

/* check if huart is associated with any initialized uart_t structs */
static uint8_t uart_handle_exists(UART_HandleTypeDef *huart) {
  for (size_t i = 0; i < num_uarts; ++i)
    if (huart == uarts[i]->huart)
      return 1;

  return 0;
}

/* get initialized uart_t instance given its huart handle struct */
static uart_t* find_uart_instance(UART_HandleTypeDef *huart) {
  for (size_t i = 0; i < num_uarts; ++i)
    if (huart == uarts[i]->huart)
      return uarts[i];

  return NULL;
}

/* tx dma complete -> check for pending message to keep transmitting */
static void uart_tx_complete_callback(UART_HandleTypeDef *huart) {
  uart_t *uart = find_uart_instance(huart);
  if (!uart)
    return;

  uint8_t *tmp;
  UBaseType_t isrflags = taskENTER_CRITICAL_FROM_ISR();
  /* check if any data is waiting to be transmitted */
  if (uart->tx_pending) {
    /* swap read / write buffer */
    tmp = uart->tx_read;
    uart->tx_read = uart->tx_write;
    uart->tx_write = tmp;
    /* initiate new transmission call for pending data */
    HAL_UART_Transmit_DMA(huart, uart->tx_read, uart->tx_pending);
    /* clear the number of pending bytes */
    uart->tx_pending = 0;
  }
  taskEXIT_CRITICAL_FROM_ISR(isrflags);
}

/* modified verision of HAL_UART_Receive_DMA that utilize double buffer mode */
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

/* overwrite the weak function defined in board specific usart.c to handle IRQ requests */
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

uart_t* uart_init(uart_t *uart, UART_HandleTypeDef *huart, uint32_t tx_buffer_size) {
  if (!huart || uart_handle_exists(huart) || num_uarts == MAX_NUM_UARTS)
    return NULL;

  if (!uart)
    uart = pvPortMalloc(sizeof(uart_t));

  uarts[num_uarts++] = uart;

  uart->huart = huart;
  /* rx is not activated by default */
  uart->rx_size = 0;
  uart->rx_data0 = NULL;
  uart->rx_data1 = NULL;
  uart->rx_callback = NULL;
  /* tx is ready to use after init */
  uart->tx_size = tx_buffer_size;
  uart->tx_pending = 0;
  uart->tx_read = pvPortMalloc(uart->tx_size * sizeof(uint8_t));
  uart->tx_write = pvPortMalloc(uart->tx_size * sizeof(uint8_t));

  HAL_UART_RegisterCallback(huart, HAL_UART_TX_COMPLETE_CB_ID, uart_tx_complete_callback);

  return uart;
}

int8_t uart_start_receiving(uart_t *uart, uint32_t rx_buffer_size, uart_callback_t callback) {
  if (!uart || !rx_buffer_size || uart->rx_size || uart->rx_data0 || uart->rx_data1)
    return -1;

  uart->rx_size = rx_buffer_size;
  uart->rx_data0 = pvPortMalloc(uart->rx_size * sizeof(uint8_t));
  uart->rx_data1 = pvPortMalloc(uart->rx_size * sizeof(uint8_t));
  uart->rx_callback = callback;

  /* UART IDLE Interrupt can notify application of data reception ASAP */
  __HAL_UART_CLEAR_FLAG(uart->huart, UART_FLAG_IDLE);
  __HAL_UART_ENABLE_IT(uart->huart, UART_IT_IDLE);

  /* enable uart rx dma transfer in back ground */
  uart_receive_dma_double_buffer(uart->huart, uart->rx_data0, uart->rx_data1, uart->rx_size);

  return 0;
}

uint8_t* uart_read(uart_t *uart, uint32_t *length) {
  if (!uart || !length)
    return NULL;
  /* capture pending bytes and perform hardware buffer switch */
  taskENTER_CRITICAL();
  __HAL_DMA_DISABLE(uart->huart->hdmarx);
  *length = uart->rx_size - __HAL_DMA_GET_COUNTER(uart->huart->hdmarx);
  __HAL_DMA_SET_COUNTER(uart->huart->hdmarx, uart->rx_size);
  uart->huart->hdmarx->Instance->CR ^= DMA_SxCR_CT;
  __HAL_DMA_ENABLE(uart->huart->hdmarx);
  taskEXIT_CRITICAL();

  /* return the buffer pointer currently not being used by DMA transfer */
  if (uart->huart->hdmarx->Instance->CR & DMA_SxCR_CT)
    return uart->rx_data0; // DMA is transfering data into rx_data1
  else
    return uart->rx_data1; // DMA is transfering data into rx_data0
}

int32_t uart_write(uart_t *uart, uint8_t *data, uint32_t length) {
  if (!uart)
    return -1;

  taskENTER_CRITICAL();
  if (uart->huart->gState == HAL_UART_STATE_BUSY_TX || uart->tx_pending) {
    /* uart tx currently transmitting -> atomically queue up new data */
    if (length + uart->tx_pending > uart->tx_size)
      length = uart->tx_size - uart->tx_pending;
    memcpy(uart->tx_write + uart->tx_pending, data, length);
    uart->tx_pending += length;
    taskEXIT_CRITICAL();
  }
  else {
    /* uart tx not transmitting so exit critical session early */
    taskEXIT_CRITICAL();
    if (length > uart->tx_size)
      length = uart->tx_size;
    /* directly write into the read buffer and start transmission */
    memcpy(uart->tx_read, data, length);
    HAL_UART_Transmit_DMA(uart->huart, uart->tx_read, length);
  }

  return length;
}


