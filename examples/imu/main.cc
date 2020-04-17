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
#include "cmsis_os.h"

#include "bsp_gpio.h"
#include "bsp_imu.h"
#include "bsp_os.h"

#include <cstring>

#define ONBOARD_IMU_SPI       hspi5
#define ONBOARD_IMU_CS_GROUP  GPIOF
#define ONBOARD_IMU_CS_PIN    GPIO_PIN_6
#define PRING_UART            huart8

volatile uint32_t start, duration;

static bsp::MPU6500 *imu;
static osThreadId   imu_task_handle;

static char stats[256];


void imu_task(void const *argu) {
  UNUSED(argu);
  uint32_t sys_tick = osKernelSysTick();
  while (true) {
    imu->UpdateData();
    osDelayUntil(&sys_tick, 1);
  }
}

void RM_RTOS_Default_Task(const void *arguments) {
  UNUSED(arguments);
  while (true) {
    vTaskGetRunTimeStats(stats);
    set_cursor(0, 0);
    clear_screen();
    print("Temp: %.4f ", imu->temp);
    print("ACC_X: %.4f ACC_Y: %.4f ACC_Z: %.4f ", imu->acce.x, imu->acce.y, imu->acce.z);
    print("GYRO_X: %.4f GYRO_Y: %.4f GYRO_Z: %.4f", imu->gyro.x, imu->gyro.y, imu->gyro.z);
    print("\r\nCPU Usage:\r\n%s", stats);
    osDelay(100);
  }
}

void RM_RTOS_Init(void) {
  bsp::set_highres_clock_timer(&htim2);
  bsp::GPIO chip_select(ONBOARD_IMU_CS_GROUP, ONBOARD_IMU_CS_PIN);
  imu = new bsp::MPU6500(&ONBOARD_IMU_SPI, chip_select);
  print_use_uart(&PRING_UART);
}


void RM_RTOS_Threads_Init(void) {
  osThreadDef(imuTask, imu_task, osPriorityNormal, 0, 256);
  imu_task_handle = osThreadCreate(osThread(imuTask), NULL);
}

