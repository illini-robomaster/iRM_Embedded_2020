#pragma once

#include "main.h"

namespace control {

/**
 * @brief simple PID controller
 */
class PIDController {
 public:
  /**
   * @brief PID controller constructor
   *
   * @param kp proportional gain
   * @param ki integral gain
   * @param kd derivative gain
   * @param dt sampling time for the control loop in [s]
   * @param N  derivative lowpass filter bandwidth in [rad / s]
   */
  PIDController(float kp, float ki, float kd, float dt, float N);

  /**
   * @brief compute output base on current error
   *
   * @param error error of the system, i.e. (target - actual)
   *
   * @return output value that could potentially drive the error to 0
   */
  float ComputeOutput(float error);

 private:
  // states
  float u1_ = 0;
  float u2_ = 0;
  float e1_ = 0;
  float e2_ = 0;

  // coefficients
  const float a0_;
  const float uc1_;
  const float uc2_;
  const float ec0_;
  const float ec1_;
  const float ec2_;
};

} /* namespace control */
