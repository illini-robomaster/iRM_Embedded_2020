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

#ifndef _BSP_ERROR_HANDLER_H_
#define _BSP_ERROR_HANDLER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_print.h"

/**
 * @ingroup bsp
 * @defgroup bsp_error_handler BSP Error Handler
 * @{
 */

#define BSP_DEBUG   print("[DEBUG] %s:%d ", __FUNCTION__, __LINE__)

/**
 * Handle error condition printf etc.
 *
 * @param  file       Which file the error occured
 * @param  line       Which line the error occured
 * @param  msg        Message want to print
 * @author Nickel_Liang
 * @date   2018-04-15
 */
void bsp_error_handler(const char* func, int line, const char* msg);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _BSP_ERROR_HANDLER_H_ */
