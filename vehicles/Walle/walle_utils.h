#pragma once

#include "bsp_error_handler.h"

#include "bsp_print.h"

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
