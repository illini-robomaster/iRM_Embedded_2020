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

#ifndef _BSP_UART_H_
#define _BSP_UART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usart.h"

struct uart_s;

typedef void (*uart_callback_t)(struct uart_s*);

typedef struct uart_s {
  UART_HandleTypeDef  *huart;
  /* rx */
  uint32_t            rx_size;
  uint8_t             *rx_data0;
  uint8_t             *rx_data1;
  uart_callback_t     rx_callback;
  /* tx */
  uint32_t            tx_size;
  uint32_t            tx_pending;
  uint8_t             *tx_write;
  uint8_t             *tx_read;
  uart_callback_t     tx_callback;
} uart_t;

uart_t* uart_init(uart_t *uart, UART_HandleTypeDef *huart, uint32_t rx_size, uint32_t tx_size);

uint8_t* uart_read(uart_t *uart, uint32_t *length);

int32_t uart_write(uart_t *uart, uint8_t *data, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* _BSP_UART_H_ */
