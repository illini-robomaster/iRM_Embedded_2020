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

#ifndef _BSP_PRINT_H_
#define _BSP_PRINT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usart.h"

#include "printf.h" // third party tiny-printf implemnetations

/**
 * @brief initialize print utility via UART
 *
 * @param huart uart handle
 */
void bsp_print_init(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif // _BSP_PRINT_H_
