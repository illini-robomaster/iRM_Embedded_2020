#pragma once

#include "main.h"
#include "arm_math.h"

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
    */
  PIDController(float kp, float ki, float kd);

  /**
   * @brief compute output base on current error
   *
   * @param error error of the system, i.e. (target - actual)
   *
   * @return output value that could potentially drive the error to 0
   */
  float ComputeOutput(float error);

 private:
  arm_pid_instance_f32 pid_f32_;
};

} /* namespace control */
