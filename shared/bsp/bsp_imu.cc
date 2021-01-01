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

#include "bsp_imu.h"

#include "bsp_error_handler.h"
#include "bsp_mpu6500_reg.h"

#define MPU6500_SIZEOF_DATA 14  // acc (6 bytes) + temp (2 bytes) + gyro (6 bytes)
#define MPU6500_DELAY 55        // some arbitrary random stuff
// configured with initialization sequences
#define MPU6500_ACC_FACTOR 4096.0f
#define MPU6500_GYRO_FACTOR 32.768f

namespace bsp {

MPU6500::MPU6500(SPI_HandleTypeDef *hspi, const GPIO &chip_select)
    : hspi_(hspi), chip_select_(chip_select) {
  uint8_t init_data[7][2] = {
      {MPU6500_PWR_MGMT_1, 0x03},      // auto select clock source
      {MPU6500_PWR_MGMT_2, 0x00},      // enable acc & gyro
      {MPU6500_CONFIG, 0x02},          // gyro LP bandwidth 92Hz
      {MPU6500_GYRO_CONFIG, 0x10},     // gyro range 1000 dps / 32.8
      {MPU6500_ACCEL_CONFIG, 0x10},    // acc range 8g / 4096
      {MPU6500_ACCEL_CONFIG_2, 0x02},  // acc LP bandwidth 92Hz
      {MPU6500_USER_CTRL, 0x20},       // enable I2C master
  };
  Reset();  // reset all registers and signal paths
  for (size_t i = 0; i < 7; ++i) WriteReg(init_data[i][0], init_data[i][1]);
  // validate register values
  uint8_t tmp;
  for (size_t i = 0; i < 7; ++i) {
    ReadReg(init_data[i][0], &tmp);
    if (tmp != init_data[i][1])
      bsp_error_handler(__FUNCTION__, __LINE__, "imu register incorrect initialization");
  }
}

void MPU6500::UpdateData() {
  uint8_t buff[MPU6500_SIZEOF_DATA];
  // interpret as int16_t array
  int16_t *array = reinterpret_cast<int16_t *>(buff);

  ReadRegs(MPU6500_ACCEL_XOUT_H, buff, MPU6500_SIZEOF_DATA);

  // in-place swap endian
  for (size_t i = 0; i < MPU6500_SIZEOF_DATA; i += 2)
    array[i / 2] = (int16_t)(buff[i] << 8 | buff[i + 1]);

  acce.x = (float)array[0] / MPU6500_ACC_FACTOR;
  acce.y = (float)array[1] / MPU6500_ACC_FACTOR;
  acce.z = (float)array[2] / MPU6500_ACC_FACTOR;
  temp = (float)array[3] / MPU6500_TEMP_FACTOR + MPU6500_TEMP_OFFSET;
  gyro.x = (float)array[4] / MPU6500_GYRO_FACTOR;
  gyro.y = (float)array[5] / MPU6500_GYRO_FACTOR;
  gyro.z = (float)array[6] / MPU6500_GYRO_FACTOR;
}

void MPU6500::Reset() {
  WriteReg(MPU6500_PWR_MGMT_1, 0x80);
  WriteReg(MPU6500_SIGNAL_PATH_RESET, 0x07);
  HAL_Delay(1);  // seems like signal path reset needs some time
}

void MPU6500::WriteReg(uint8_t reg, uint8_t data) { WriteRegs(reg, &data, 1); }

void MPU6500::WriteRegs(uint8_t reg_start, uint8_t *data, uint8_t len) {
  uint8_t tx = reg_start & 0x7f;

  chip_select_.Low();
  HAL_SPI_Transmit(hspi_, &tx, 1, MPU6500_DELAY);
  HAL_SPI_Transmit(hspi_, data, len, MPU6500_DELAY);
  chip_select_.High();
}

void MPU6500::ReadReg(uint8_t reg, uint8_t *data) { ReadRegs(reg, data, 1); }

void MPU6500::ReadRegs(uint8_t reg_start, uint8_t *data, uint8_t len) {
  chip_select_.Low();
  *data = static_cast<uint8_t>(reg_start | 0x80);
  HAL_SPI_Transmit(hspi_, data, 1, MPU6500_DELAY);
  HAL_SPI_Receive(hspi_, data, len, MPU6500_DELAY);
  chip_select_.High();
}

} /* namespace bsp */
