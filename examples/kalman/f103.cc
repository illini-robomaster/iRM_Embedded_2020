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
/**
 * Due to size of FLASH for STM32f103C8T6 (64kB), we can't load everything at the same time
 * Use Macros to test different parts
 * Don't deploy Kalman filter on STM32f103 series.
 * It can only test if the Kalman filter runs, but not if it works.
 */
//#define DIFF_MATRIX   // test different matrix
//#define TEST_ESTIMATE // test EstimateNext()
#define TEST_UPDATE     // test Process()

#include <Eigen/Dense>

#include "bsp_print.h"
#include "bsp_kalman.h"
#include "cmsis_os.h"
#include "main.h"


static osThreadId_t eigen_task_handle;
const osThreadAttr_t eigen_task_thread_attr = {
    .name = "eigenTask",
    .attr_bits = 0,
    .cb_mem = 0,
    .cb_size = 0,
    .stack_mem = 0,
    .stack_size = 256 * 4,
    .priority = osPriorityNormal,
    .tz_module = 0,
    .reserved = 0,
};

static bsp::Kalman<2,2,2> *filter;

void eigen_task(void* arguments) {
  UNUSED(arguments);
  print_use_uart(&huart1);

  Eigen::Matrix2f F;
  F.setIdentity(2, 2);
#ifdef DIFF_MATRIX
  Eigen::Matrix<float,2,2> H;
  H << 1, -1, 1, 0;
  Eigen::Matrix<float,2,2> B;
  B << 1, 0, 0, 1; 
  filter = new bsp::Kalman<2,2,2>(F, H, B); // Kalman(F, H, B, Q, R) also works
#else
  filter = new bsp::Kalman<2,2,2>(F, F, F); // H = F and B = F to save memory
#endif

  Eigen::Vector2f x;

#ifdef TEST_ESTIMATE
  Eigen::Matrix2f P;
  x << 10, 0;
  P.setIdentity(2, 2);
  filter->SetInit(x, P); // If don't set, x is Zero and P is Identity
  Eigen::Vector2f esti = filter->EstimateNext();     // estimate without observation
  print("[%.4f]\r\n[%.4f]\r\n", esti(0), esti(1));
  Eigen::Vector2f u;
  u << 1, 1;
  esti = filter->EstimateNext(u);  //estimate with observation
  print("Estimation = [%.4f]\r\n[%.4f]\r\n", esti(0), esti(1));
#endif

#ifdef TEST_UPDATE
  x << 10, 10;  //use x as observation
  filter->Process(x);    // = filter->Predict( input(optional) );
			 // + filter->Update( observation(required) ); 
  x = filter->GetState();
  print("[%.4f]\r\n[%.4f]\r\n", x(0), x(1)); 
#endif
}

extern "C" void RM_RTOS_Threads_Init() {
  eigen_task_handle = osThreadNew(eigen_task, NULL, &eigen_task_thread_attr);
}
