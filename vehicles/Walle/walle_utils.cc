#include "utils.h"
#include "walle_utils.h"
#include "bsp_print.h"

MG90S::MG90S(TIM_HandleTypeDef *htim, uint8_t channel, 
             uint32_t lower_pw, uint32_t upper_pw)
    : output_(.5),
      mapper_(0., 1., lower_pw, upper_pw),
      pwm_(htim, channel, 1000000, 50, mapper_.Map(output_)) {
  RM_ASSERT_GE(lower_pw, MIN_PULSE_WIDTH, "lower pulse width out of bound");
  RM_ASSERT_LE(lower_pw, MAX_PULSE_WIDTH, "lower pulse width out of bound");
  RM_ASSERT_GE(upper_pw, MIN_PULSE_WIDTH, "upper pulse width out of bound");
  RM_ASSERT_LE(upper_pw, MAX_PULSE_WIDTH, "upper pulse width out of bound");
  pwm_.Start();
}

void MG90S::SetOutput(float output) {
  output_ = clip<float>(output, 0, 1);
  pwm_.SetPulseWidth(mapper_.Map(output_));
}

void MG90S::SetIncrement(float inc) {
  SetOutput(output_ + inc);
}

