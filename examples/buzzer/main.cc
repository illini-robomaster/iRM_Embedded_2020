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

#include "main.h"
#include "tim.h"

#include "bsp_buzzer.h"

static buzzer_freq_t startup[] = {
  Mi3M, Silent, Mi3M, Silent, Mi3M, Silent, 
  Do1M, Silent, Mi3M, Silent, 
  So5M, Silent, So5L, Silent, 
  Finish,
};

static uint32_t startup_delay[] = {
  80, 80, 80, 240, 80, 240,
  80, 80, 80, 240, 
  80, 560, 80, 560,
};

void RM_RTOS_Init(void) {
  buzzer_init(&htim12, 1, 1000000);
  buzzer_sing_song(startup, startup_delay);
}
