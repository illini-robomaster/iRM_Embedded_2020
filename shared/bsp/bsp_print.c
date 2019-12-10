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

#include "bsp_print.h"
#include "bsp_usb.h"

#include "main.h"
#include "printf.h" // third party tiny-printf implemnetations

#define MAX_PRINT_LEN 80

int print(const char *format, ...) {
#ifdef NDEBUG
  UNUSED(format);
  return 0;
#else // == #ifdef DEBUG
  char    buffer[MAX_PRINT_LEN];
  va_list args; 
  int     length;

  va_start(args, format);
  length = vsnprintf(buffer, MAX_PRINT_LEN, format, args);
  va_end(args);

  return usb_transmit((uint8_t*)buffer, length);  
#endif // #ifdef NDEBUG
}
