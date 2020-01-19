#pragma once

#include "can.h"

#define MAX_CAN_DATA_SIZE 8
#define MAX_CAN_DEVICES   12

namespace BSP {

typedef void (*can_rx_callback_t)(const uint8_t data[], void *args);

class CAN {
 public:
  CAN(CAN_HandleTypeDef *hcan, uint32_t start_id);

  bool Uses(CAN_HandleTypeDef *hcan) { return hcan_ == hcan; }
  int RegisterRxCallback(uint32_t std_id, can_rx_callback_t callback, void *args = NULL);
  int Transmit(uint16_t id, const uint8_t data[], uint32_t length);
  void RxCallback();

 private:
  void ConfigureFilter(CAN_HandleTypeDef *hcan);

  CAN_HandleTypeDef   *hcan_;
  uint32_t            start_id_;
  can_rx_callback_t   rx_callbacks_[MAX_CAN_DEVICES] = { 0 };
  void                *rx_args_[MAX_CAN_DEVICES] = { NULL };
};

} /* namespace BSP */
