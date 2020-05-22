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
 
static osThreadId eye_test_task_handle;
static remote::DBUS *dbus;

void eye_test_task(void const *argu) {
  UNUSED(argu);
  print_use_uart(&huart8);

  dbus = new remote::DBUS(&huart1);

  MG90S left_eye(&htim4, 1, 1100, 1400);
  MG90S right_eye(&htim4, 2, 2100, 1800);

  while (true) {
    left_eye.SetOutput(dbus->swl == remote::UP);
    right_eye.SetOutput(dbus->swr == remote::UP);
    osDelay(50);
  }
}

void RM_RTOS_Threads_Init(void) {
  osThreadDef(eyeTestTask, eye_test_task, osPriorityNormal, 0, 1024);
  eye_test_task_handle = osThreadCreate(osThread(eyeTestTask), NULL);
}

