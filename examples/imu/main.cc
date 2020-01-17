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

#define ONBOARD_IMU_SPI       hspi5
#define ONBOARD_IMU_CS_GROUP  GPIOF
#define ONBOARD_IMU_CS_PIN    GPIO_PIN_6
#define PRING_UART            huart8

volatile uint32_t start, duration;

BSP::MPU6500 *imu;

void RM_RTOS_Init(void) {
  BSP::GPIO chip_select(ONBOARD_IMU_CS_GROUP, ONBOARD_IMU_CS_PIN);
  imu = new BSP::MPU6500(&ONBOARD_IMU_SPI, chip_select);
  print_use_uart(&PRING_UART);
}

void RM_RTOS_Default_Task(const void *arguments) {
  UNUSED(arguments);
  while (true) {
    imu->UpdateData();
    print("Temp: %f ", imu->temp);
    print("ACC_X: %f ACC_Y: %f ACC_Z: %f ", imu->acce.x, imu->acce.y, imu->acce.z);
    print("GYRO_X: %f GYRO_Y: %f GYRO_Z: %f\r\n", imu->gyro.x, imu->gyro.y, imu->gyro.z);
    osDelay(100);
  }
}
