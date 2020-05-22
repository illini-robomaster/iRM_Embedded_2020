#pragma once

#include "bsp_error_handler.h"
#include "bsp_print.h"
#include "bsp_pwm.h"

template<class Ti, class To>
class LinearMapper {
 public:
  LinearMapper(Ti ai, Ti bi, To ao, To bo)
      : k_((float)(ao - bo) / (ai - bi)), 
      b_((float)(ai*bo - ao*bi) / (ai - bi)) {}

  To Map(Ti input) {
    return k_ * input + b_;
  }

 private:
  const float k_;
  const float b_;
};

class MG90S {
 public:
  static const int32_t MIN_PULSE_WIDTH=400;
  static const int32_t MAX_PULSE_WIDTH=2400;

  MG90S(TIM_HandleTypeDef *htim, uint8_t channel, 
        uint32_t lower_pw, uint32_t upper_pw);

  void SetOutput(float input);

  void SetIncrement(float inc);

 private:
  float output_;
  LinearMapper<float, int32_t> mapper_;
  bsp::PWM pwm_;
};
