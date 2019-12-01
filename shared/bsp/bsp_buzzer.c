#include "bsp_buzzer.h"
#include "arm_math.h"

static pwm_t buzzer_pwm;

void buzzer_init(TIM_HandleTypeDef *htim, uint32_t channel, 
      uint32_t clock_freq) {
  pwm_init(&buzzer_pwm, htim, channel, clock_freq, 0, 0); 
}

void buzzer_sing_tone(const buzzer_freq_t freq) {
  pwm_set_freq(&buzzer_pwm, freq);
  pwm_set_pulse_width(&buzzer_pwm, (buzzer_pwm.htim->Instance->ARR + 1) / 2);
}

void buzzer_sing_song(const buzzer_freq_t *freq, const uint32_t *delay) {
  while (*freq != Finish) {
    buzzer_sing_tone(*freq++);
    HAL_Delay(*delay++);
  }
}
