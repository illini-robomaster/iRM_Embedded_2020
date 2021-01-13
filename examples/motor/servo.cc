#include "cmsis_os.h"
#include "main.h"

#include "bsp_print.h"
#include "motor.h"

#define KEY_GPIO_GROUP  GPIOB
#define KEY_GPIO_PIN    GPIO_PIN_2

control::MotorPWMBase *motor1 = NULL;

void RM_RTOS_Init() {
    print_use_uart(&huart8);
    motor1 = new control::MotorPWMBase(&htim4, 1, 1000000, 50, 0);
}

void RM_RTOS_Default_Task(const void *args) {
    UNUSED(args);
    while(1){
        motor1->SetOutput(1500);
        osDelay(1000);
        motor1->SetOutput(1000);
        osDelay(1000);
    }
}
