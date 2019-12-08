#include "bsp_usb.h"

static usb_callback_t rm_usb_rx_callback = NULL;

static void default_callback(uint8_t *buf, uint32_t len) {
  UNUSED(buf);
  UNUSED(len);
}

void usb_init(void) {
  rm_usb_rx_callback = default_callback;
}

void usb_register_callback(usb_callback_t callback) {
  rm_usb_rx_callback = callback;
}

void usb_unregister_callback(void) {
  rm_usb_rx_callback = NULL;
}

int usb_transmit(uint8_t *buf, uint32_t len) {
  uint8_t status = CDC_Transmit_FS(buf, (uint16_t)len);
  if (status == USBD_OK) {
    return len;
  }
  else if (status == USBD_BUSY) {
    return -1;
  }
  else { // status == USBD_FAIL (shouldn't get here)
    return -2;
  }
}

void RM_USB_RxCplt_Callback(uint8_t *Buf, uint32_t Len) {
  if (rm_usb_rx_callback) {
    rm_usb_rx_callback(Buf, Len);
  }
}
