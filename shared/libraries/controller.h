#pragma once

#include "main.h"
#include "arm_math.h"

namespace control {

class PIDController {
 public:
  PIDController(float kp, float ki, float kd);
  float ComputeOutput(float error);
 private:
  arm_pid_instance_f32 pid_f32_;
};

} /* namespace control */
