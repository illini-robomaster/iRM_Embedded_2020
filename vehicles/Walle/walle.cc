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

#include "bsp_print.h"
#include "bsp_pwm.h"
#include "dbus.h"
#include "utils.h"

#include "walle_utils.h"
 
static osThreadId walle_task_handle;
static remote::DBUS *dbus = NULL;
static MG90S *eye_left = NULL;
static MG90S *eye_right = NULL;
static MG90S *arm_left = NULL;
static MG90S *arm_right = NULL;
static MG90S *neck_bottom = NULL;
static MG90S *neck_top = NULL;
static MG90S *neck_rotate = NULL;
static LinearMapper<int16_t, float> dbus_mapper(-660, 660, -.05, .05);
static LinearMapper<int16_t, float> neck_mapper(-660, 660, .01, -.01);

void walle_task(void) {
  while (true) {
    switch (dbus->swl) {
    case remote::UP: // control arm + eyes
      switch (dbus->swr) {
      case remote::UP: // control eye
        eye_left->SetIncrement(dbus_mapper.Map(dbus->ch3));
        eye_right->SetIncrement(dbus_mapper.Map(dbus->ch1));
        break;
      case remote::MID: // control arm
        arm_left->SetIncrement(dbus_mapper.Map(dbus->ch3));
        arm_right->SetIncrement(dbus_mapper.Map(dbus->ch1));
        break;
      case remote::DOWN: // control neck bottom
        neck_bottom->SetIncrement(dbus_mapper.Map(dbus->ch1));
        break;
      }
      break;
    default: // control base + neck
      neck_rotate->SetIncrement(neck_mapper.Map(dbus->ch0));
      neck_top->SetIncrement(dbus_mapper.Map(dbus->ch1));
      break;
    }
    osDelay(10);
  }
}

void walle_init(void const *argu) {
  UNUSED(argu);

  print_use_uart(&huart8);

  dbus        = new remote::DBUS(&huart1);
  eye_left    = new MG90S(&htim4, 1, 1100, 1400);
  eye_right   = new MG90S(&htim4, 2, 2100, 1800);
  arm_left    = new MG90S(&htim4, 3, 1750, 700);
  arm_right   = new MG90S(&htim4, 4, 1200, 2250);
  neck_bottom = new MG90S(&htim5, 1, 2000, 1000);
  neck_top    = new MG90S(&htim5, 2, 750, 2350);
  neck_rotate = new MG90S(&htim5, 3, 675, 2075);

  walle_task();
}

void RM_RTOS_Threads_Init(void) {
  osThreadDef(walleTask, walle_init, osPriorityNormal, 0, 1024);
  walle_task_handle = osThreadCreate(osThread(walleTask), NULL);
}


