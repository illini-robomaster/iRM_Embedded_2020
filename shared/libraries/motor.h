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

#include "bsp_can.h"
#include "bsp_pwm.h"

namespace control {

class MotorBase {
 public:
  MotorBase() : output_(0) {}
  virtual ~MotorBase() {}

  virtual void SetOutput(int16_t val) { output_ = val; }

 protected:
  int16_t output_;
};

class MotorCANBase : public MotorBase {
 public:
  MotorCANBase(BSP::CAN *can, uint16_t rx_id);

  virtual void  UpdateData(const uint8_t data[]) = 0;
  virtual void  PrintData() const = 0;
  virtual float GetTheta() const;
  virtual float GetThetaDelta(const float target) const;
  virtual float GetOmega() const;
  virtual float GetOmegaDelta(const float target) const;

  static void TransmitOutput(MotorCANBase *motors[], uint8_t num_motors);

 protected:
  float theta_;
  float omega_;

 private:
  BSP::CAN  *can_;
  uint16_t  rx_id_;
  uint16_t  tx_id_;
};

class Motor3508 : public MotorCANBase {
 public:
  Motor3508(BSP::CAN *can, uint16_t rx_id) : MotorCANBase(can, rx_id) {}
  void UpdateData(const uint8_t data[]) override final;
  void PrintData() const override final;
  void SetOutput(int16_t val) override final; 

 private:
  int16_t raw_current_get_ = 0;
  uint8_t raw_temperature_ = 0;
};

class Motor6623 : public MotorCANBase {
 public:
  void UpdateData(const uint8_t data[]) override final;
  void SetOutput(int16_t val) override final;
  float GetOmega() const override final;
  float GetOmegaDelta(const float target) const override final;

 private:
  int16_t raw_current_get_ = 0;
  int16_t raw_current_set_ = 0;

  static const int16_t CURRENT_CORRECTION = -1; // current direction is reversed
};

class Motor2006 : public MotorCANBase {
 public:
  void UpdateData(const uint8_t data[]) override final;
  void SetOutput(int16_t val) override final;

 private:
  int16_t raw_current_get_ = 0;
};

class MotorPWMBase : public MotorBase {
 public:
  MotorPWMBase(TIM_HandleTypeDef *htim, uint8_t channe, 
      uint32_t clock_freq, uint32_t output_freq, uint32_t idle_throttle);

  virtual void SetOutput(int16_t val) override;

 private:
  BSP::PWM pwm_;
  uint32_t idle_throttle_;
};

class Motor2305 : public MotorPWMBase {
 public:
  virtual void SetOutput(int16_t val) override final; 
};

} /* namespace control */
