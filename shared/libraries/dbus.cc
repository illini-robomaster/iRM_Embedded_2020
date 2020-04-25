#include "dbus.h"
#include "bsp_error_handler.h"

#include <cmath>
#include <cstring>

/* switch id */
#define RC_SWITCH_UP 1
#define RC_SWITCH_MI 3
#define RC_SWITCH_DN 2

/* rocker range and deadzones */
#define RC_ROCKER_MIN           364
#define RC_ROCKER_MID           1024
#define RC_ROCKER_MAX           1684
#define RC_ROCKER_ZERO_DRIFT    10 // Range of possible drift around initial position
// Range of possible drift around min or max position
#define RC_ROCKER_MIN_MAX_DRIFT (RC_ROCKER_MAX - RC_ROCKER_MID + 10)   

// helper struct for decoding raw bytes
typedef struct {
    /* rocker channel information */
    uint16_t ch0 : 11;    // S1*             *S2
    uint16_t ch1 : 11;    //   C3-^       ^-C1
    uint16_t ch2 : 11;    // C2-<   >+ -<   >+C0
    uint16_t ch3 : 11;    //     +v       v+
    /* left and right switch information */
    uint8_t swr : 2;
    uint8_t swl : 2;
    /* mouse movement and button information */
    remote::mouse_t mouse;
    /* keyboard key information */
    remote::keyboard_t keyboard;
    uint16_t reserved;
} __packed dbus_t;

namespace remote {

DBUS::DBUS(UART_HandleTypeDef *huart) : bsp::UART(huart) {
  SetupRx(sizeof(dbus_t) + 1);
}

void DBUS::RxCompleteCallback() {
  uint8_t *data;
  // data frame misalignment
  if (this->ReadFromISR(&data) != sizeof(dbus_t))
    return ;

  // re-interpret the data buffer and decode into class properties
  dbus_t *repr = reinterpret_cast<dbus_t*>(data);
  this->ch0 = repr->ch0 - RC_ROCKER_MID;
  this->ch1 = repr->ch1 - RC_ROCKER_MID;
  this->ch2 = repr->ch2 - RC_ROCKER_MID;
  this->ch3 = repr->ch3 - RC_ROCKER_MID;
  this->ch0 = abs(this->ch0) <= RC_ROCKER_ZERO_DRIFT ? 0 : this->ch0;
  this->ch1 = abs(this->ch1) <= RC_ROCKER_ZERO_DRIFT ? 0 : this->ch1;
  this->ch2 = abs(this->ch2) <= RC_ROCKER_ZERO_DRIFT ? 0 : this->ch2;
  this->ch3 = abs(this->ch3) <= RC_ROCKER_ZERO_DRIFT ? 0 : this->ch3;

  this->swl = repr->swl;
  this->swr = repr->swr;

  memcpy(&this->mouse, &repr->mouse, sizeof(mouse_t));
  memcpy(&this->keyboard, &repr->keyboard, sizeof(keyboard_t));
  
  this->timestamp = HAL_GetTick();
}

} /* namespace remote */
