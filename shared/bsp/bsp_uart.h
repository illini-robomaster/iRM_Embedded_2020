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

/**
 * @brief per uart abstraction
 */
typedef struct uart_s {
  UART_HandleTypeDef  *huart;
  /* rx */
  uint32_t            rx_size;
  uint8_t             *rx_data0;  // user can access one of them while the other 
  uint8_t             *rx_data1;  // is being written by DMA
  uart_callback_t     rx_callback;
  /* tx */
  uint32_t            tx_size;
  uint32_t            tx_pending;
  uint8_t             *tx_write;  // user can write into tx_write
  uint8_t             *tx_read;   // actual transmission is initiated with tx_read 
} uart_t;

/**
 * @brief initialize an uart channel for non blocking transmission
 *
 * @param uart            pointer to uart struct (could be None)
 * @param huart           pointer to HAL uart handle
 * @param tx_buffer_size  transmission buffer size (burst transmission calls will
 *                        be queued into this buffer)
 *
 * @return initialized uart_t struct
 *
 * @note initialization does not activate rx dma functionalities
 *
 */
uart_t* uart_init(uart_t *uart, UART_HandleTypeDef *huart, uint32_t tx_buffer_size);

/**
 * @brief activate uart rx to receive message with DMA in the background
 *
 * @param uart            pointer to uart struct
 * @param rx_buffer_size  receive buffer size (all data that has not been read 
 *                        out is queued into this buffer)
 * @param callback        optional callback function to handle rx data inside IRQ context
 *
 * @return 0 for success, -1 for failuer (NULL pointer or calling more than once)
 */
int8_t uart_start_receiving(uart_t *uart, uint32_t rx_buffer_size, uart_callback_t callback);

/**
 * @brief get pending data from the receiver queue
 *
 * @param uart    pointer to uart struct
 * @param length  pointer to uint32_t to store the length of the received data
 *
 * @return pointer to the data buffer, NULL if failed.
 *
 * @note returned data pointer will be invalidated by calling this function again
 * @note to optimize for performance, the implementation is NOT thread safe!
 */
uint8_t* uart_read(uart_t *uart, uint32_t *length);

/**
 * @brief transmit known amount of data without blocking
 *
 * @param uart    pointer to uart struct
 * @param data    pointer to data to be transmitted
 * @param length  length to be transmitted [in bytes]
 *
 * @return number of bytes transmitted, -1 if failed
 *
 * @note multiple burst calls to this function can potentially cause tx buffer 
 *       to fill up, so remember to check return value for the actual number
 *       of bytes succesfully transmitted
 * @note to optimize for performance, the implementation is NOT thread safe!
 */
int32_t uart_write(uart_t *uart, uint8_t *data, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* _BSP_UART_H_ */
