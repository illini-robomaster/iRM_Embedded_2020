/****************************************************************************
 *                                                                          *
 *  Copyright (C) 2021 RoboMaster.                                          *
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

#include <Eigen/Dense>

#include "bsp_print.h"
#include "cmsis_os.h"

static osThreadId eigen_task_handle;

void eigen_task(const void* arguments) {
  UNUSED(arguments);

  print_use_usb();
  osDelay(5000);  // wait for USB connection

  Eigen::Matrix2f mat;
  Eigen::Matrix2f mat_acc = Eigen::Matrix2f::Identity();
  mat << 1, 1, 1, 0;  // accumulative multiplication of this yields Fibonacci Sequence

  for (int i = 0; i < 30; ++i) {
    osDelay(1000);

    print("\r\n[Iter %d]: \r\n[%.4f %.4f]\r\n[%.4f %.4f]\r\n", i, mat_acc(0, 0), mat_acc(0, 1),
          mat_acc(1, 0), mat_acc(1, 1));

    mat_acc *= mat;
  }
}

extern "C" void RM_RTOS_Threads_Init() {
  osThreadDef(eigenTask, eigen_task, osPriorityNormal, 0, 1024);
  eigen_task_handle = osThreadCreate(osThread(eigenTask), NULL);
}
