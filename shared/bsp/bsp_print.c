#include "bsp_print.h"

static UART_HandleTypeDef *bsp_print_port = NULL;

void bsp_print_init(UART_HandleTypeDef *huart) {
  bsp_print_port = huart;
}

/* implement low level io for third party printf library */
void _putchar(char ch) {
  if (bsp_print_port) {
    HAL_UART_Transmit(bsp_print_port, (uint8_t*)&ch, 1, 0xffff);
  }
}

