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
  auto pid = std::make_unique<control::PIDController>(20, 0.01, 1, CONTROL_DT / 1e3, 20);
  auto key = std::make_unique<bsp::GPIO>(KEY_GPIO_GROUP, KEY_GPIO_PIN);

  const std::vector<control::MotorCANBase*> motors = { motor.get() };

  while (1) {
    const uint32_t start = osKernelSysTick();
    const float target = key->Read() ? TARGET_SPEED : 0;

    motor->SetOutput(pid->ComputeOutput(motor->GetOmegaDelta(target)));
    control::MotorCANBase::TransmitOutput(motors);
    motor->PrintData();
    osDelayUntil(start + CONTROL_DT);
  }
}
