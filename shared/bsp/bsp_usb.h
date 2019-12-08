#ifndef _BSP_USB_H_
#define _BSP_USB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* build library only when USB is present on the MCU */
#ifdef HAL_PCD_MODULE_ENABLED

#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

typedef void (*usb_callback_t)(uint8_t *buf, uint32_t len);

void usb_init(void);

void usb_register_callback(usb_callback_t callback);

void usb_unregister_callback(void);

int usb_transmit(uint8_t *buf, uint32_t len);

#endif // HAL_PCD_MODULE_ENABLED

#ifdef __cplusplus
}
#endif

#endif
