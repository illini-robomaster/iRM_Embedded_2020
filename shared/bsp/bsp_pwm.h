#ifndef _BSP_PWM_H_
#define _BSP_PWM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"

/**
 * @ingroup bsp
 * @defgroup bsp_pwm BSP PWM
 * @{
 */

typedef struct {
  TIM_HandleTypeDef   *htim;
  uint32_t            channel;
  uint32_t            clock_freq;
  uint32_t            output_freq;
  uint32_t            pulse_width;
} pwm_t;

/**
 * @brief initialize a pwm channel and start sending pwm signals
 *
 * @param my_pwm      a pwm structure to be initialized
 * @param htim        hal timer handle type
 * @param channel     pwm channel (should be in the range [1 4])
 * @param clock_freq  clock frequency associated with the timer in [Hz]
 * @param output_freq pwm output frequency in [Hz]
 * @param pulse_width pwm output pulse width in [us]
 *
 * @return  initialized pwm structure
 */
pwm_t *pwm_init(pwm_t *my_pwm, TIM_HandleTypeDef *htim, uint8_t channel, 
    uint32_t clock_freq, uint32_t output_freq, uint32_t pulse_width);

/**
 * @brief start sending pwm signals
 *
 * @param my_pwm    pwm instance
 */
void pwm_start(pwm_t *my_pwm);

/**
 * @brief stop sending pwm signals
 *
 * @param my_pwm    pwm instance
 */
void pwm_stop(pwm_t *my_pwm);

/**
 * @brief set pwm output frequency (i.e. 1 / period) in [Hz]
 *
 * @param my_pwm      pwm instance
 * @param output_freq pwm output frequency in [Hz]
 */
void pwm_set_freq(pwm_t *my_pwm, uint32_t output_freq);

/**
 * @brief adjust pulse width for a pwm channel
 * @param my_pwm        pwm instance
 * @param pulse_width   pulse width in [us]
 */
void pwm_set_pulse_width(pwm_t *my_pwm, uint32_t pulse_width);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
