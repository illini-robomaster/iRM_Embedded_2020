#include "controller.h"

namespace control {

PIDController::PIDController(float kp, float ki, float kd, float N, float dt)
  : a0_(1 + N * dt),
    uc1_((2 + N * dt) / a0_),
    uc2_(-1 / a0_),
    ec0_(kp + (ki * dt * a0_ + kd * N) / a0_),
    ec1_(-(kp * (2 + N * dt) + ki * dt + 2 * kd * N) / a0_),
    ec2_((kp + kd * N) / a0_) {}

float PIDController::ComputeOutput(float error) {
  // compute output
  float u0 = uc1_ * u1_ + uc2_ * u2_ + ec0_ * error + ec1_ * e1_ + ec2_ * e2_;

  // update state
  e2_ = e1_;
  e1_ = error;
  u2_ = u1_;
  u1_ = u0;

  return u0;
}

} /* namespace control */
