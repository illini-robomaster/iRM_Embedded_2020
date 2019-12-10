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

#include "bsp_buzzer.h"

static pwm_t    buzzer_pwm;
static uint8_t  buzzer_initialized = 0;

int buzzer_init(TIM_HandleTypeDef *htim, uint32_t channel, uint32_t clock_freq) {
  if (!htim)
    return -1;

  pwm_init(&buzzer_pwm, htim, channel, clock_freq, 0, 0); 
  buzzer_initialized = 1;
  return 0;
}

void buzzer_sing_tone(const buzzer_freq_t freq) {
  if (!buzzer_initialized)
    return;

  pwm_set_freq(&buzzer_pwm, freq);
  pwm_set_pulse_width(&buzzer_pwm, (buzzer_pwm.htim->Instance->ARR + 1) / 2);
}

void buzzer_sing_song(const buzzer_freq_t *freq, const uint32_t *delay) {
  while (*freq != Finish) {
    buzzer_sing_tone(*freq++);
    HAL_Delay(*delay++);
  }
}
