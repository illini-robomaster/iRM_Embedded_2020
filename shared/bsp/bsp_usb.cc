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

#include "bsp_usb.h"

#include "bsp_error_handler.h"
#include "cmsis_os.h"
#include "usbd_cdc_if.h"

static bsp::USB* usb = nullptr;

namespace bsp {

USB::USB()
    : rx_size_(0),
      rx_pending_(0),
      rx_write_(nullptr),
      rx_read_(nullptr),
      tx_size_(0),
      tx_pending_(0),
      tx_write_(nullptr),
      tx_read_(nullptr) {
  RM_ASSERT_FALSE(usb, "usb initialized twice");
  usb = this;
}

USB::~USB() {
  if (rx_write_)
    delete[] rx_write_;
  if (rx_read_)
    delete[] rx_read_;
  if (tx_write_)
    delete[] tx_write_;
  if (tx_read_)
    delete[] tx_read_;
  usb = nullptr;
}

void USB::SetupTx(uint32_t tx_buffer_size) {
  /* usb tx already setup */
  if (tx_size_ || tx_write_ || tx_read_)
    return;

  tx_size_ = tx_buffer_size;
  tx_pending_ = 0;
  tx_write_ = new uint8_t[tx_buffer_size];
  tx_read_ = new uint8_t[tx_buffer_size];
}

void USB::SetupRx(uint32_t rx_buffer_size) {
  /* usb rx already setup */
  if (rx_size_ || rx_write_ || rx_read_)
    return;

  rx_size_ = rx_buffer_size;
  rx_pending_ = 0;
  rx_write_ = new uint8_t[rx_buffer_size];
  rx_read_ = new uint8_t[rx_buffer_size];
}

uint32_t USB::Read(uint8_t** data) {
  taskENTER_CRITICAL();
  uint32_t length = rx_pending_;
  *data = rx_write_;
  /* swap read / write buffer */
  uint8_t* tmp = rx_write_;
  rx_write_ = rx_read_;
  rx_read_ = tmp;
  rx_pending_ = 0;
  taskEXIT_CRITICAL();
  return length;
}

uint32_t USB::Write(uint8_t* data, uint32_t length) {
  taskENTER_CRITICAL();
  if (length > tx_size_)
    length = tx_size_;
  /* try to transmit the data */
  uint8_t status = CDC_Transmit_FS(data, length);
  if (status == USBD_BUSY || tx_pending_) {
    /* usb not available, queuing up data */
    if (length + tx_pending_ > tx_size_) {
      length = tx_size_ - tx_pending_;
      RM_EXPECT_TRUE(1, "usb data transmission truncated");
    }
    memcpy(tx_write_ + tx_pending_, data, length);
    tx_pending_ += length;
  }
  taskEXIT_CRITICAL();
  return length;
}

void USB::TxCompleteCallback() {
  uint8_t* tmp;
  UBaseType_t isrflags = taskENTER_CRITICAL_FROM_ISR();
  /* check if any data is waiting to be transmitted */
  if (tx_pending_) {
    /* swap read / write buffer */
    tmp = tx_read_;
    tx_read_ = tx_write_;
    tx_write_ = tmp;
    /* initiate new transmission call for pending data */
    CDC_Transmit_FS(tx_read_, tx_pending_);
    /* clear the number of pending bytes */
    tx_pending_ = 0;
  }
  taskEXIT_CRITICAL_FROM_ISR(isrflags);
}

void USB::RxCompleteCallback() {}

uint32_t USB::QueueUpRxData(const uint8_t* data, uint32_t length) {
  if (length + rx_pending_ > rx_size_) {
    length = rx_size_ - rx_pending_;
    RM_EXPECT_TRUE(1, "usb data reception truncated");
  }
  memcpy(rx_write_ + rx_pending_, data, length);
  rx_pending_ += length;
  return length;
}

void TxCompleteCallbackWrapper() {
  if (usb)
    usb->TxCompleteCallback();
}

void RxCompleteCallbackWrapper(uint8_t* data, uint32_t length) {
  usb->QueueUpRxData(data, length);
  usb->RxCompleteCallback();
}

} /* namespace bsp */

void RM_USB_TxCplt_Callback() {
  bsp::TxCompleteCallbackWrapper();
}

void RM_USB_RxCplt_Callback(uint8_t* data, uint32_t length) {
  bsp::RxCompleteCallbackWrapper(data, length);
}
