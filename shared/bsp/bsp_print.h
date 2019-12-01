#ifndef _BSP_PRINT_H_
#define _BSP_PRINT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"

#include "printf.h" // third party tiny-printf implemnetations

/**
 * @brief initialize print utility via UART
 *
 * @param huart uart handle
 */
void bsp_print_init(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif // _BSP_PRINT_H_
