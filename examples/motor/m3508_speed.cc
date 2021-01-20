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
#define CONTROL_DT   10

void RM_RTOS_Default_Task(const void* args) {
  UNUSED(args);

  print_use_uart(&huart8);

  auto can1 = std::make_shared<bsp::CAN>(&hcan1, 0x201);
  auto motor = std::make_unique<control::Motor3508>(can1, 0x201);
  auto pid = std::make_unique<control::PIDController>(3, 0, 0, CONTROL_DT / 1e3, 40);
  auto key = std::make_unique<bsp::GPIO>(KEY_GPIO_GROUP, GPIO_PIN_2);

  while (1) {
    if (key->Read()) {
      float target = TARGET_SPEED;
      motor->SetOutput(pid->ComputeOutput(motor->GetOmegaDelta(target)));
    } else {
      motor->SetOutput(0);
    }

    motor->PrintData();
    control::MotorCANBase::TransmitOutput({motor.get()});
    osDelay(CONTROL_DT);
  }
}
