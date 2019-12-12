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

#include "bsp_gpio.h"
#include "FreeRTOS.h"

gpio_t* gpio_init(gpio_t *gpio, GPIO_TypeDef *group, uint16_t pin) {
  if (!gpio)
    gpio = pvPortMalloc(sizeof(gpio_t));

  gpio->group = group;
  gpio->pin   = pin;
  gpio->state = LOW;

  return gpio;
}

void gpio_low(gpio_t *gpio) {
  HAL_GPIO_WritePin(gpio->group, gpio->pin, GPIO_PIN_RESET);
  gpio->state = LOW;
}

void gpio_high(gpio_t *gpio) {
  HAL_GPIO_WritePin(gpio->group, gpio->pin, GPIO_PIN_SET);
  gpio->state = HIGH;
}

void gpio_toggle(gpio_t *gpio) {
  HAL_GPIO_TogglePin(gpio->group, gpio->pin);
  gpio->state ^= 1;
}

gpio_state_t gpio_read(gpio_t *gpio) {
  gpio->state = (gpio_state_t)HAL_GPIO_ReadPin(gpio->group, gpio->pin);
  return gpio->state;
}

