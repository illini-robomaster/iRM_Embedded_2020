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

#include <memory>

#include "bsp_gpio.h"
#include "bsp_print.h"
#include "cmsis_os.h"
#include "main.h"
#include "motor.h"

#define KEY_GPIO_GROUP GPIOB
#define KEY_GPIO_PIN GPIO_PIN_2

void RM_RTOS_Default_Task(const void* args) {
  UNUSED(args);

  print_use_uart(&huart8);

  auto can1 = std::make_shared<bsp::CAN>(&hcan1, 0x201);
  auto motor = std::make_unique<control::Motor3508>(can1, 0x201);
  auto key = std::make_unique<bsp::GPIO>(KEY_GPIO_GROUP, GPIO_PIN_2);

  const std::vector<control::MotorCANBase*> motors = { motor.get() };

  uint32_t timestamp = osKernelSysTick();
  while (1) {
    if (key->Read())
      motor->SetOutput(800);
    else
      motor->SetOutput(0);
    control::MotorCANBase::TransmitOutput(motors);
    motor->PrintData();
    osDelayUntil(&timestamp, 10);
  }
}
