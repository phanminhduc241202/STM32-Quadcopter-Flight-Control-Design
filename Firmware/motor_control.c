#include "motor_control.h"

// Giả lập HAL API
#define HAL_TIM_PWM_Start(htim, channel) 0
#define _HAL_TIM_SET_COMPARE(htim, channel, value) 0
#define TIM_CHANNEL_1 1
#define TIM_CHANNEL_2 2
#define TIM_CHANNEL_3 3
#define TIM_CHANNEL_4 4

// --- Dữ liệu Mock ---
void *htim1_mock = NULL; 
uint16_t motor_outputs[4] = {0, 0, 0, 0}; // Để theo dõi trạng thái động cơ

// Bắt đầu tín hiệu PWM cho cả 4 kênh (Motor)
void Motor_StartPWM(void) {
    // Giả sử TIM1 được cấu hình với 4 kênh
    HAL_TIM_PWM_Start(htim1_mock, TIM_CHANNEL_1); 
    HAL_TIM_PWM_Start(htim1_mock, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(htim1_mock, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(htim1_mock, TIM_CHANNEL_4);
}

// Đặt giá trị chu kỳ nhiệm vụ (Duty Cycle) cho động cơ
void Motor_SetSpeed(uint8_t motor_id, uint16_t pwm_value) {
    // Giới hạn giá trị PWM (giả sử từ 0 đến 1000 là khoảng hoạt động)
    if (pwm_value > 1000) pwm_value = 1000;
    else if (pwm_value < 0) pwm_value = 0;
    
    // Cập nhật giá trị Compare Register của Timer
    switch (motor_id) {
        case 1:
            _HAL_TIM_SET_COMPARE(htim1_mock, TIM_CHANNEL_1, pwm_value);
            motor_outputs[0] = pwm_value;
            break;
        case 2:
            _HAL_TIM_SET_COMPARE(htim1_mock, TIM_CHANNEL_2, pwm_value);
            motor_outputs[1] = pwm_value;
            break;
        case 3:
            _HAL_TIM_SET_COMPARE(htim1_mock, TIM_CHANNEL_3, pwm_value);
            motor_outputs[2] = pwm_value;
            break;
        case 4:
            _HAL_TIM_SET_COMPARE(htim1_mock, TIM_CHANNEL_4, pwm_value);
            motor_outputs[3] = pwm_value;
            break;
        default:
            // Xử lý lỗi
            break;
    }
}
