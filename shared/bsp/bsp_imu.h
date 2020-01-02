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

#pragma once

#include "gpio.h"
#include "spi.h"

#include "bsp_gpio.h"

namespace BSP {

typedef struct {
  float x;
  float y;
  float z;
} vec3f_t;

class MPU6500 {
 public:
  MPU6500(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_group, uint16_t cs_pin);
  // sample latest sensor data
  void UpdateData();
  // reset sensor registers
  void Reset();

  // 3-axis accelarometer
  vec3f_t acce;
  // 3-axis gyroscope
  vec3f_t gyro;
  // sensor temperature
  float   temp;

 private:
  void WriteReg(const uint8_t reg, uint8_t data);
  void WriteRegs(const uint8_t reg_start, uint8_t *data, uint8_t len);
  void ReadReg(const uint8_t reg, uint8_t *data);
  void ReadRegs(const uint8_t reg_start, uint8_t *data, uint8_t len);

  SPI_HandleTypeDef *hspi_;
  GPIO chip_select_;
};

} /* namespace BSP */
