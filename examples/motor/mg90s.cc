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

#include "bsp_pwm.h"
#include "bsp_gpio.h"

#define KEY_GPIO_GROUP  GPIOB
#define KEY_GPIO_PIN    GPIO_PIN_2

bsp::PWM *servo = NULL;

void RM_RTOS_Init() {
  servo = new bsp::PWM(&htim4, 1, 1000000, 50, 1400);
}

void RM_RTOS_Default_Task(const void *args) {
  UNUSED(args);

  bsp::GPIO key(KEY_GPIO_GROUP, GPIO_PIN_2);
  servo->Start();

  const uint32_t min_pulse_width = 400;
  uint32_t pulse_width_offset = 1000;
  while (1) {
    if (key.Read()) {
      pulse_width_offset = (pulse_width_offset + 100) % 2000;
      servo->SetPulseWidth(min_pulse_width + pulse_width_offset);
      osDelay(500);
    }
  }
}

