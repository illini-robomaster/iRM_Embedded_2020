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

#ifndef _BSP_GPIO_H_
#define _BSP_GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "bsp_error_handler.h"
#include "main.h"

/**
 * @ingroup bsp
 * @defgroup bsp_gpio BSP GPIO
 * @{
 */

/**
 * @enum    gpio_state_t
 * @brief   describe gpio state as in high / low
 * @var HIGH    gpio in high state
 * @var LOW     gpio in low state
 */
typedef enum {
  HIGH = 0,
  LOW = 1,
}   gpio_state_t;

/**
 * @struct  gpio_t
 * @brief   hold all necessary information for a gpio pin
 * @var group   gpio group number
 * @var pin     gpio pin number
 * @var state   gpio state
 */
typedef struct {
  GPIO_TypeDef    *group;
  uint16_t        pin;
  gpio_state_t    state;
}   gpio_t;

/**
 * @brief initialize a GPIO instance with a group id and pin number
 *
 * @param gpio  gpio struct to be initialized (could be NULL)
 * @param group pointer to a gpio group id
 * @param pin   gpio pin number
 *
 * @return initialized gpio_t instance
 */
gpio_t* gpio_init(gpio_t *gpio, GPIO_TypeDef *group, uint16_t pin);

/**
 * @brief output low signal to a gpio pin
 * @param gpio pointer to a gpio instance
 */
void gpio_low(gpio_t *gpio);

/**
 * @brief output high signal to a gpio pin
 * @param gpio pointer to a gpio instance
 */
void gpio_high(gpio_t *gpio);

void gpio_toggle(gpio_t *gpio);

/**
 * @brief read in signal from a gpio pin
 * @param gpio pointer to a gpio instance
 */
gpio_state_t gpio_read(gpio_t *gpio);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _BSP_GPIO_H_ */
