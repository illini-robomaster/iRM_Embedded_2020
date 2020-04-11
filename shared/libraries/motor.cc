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

#include "bsp_error_handler.h"

#include "arm_math.h"
#include "motor.h"
#include "utils.h"

using namespace bsp;

namespace control {

static void can_motor_callback(const uint8_t data[], void *args) {
  MotorCANBase *motor = reinterpret_cast<MotorCANBase*>(args);
  motor->DataCallback(data);
}

MotorCANBase::MotorCANBase(bsp::CAN *can, uint16_t rx_id) 
      : theta_(0), omega_(0), can_(can), rx_id_(rx_id) {
  constexpr uint16_t GROUP_SIZE = 4;
  constexpr uint16_t RX1_ID_START = 0x201;
  constexpr uint16_t RX2_ID_START = 0x205;
  constexpr uint16_t RX3_ID_START = 0x209;
  constexpr uint16_t TX1_ID = 0x200;
  constexpr uint16_t TX2_ID = 0x1ff;
  constexpr uint16_t TX3_ID = 0x2ff;

  RM_ASSERT_GE(rx_id, RX1_ID_START, "Invalid rx id");
  RM_ASSERT_LT(rx_id, RX3_ID_START + GROUP_SIZE, "Invalid rx id");
  if (rx_id >= RX3_ID_START)
    tx_id_ = TX3_ID;
  else if (rx_id >= RX2_ID_START)
    tx_id_ = TX2_ID;
  else
    tx_id_ = TX1_ID;
}

void MotorCANBase::PopulateMotorMsg(motor_msg_t *msg) {
  msg->theta = GetTheta();
  msg->omega = GetOmega();
  msg->timestamp = timestamp_;
}

void MotorCANBase::DataCallback(const uint8_t data[]) {
  timestamp_ = HAL_GetTick();
  UpdateData(data);
}

void MotorCANBase::TransmitOutput(MotorCANBase *motors[], uint8_t num_motors) {
  uint8_t data[8] = { 0 };

  RM_ASSERT_GT(num_motors, 0, "Meaningless empty can motor transmission");
  RM_ASSERT_LT(num_motors, 4, "Exceeding maximum of 4 motor commands per CAN message");
  for (uint8_t i = 0; i < num_motors; ++i) {
    RM_ASSERT_EQ(motors[i]->tx_id_, motors[0]->tx_id_, "tx id mismatch");
    RM_ASSERT_EQ(motors[i]->can_, motors[0]->can_, "can line mismatch");
    const uint8_t motor_idx = (motors[i]->rx_id_ - 1) % 4;
    const int16_t output = motors[i]->output_;
    data[2*motor_idx] = output >> 8;
    data[2*motor_idx+1] = output & 0xff;
  }

  motors[0]->can_->Transmit(motors[0]->tx_id_, data, 8);
}

float MotorCANBase::GetTheta() const {
  return theta_;
}

float MotorCANBase::GetThetaDelta(float target) const {
  return wrap<float>(target - theta_, -PI, PI);
}

float MotorCANBase::GetOmega() const {
  return omega_;
}

float MotorCANBase::GetOmegaDelta(float target) const {
  return target - omega_;
}

Motor3508::Motor3508(CAN *can, uint16_t rx_id) : MotorCANBase(can, rx_id) {
  can->RegisterRxCallback(rx_id, can_motor_callback, this);
}

void Motor3508::UpdateData(const uint8_t data[]) {
  const int16_t raw_theta = data[0] << 8 | data[1];
  const int16_t raw_omega = data[2] << 8 | data[3];
  raw_current_get_ = data[4] << 8 | data[5];
  raw_temperature_ = data[6];

  constexpr float THETA_SCALE = 2 * PI / 8192; // digital -> rad
  constexpr float OMEGA_SCALE = 2 * PI / 60; // rpm -> rad / sec
  theta_ = raw_theta * THETA_SCALE;
  omega_ = raw_omega * OMEGA_SCALE;
}

void Motor3508::PrintData() const {
  print("theta: %.4f ", GetTheta());
  print("omega: %.4f ", GetOmega());
  print("raw temperature: %d ", raw_temperature_);
  print("raw current get: %d \r\n", raw_current_get_);
}

void Motor3508::SetOutput(int16_t val) {
  constexpr int16_t MAX_ABS_CURRENT = 12288; // ~20A
  output_ = clip<int16_t>(val, -MAX_ABS_CURRENT, MAX_ABS_CURRENT);
}

Motor6623::Motor6623(CAN *can, uint16_t rx_id) : MotorCANBase(can, rx_id) {
  can->RegisterRxCallback(rx_id, can_motor_callback, this);
}

void Motor6623::UpdateData(const uint8_t data[]) {
  const int16_t raw_theta = data[0] << 8 | data[1];
  raw_current_get_ = (data[2] << 8 | data[3]) * CURRENT_CORRECTION;
  raw_current_set_ = (data[4] << 8 | data[5]) * CURRENT_CORRECTION;

  constexpr float THETA_SCALE = 2 * PI / 8192;
  theta_ = raw_theta * THETA_SCALE;
}

void Motor6623::PrintData() const {
  print("theta: %.4f ", GetTheta());
  print("raw current get: %d ", raw_current_get_);
  print("raw current set: %d \r\n", raw_current_set_);
}

void Motor6623::SetOutput(int16_t val) {
  constexpr int16_t MAX_ABS_CURRENT = 5000; // ~5.3A
  output_ = clip<int16_t>(val * CURRENT_CORRECTION, -MAX_ABS_CURRENT, MAX_ABS_CURRENT);
}

float Motor6623::GetOmega() const {
  RM_EXPECT_TRUE(false, "6623 does not support omega messurement");
  return 0;
}

float Motor6623::GetOmegaDelta(const float target) const {
  UNUSED(target);
  RM_EXPECT_TRUE(false, "6623 does not support omega messurement");
  return 0;
}

Motor2006::Motor2006(CAN *can, uint16_t rx_id) : MotorCANBase(can, rx_id) {
  can->RegisterRxCallback(rx_id, can_motor_callback, this);
}

void Motor2006::UpdateData(const uint8_t data[]) {
  const int16_t raw_theta = data[0] << 8 | data[1];
  const int16_t raw_omega = data[2] << 8 | data[3];
  raw_current_get_ = data[4] << 8 | data[5];

  constexpr float THETA_SCALE = 2 * PI / 8192; // digital -> rad
  constexpr float OMEGA_SCALE = 2 * PI / 60; // rpm -> rad / sec
  theta_ = raw_theta * THETA_SCALE;
  omega_ = raw_omega * OMEGA_SCALE;
}

void Motor2006::PrintData() const {
  print("theta: %.4f ", GetTheta());
  print("omega: %.4f ", GetOmega());
  print("raw current get: %d \r\n", raw_current_get_);
}

void Motor2006::SetOutput(int16_t val) {
  constexpr int16_t MAX_ABS_CURRENT = 10000; // ~10A
  output_ = clip<int16_t>(val, -MAX_ABS_CURRENT, MAX_ABS_CURRENT);
}

MotorPWMBase::MotorPWMBase(TIM_HandleTypeDef *htim, uint8_t channel, 
    uint32_t clock_freq, uint32_t output_freq, uint32_t idle_throttle)
      : pwm_(htim, channel, clock_freq, output_freq, idle_throttle), 
        idle_throttle_(idle_throttle) {
  pwm_.Start();
}

void MotorPWMBase::SetOutput(int16_t val) {
  output_ = val;
  pwm_.SetPulseWidth(val + idle_throttle_);
}

void Motor2305::SetOutput(int16_t val) {
  constexpr int16_t MIN_OUTPUT = 0;
  constexpr int16_t MAX_OUTPUT = 700;
  MotorPWMBase::SetOutput(clip<int16_t>(val, MIN_OUTPUT, MAX_OUTPUT));
}

} /* namespace control */
