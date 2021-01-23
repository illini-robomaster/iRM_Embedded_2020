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
#include "bsp_uart.h"
#include "cmsis_os.h"

void RM_RTOS_Default_Task(const void* args) {
  UNUSED(args);
  //auto uart = bsp::UART(&UART_HANDLE);

  print_use_uart(&huart1);
  
  Eigen::Matrix2f mat = Eigen::Matrix2f::Identity();
  Eigen::Matrix2f mat_acc = Eigen::Matrix2f::Identity();
  //mat << 1, 1, 1, 0;  // accumulative multiplication of this yields Fibonacci Sequence

  for (int i = 0; i < 10; ++i) {
    osDelay(500);
    print("1/n");
    osDelay(500);
    print("%.4f, %.4f, %.4f, %.4f \n", mat_acc(0,0), mat_acc(0,1), mat_acc(1,0), mat_acc(1,1));
    osDelay(2000);    
    print("2/n");
    osDelay(500);    
    mat_acc = mat_acc + mat;
    osDelay(500);    
    print("3/n");
    osDelay(500);        
//print("%d, %d, %d, %d \n", mat_acc(0,0), mat_acc(0,1), mat_acc(1,0), mat_acc(1,1));
    //print("\r\n[Iter %d]: \r\n[%.4f %.4f]\r\n[%.4f %.4f]\r\n", i, mat_acc(0, 0), mat_acc(0, 1),
          //mat_acc(1, 0), mat_acc(1, 1));
    
  }
}
