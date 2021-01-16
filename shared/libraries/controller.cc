#include "controller.h"

namespace control {

PIDController::PIDController(float kp, float ki, float kd) {
  pid_f32_.Kp = kp;
  pid_f32_.Ki = ki;
  pid_f32_.Kd = kd;
  arm_pid_init_f32(&pid_f32_, 1);
}

float PIDController::ComputeOutput(float error) { return arm_pid_f32(&pid_f32_, error); }

} /* namespace control */
