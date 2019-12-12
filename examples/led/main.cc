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

#include "bsp_gpio.h"

#define LED_RED_Pin GPIO_PIN_11
#define LED_RED_GPIO_Port GPIOE

#define LED_GREEN_Pin GPIO_PIN_14
#define LED_GREEN_GPIO_Port GPIOF

static gpio_t red_led, green_led;

void RM_RTOS_Init(void) {
  gpio_init(&red_led, LED_RED_GPIO_Port, LED_RED_Pin);
  gpio_init(&green_led, LED_GREEN_GPIO_Port, LED_GREEN_Pin);
  gpio_high(&red_led);
  gpio_low(&green_led);
}

void RM_RTOS_Default_Task(const void *args) {
  UNUSED(args);
  while (1) {
    gpio_toggle(&red_led);
    gpio_toggle(&green_led);
    osDelay(500);
  }
}
