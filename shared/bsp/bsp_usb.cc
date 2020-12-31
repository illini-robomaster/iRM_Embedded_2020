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

#include "usbd_cdc_if.h"

#include "cmsis_os.h"

static bsp::USB *usb = nullptr;

namespace bsp {

USB::USB(bool use_callback) : hcdc(nullptr), use_callback(use_callback),
    rx_size_(0), rx_pending_(0), rx_write_(nullptr), rx_read_(nullptr),
    tx_size_(0), tx_pending_(0), tx_write_(nullptr), tx_read_(nullptr) {
  delete usb;
  usb = this;
}

USB::~USB() {
  delete[] rx_write_;
  delete[] rx_read_;
  delete[] tx_write_;
  delete[] tx_read_;
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
  /* use callback instead of receiver buffer */
  if (use_callback)
    return;

  /* usb rx already setup */
  if (rx_size_ || rx_write_ || rx_read_)
    return;

  rx_size_ = rx_buffer_size;
  rx_pending_ = 0;
  rx_write_ = new uint8_t[rx_buffer_size];
  rx_read_ = new uint8_t[rx_buffer_size];
}

uint32_t USB::Read(uint8_t *&data) {
  taskDISABLE_INTERRUPTS();
  uint32_t length = rx_pending_;
  data = rx_write_;
  /* swap read / write buffer */
  uint8_t* tmp = rx_write_;
  rx_write_ = rx_read_;
  rx_read_ = tmp;
  rx_pending_ = 0;
  taskENABLE_INTERRUPTS();
  return length;
}

uint32_t USB::Write(uint8_t *data, uint32_t length) {
  taskDISABLE_INTERRUPTS();
  if (!hcdc)
    hcdc = RM_USB_Device();
  if (hcdc->TxState == USBD_BUSY || tx_pending_) {
    if (length + tx_pending_ > tx_size_) {
      length = tx_size_ - tx_pending_;
    } else {
      memcpy(tx_write_ + tx_pending_, data, length);
      tx_pending_ += length;
    }
  } else {
    if (length > tx_size_)
      length = tx_size_;
    /* directly write into the read buffer and start transmission */
    memcpy(tx_read_, data, length);
    CDC_Transmit_FS(tx_read_, length);
  }
  taskENABLE_INTERRUPTS();
  return length;
}

void USB::TxCompleteCallback() {
  uint8_t *tmp;
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

void USB::RxCompleteCallback(uint8_t *data, uint32_t length) {
  UNUSED(data);
  UNUSED(length);
}

uint32_t USB::QueueUpRxData(uint8_t* data, uint32_t length) {
  if (length + rx_pending_ > rx_size_) {
    length = rx_size_ - rx_pending_;
  } else {
    memcpy(rx_write_ + rx_pending_, data, length);
    rx_pending_ += length;
  }
  return length;
}

bool USB::UseCallback() const {
  return use_callback;
}

} /* namespace bsp */

void RM_USB_TxCplt_Callback() {
  if (usb)
    usb->TxCompleteCallback();
}

void RM_USB_RxCplt_Callback(uint8_t* Buf, uint32_t Len) {
  if (usb->UseCallback()) {
    /* have registered callback */
    usb->RxCompleteCallback(Buf, Len);
  } else {
    /* does not have registered callback, queue up data */
    usb->QueueUpRxData(Buf, Len);
  }
}
