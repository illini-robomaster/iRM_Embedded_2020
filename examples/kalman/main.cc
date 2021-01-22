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
#include <Eigen/Dense>

#include "bsp_print.h"
#include "bsp_kalman.h"
#include "cmsis_os.h"

static bsp::Kalman<2,2,2> *filter;


void RM_RTOS_Init(void) {
  Eigen::Matrix2f F;
  F.setIdentity(2, 2);
  Eigen::Matrix<float,2,2> H;
  H << 1, -1, 1, 0;
  Eigen::Matrix<float,2,2> B;
  B << 1, 0, 0, 1;
  
  filter = new bsp::Kalman(F, H, B); // Kalman(F, H, B, Q, R) also works
  
  Eigen::Vector2f x;
  Eigen::Matrix2f P;
  x << 10, 0;
  P.setIdentity(2, 2);
  filter->SetInit(x, P); // If don't set, x should be zero and P should be Identity
 
}

void RM_RTOS_Default_Task(const void* arguments) {
  UNUSED(arguments);

  print_use_usb();
  osDelay(5000);  // wait for USB connection

 
  Eigen::Vector2f esti = filter->GetEstimate();     // estimate without observation
  print("[%.4f]\r\n[%.4f]\r\n", esti(0), esti(1));
  
  Eigen::Vector2f u;
  u << 1, 1;
  esti = filter->GetEstimate(u);
  print("Estimation = [%.4f]\r\n[%.4f]\r\n", esti(0), esti(1)); // should output [1, 1]

  Eigen::Vector2f obse;
  obse << 10, 10;
  filter->Predict();  // Predict(u) also works
  filter->Update(obse);
  Eigen::Vector2f x = filter->GetState();
  print("[%.4f]\r\n[%.4f]\r\n", x(0), x(1)); // should output something between [1,1] and [10, 10]
}
