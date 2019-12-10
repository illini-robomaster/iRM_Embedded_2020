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

#include "cmsis_os.h"

#include "main.h"
#include "tim.h"

#include "bsp_uart.h"

static uart_t uart8;

void RM_RTOS_Init(void) {
  uart_init(&uart8, &huart8, 30, 30);
}

void RM_RTOS_Default_Task(const void *argument) {
  uint32_t length;
  uint8_t *data;

  UNUSED(argument);

  while (1) {
    data = uart_read(&uart8, &length);
    data[length] = '\0';
    if (length) {
      uart_write(&uart8, data, length);
      osDelay(5);
    }
  }
}
