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
 
static osThreadId servo_test_task_handle;
static remote::DBUS *dbus;

void servo_test_task(void const *argu) {
  UNUSED(argu);
  print_use_uart(&huart8);

  dbus = new remote::DBUS(&huart1);
  LinearMapper<int, int> mapper(-660, 660, -10, 10);

  bsp::PWM servo1(&htim4, 1, 1000000, 50, 1400);
  bsp::PWM servo2(&htim4, 2, 1000000, 50, 1400);

  servo1.Start();
  servo2.Start();
  
  const uint32_t pw_min = 400;
  const uint32_t pw_max = 2400;
  uint32_t pw1 = 1400;
  uint32_t pw2 = 1400;
  while (true) {
    pw1 += mapper.Map(dbus->ch3);
    pw2 += mapper.Map(dbus->ch1);
    pw1 = clip<uint32_t>(pw1, pw_min, pw_max);
    pw2 = clip<uint32_t>(pw2, pw_min, pw_max);

    servo1.SetPulseWidth(pw1);
    servo2.SetPulseWidth(pw2);

    print("Pulse Width 1: %lu us Pulse Widht 2: %lu us\r\n", pw1, pw2);
    osDelay(50);
  }
}

void RM_RTOS_Threads_Init(void) {
  osThreadDef(servoTestTask, servo_test_task, osPriorityNormal, 0, 1024);
  servo_test_task_handle = osThreadCreate(osThread(servoTestTask), NULL);
}
