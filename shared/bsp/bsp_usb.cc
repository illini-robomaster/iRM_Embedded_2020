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

static bsp::USB *usb;

namespace bsp {

static inline USB* GetUsbInstance() {
  return usb;
}

USB::USB() : usb_rx_callback_(nullptr),
    tx_size_(0), tx_pending_(0), tx_write_(nullptr), tx_read_(nullptr) {
  hcdc = RM_USB_Device();
  delete usb;
  usb = this;
}

USB::~USB() {
    delete[] tx_write_;
    delete[] tx_read_;
}

void USB::SetupTx(uint32_t tx_buffer_size) {
  /* uart tx already setup */
  if (tx_size_ || tx_write_ || tx_read_)
    return;

  tx_size_ = tx_buffer_size;
  tx_pending_ = 0;
  tx_write_ = new uint8_t[tx_buffer_size];
  tx_read_ = new uint8_t[tx_buffer_size];
}

uint32_t USB::Write(uint8_t *data, uint32_t length) {
  taskENTER_CRITICAL();
  /* queue up new data */
  if (hcdc->TxState == USBD_BUSY || tx_pending_) {
    if (length + tx_pending_ > tx_size_)
      length = tx_size_ - tx_pending_;
    memcpy(tx_write_ + tx_pending_, data, length);
    tx_pending_ += length;
  } else {
    if (length > tx_size_)
      length = tx_size_;
    /* directly write into the read buffer and start transmission */
    memcpy(tx_read_, data, length);
    CDC_Transmit_FS(tx_read_, length);
  }
  taskEXIT_CRITICAL();
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

void USB::RegisterRxCompleteCallback(const usb_callback_t callback) {
  usb_rx_callback_ = callback;
}

void USB::UnregisterRxCompleteCallback() {
  usb_rx_callback_ = nullptr;
}

} /* namespace bsp */

void RM_USB_TxCplt_Callback(uint8_t *Buf, uint32_t Len) {
  if (usb)
    usb->TxCompleteCallback();
  UNUSED(Buf);
  UNUSED(Len);
}

void RM_USB_RxCplt_Callback(uint8_t *Buf, uint32_t Len) {
  if (usb->usb_rx_callback_)
    usb->usb_rx_callback_(Buf, Len);
}
