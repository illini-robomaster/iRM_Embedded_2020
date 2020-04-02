#include "dbus.h"
#include "bsp_error_handler.h"

/* uart data frame length */
#define DBUS_DATA_LEN  18

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

typedef struct {
    /* rocker channel information */
    int16_t ch0 : 11;    // S1*             *S2
    int16_t ch1 : 11;    //   C3-^       ^-C1
    int16_t ch2 : 11;    // C2-<   >+ -<   >+C0
    int16_t ch3 : 11;    //     +v       v+
    /* left and right switch information */
    uint8_t swr : 2;
    uint8_t swl : 2;
    /* mouse movement and button information */
    struct {
        int16_t x;
        int16_t y;
        int16_t z;
        uint8_t l;
        uint8_t r;
    } __packed mouse;
    /* keyboard key information */
    uint16_t code;
} __packed dbus_t;

static remote::DBUS *dbus = NULL;

static void dbus_callback(bsp::UART *uart) {
  if (!dbus)
    return;
  
  uint8_t *data;
  /* data frame is not aligned */
  if (uart->ReadFromISR(&data) != DBUS_DATA_LEN)
    return;
   
  /* fill in rocker value */
  dbus_t *repr = reinterpret_cast<dbus_t*>(data);
  dbus->ch0 = repr->ch0 - RC_ROCKER_MID;
  dbus->ch1 = repr->ch1 - RC_ROCKER_MID;
  dbus->ch2 = repr->ch2 - RC_ROCKER_MID;
  dbus->ch3 = repr->ch3 - RC_ROCKER_MID;
}

namespace remote {

DBUS::DBUS(UART_HandleTypeDef *huart) : uart_(huart) {
  RM_ASSERT_FALSE(dbus, "duplicate initializations");
  dbus = this;
  uart_.SetupRx(DBUS_DATA_LEN * 3, dbus_callback);
}

} /* namespace remote */
