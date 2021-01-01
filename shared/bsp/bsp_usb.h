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

#pragma once

#include "main.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

typedef void (*usb_callback_t)(uint8_t *buf, uint32_t len);

/**
 * @brief register an arbitrary function to handle usb rx callback
 *
 * @param callback  a function pointer of type usb_callback_t
 */
void usb_register_callback(const usb_callback_t callback);

/**
 * @brief un-register usb rx callback function
 */
void usb_unregister_callback(void);

/**
 * @brief transmit data via usb
 *
 * @param buf buffer containing data
 * @param len length [in bytes] to transmit
 *
 * @return number of bytes successfully transmitted, -1 if usb busy, -2 if failed
 */
int32_t usb_transmit(uint8_t *buf, uint32_t len);
