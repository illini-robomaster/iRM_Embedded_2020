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

static usb_callback_t rm_usb_rx_callback = NULL;

void usb_register_callback(usb_callback_t callback) {
  rm_usb_rx_callback = callback;
}

void usb_unregister_callback(void) {
  rm_usb_rx_callback = NULL;
}

int usb_transmit(uint8_t *buf, uint32_t len) {
  uint8_t status = CDC_Transmit_FS(buf, (uint16_t)len);
  if (status == USBD_OK) {
    return len;
  }
  else if (status == USBD_BUSY) {
    return -1;
  }
  else { // status == USBD_FAIL (shouldn't get here)
    return -2;
  }
}

void RM_USB_RxCplt_Callback(uint8_t *Buf, uint32_t Len) {
  if (rm_usb_rx_callback) {
    rm_usb_rx_callback(Buf, Len);
  }
}
