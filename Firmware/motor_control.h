#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>

// Giả lập HAL_TIM_HandleTypeDef (cần được CubeMX định nghĩa)
extern void *htim1_mock; 

void Motor_StartPWM(void);
void Motor_SetSpeed(uint8_t motor_id, uint16_t pwm_value);

#endif // MOTOR_CONTROL_H
