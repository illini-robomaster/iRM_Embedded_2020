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

namespace bsp {

static UART *uarts[MAX_NUM_UARTS] = { 0 };
static size_t num_uarts = 0;

/* check if huart is associated with any initialized uart_t structs */
static inline bool uart_handle_exists(UART_HandleTypeDef *huart) {
  for (size_t i = 0; i < num_uarts; ++i)
    if (uarts[i]->Uses(huart))
      return true;

  return false;
}

/* get initialized uart_t instance given its huart handle struct */
static inline UART* find_uart_instance(UART_HandleTypeDef *huart) {
  for (size_t i = 0; i < num_uarts; ++i)
    if (uarts[i]->Uses(huart))
      return uarts[i];

  return NULL;
}

/* modified version of HAL_UART_Receive_DMA that utilize double buffer mode */
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

/* tx dma complete -> check for pending message to keep transmitting */
void TxCompleteCallbackWrapper(UART_HandleTypeDef *huart) {
  UART *uart = find_uart_instance(huart);
  if (!uart)
    return;
  uart->TxCompleteCallback();
}

/* rx idle line detected -> trigger rx callback */
void RxCompleteCallbackWrapper(UART_HandleTypeDef *huart) {
  UART *uart = find_uart_instance(huart);
  if (!uart)
    return;

  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) && __HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE)) {
    uart->RxCompleteCallback();
    __HAL_UART_CLEAR_IDLEFLAG(huart);
  }

  // TODO(alvin): add actual error handler in the future, ignore it for now
  if (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_ERR)) {
    __HAL_UART_CLEAR_PEFLAG(huart);
  }
}


UART::UART(UART_HandleTypeDef *huart) : huart_(huart),
    rx_size_(0), rx_data0_(NULL), rx_data1_(NULL),
    tx_size_(0), tx_pending_(0), tx_write_(NULL), tx_read_(NULL) {
  RM_ASSERT_FALSE(uart_handle_exists(huart), "Uart repeated initialization");
  uarts[num_uarts++] = this;
}

UART::~UART() {
  if (rx_data0_)
    delete[] rx_data0_;
  if (rx_data1_)
    delete[] rx_data1_;
  if (tx_write_)
    delete[] tx_write_;
  if (tx_read_)
    delete[] tx_read_;
}

void UART::SetupRx(uint32_t rx_buffer_size) {
  /* uart rx already setup */
  if (rx_size_ || rx_data0_ || rx_data1_)
    return ;

  rx_size_ = rx_buffer_size;
  rx_data0_ = new uint8_t[rx_buffer_size];
  rx_data1_ = new uint8_t[rx_buffer_size];

  /* enable uart rx dma transfer in back ground */
  uart_receive_dma_double_buffer(huart_, rx_data0_, rx_data1_, rx_size_);

  /* UART IDLE Interrupt can notify application of data reception ASAP */
  __HAL_UART_CLEAR_FLAG(huart_, UART_FLAG_IDLE);
  __HAL_UART_ENABLE_IT(huart_, UART_IT_IDLE);
}

void UART::SetupTx(uint32_t tx_buffer_size) {
  /* uart tx already setup */
  if (tx_size_ || tx_write_ || tx_read_)
    return;

  tx_size_ = tx_buffer_size;
  tx_pending_ = 0;
  tx_write_ = new uint8_t[tx_buffer_size];
  tx_read_ = new uint8_t[tx_buffer_size];

  HAL_UART_RegisterCallback(huart_, HAL_UART_TX_COMPLETE_CB_ID, TxCompleteCallbackWrapper);
}

int32_t UART::Read(uint8_t **data) {
  if (!data)
    return -1;
  /* capture pending bytes and perform hardware buffer switch */
  int32_t length;
  taskENTER_CRITICAL();
  __HAL_DMA_DISABLE(huart_->hdmarx);
  length = rx_size_ - __HAL_DMA_GET_COUNTER(huart_->hdmarx);
  __HAL_DMA_SET_COUNTER(huart_->hdmarx, rx_size_);
  huart_->hdmarx->Instance->CR ^= DMA_SxCR_CT;
  __HAL_DMA_ENABLE(huart_->hdmarx);
  taskEXIT_CRITICAL();

  /* return the buffer pointer currently not being used by DMA transfer */
  if (huart_->hdmarx->Instance->CR & DMA_SxCR_CT)
    *data = rx_data0_; // DMA is transferring data into rx_data1
  else
    *data = rx_data1_; // DMA is transferring data into rx_data0

  return length;
}

int32_t UART::ReadFromISR(uint8_t **data) {
  if (!data)
    return -1;
  /* capture pending bytes and perform hardware buffer switch */
  int32_t length;
  UBaseType_t isrflags = taskENTER_CRITICAL_FROM_ISR();
  __HAL_DMA_DISABLE(huart_->hdmarx);
  length = rx_size_ - __HAL_DMA_GET_COUNTER(huart_->hdmarx);
  __HAL_DMA_SET_COUNTER(huart_->hdmarx, rx_size_);
  huart_->hdmarx->Instance->CR ^= DMA_SxCR_CT;
  __HAL_DMA_ENABLE(huart_->hdmarx);
  taskEXIT_CRITICAL_FROM_ISR(isrflags);

  /* return the buffer pointer currently not being used by DMA transfer */
  if (huart_->hdmarx->Instance->CR & DMA_SxCR_CT)
    *data = rx_data0_; // DMA is transferring data into rx_data1
  else
    *data = rx_data1_; // DMA is transferring data into rx_data0

  return length;
}

int32_t UART::Write(const uint8_t *data, uint32_t length) {
  taskENTER_CRITICAL();
  if (huart_->gState == HAL_UART_STATE_BUSY_TX || tx_pending_) {
    /* uart tx currently transmitting -> atomically queue up new data */
    if (length + tx_pending_ > tx_size_)
      length = tx_size_ - tx_pending_;
    memcpy(tx_write_ + tx_pending_, data, length);
    tx_pending_ += length;
  }
  else {
    if (length > tx_size_)
      length = tx_size_;
    /* directly write into the read buffer and start transmission */
    memcpy(tx_read_, data, length);
    HAL_UART_Transmit_DMA(huart_, tx_read_, length);
  }
  taskEXIT_CRITICAL();

  return length;
}

bool UART::Uses(UART_HandleTypeDef *huart) {
  return huart == huart_;
}

void UART::TxCompleteCallback() {
  uint8_t *tmp;
  UBaseType_t isrflags = taskENTER_CRITICAL_FROM_ISR();
  /* check if any data is waiting to be transmitted */
  if (tx_pending_) {
    /* swap read / write buffer */
    tmp = tx_read_;
    tx_read_ = tx_write_;
    tx_write_ = tmp;
    /* initiate new transmission call for pending data */
    HAL_UART_Transmit_DMA(huart_, tx_read_, tx_pending_);
    /* clear the number of pending bytes */
    tx_pending_ = 0;
  }
  taskEXIT_CRITICAL_FROM_ISR(isrflags);
}

void UART::RxCompleteCallback() {}

} /* namespace bsp */

/* overwrite the weak function defined in board specific usart.c to handle IRQ requests */
void RM_UART_IRQHandler(UART_HandleTypeDef *huart) {
  bsp::RxCompleteCallbackWrapper(huart);
}

