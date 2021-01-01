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
#include "bsp_print.h"
#include "cmsis_os.h"
#include "controller.h"
#include "main.h"
#include "motor.h"

#define KEY_GPIO_GROUP GPIOB
#define KEY_GPIO_PIN GPIO_PIN_2

#define TARGET_SPEED 80

bsp::CAN *can1 = NULL;
control::MotorCANBase *motor = NULL;

void RM_RTOS_Init() {
  print_use_uart(&huart8);

  can1 = new bsp::CAN(&hcan1, 0x201);
  motor = new control::Motor3508(can1, 0x201);
}

void RM_RTOS_Default_Task(const void *args) {
  UNUSED(args);
  control::MotorCANBase *motors[] = {motor};
  control::PIDController pid(20, 8, 0);

  bsp::GPIO key(KEY_GPIO_GROUP, GPIO_PIN_2);

  float target;

  while (1) {
    if (key.Read())
      target = TARGET_SPEED;
    else
      target = 0;

    motor->SetOutput(pid.ComputeOutput(motor->GetOmegaDelta(target)));
    control::MotorCANBase::TransmitOutput(motors, 1);
    motor->PrintData();
    osDelay(10);
  }
}
