#include "main.h"
#include "tim.h"
#include "usart.h"
#include "stm32f4xx.h"

#include "bsp_buzzer.h"
#include "bsp_print.h"

static buzzer_freq_t startup[] = {
  Mi3M, Silent, Mi3M, Silent, Mi3M, Silent, 
  Do1M, Silent, Mi3M, Silent, 
  So5M, Silent, So5L, Silent, 
  Finish,
};

static uint32_t startup_delay[] = {
  80, 80, 80, 240, 80, 240,
  80, 80, 80, 240, 
  80, 560, 80, 560,
};

void RM_RTOS_Init(void) {
  buzzer_init(&htim12, 1, 1000000);
  bsp_print_init(&huart7);

  buzzer_sing_song(startup, startup_delay);

  printf("All Peripheral Initialized\r\n");
}
