#include <stdio.h>
#include <string.h>
#include "flight_controller.h"
#include "motor_control.h"
#include "rc_input.h"

// Giả lập HAL API
#define HAL_Init() 0
#define HAL_Delay(ms) 0
#define HAL_GetTick() 0
#define HAL_UART_Transmit(huart, data, len, timeout) 0

// Cần được CubeMX định nghĩa
void MX_GPIO_Init(void) {}
void MX_I2C1_Init(void) {} // Dùng cho MPU6050
void MX_TIM1_Init(void) {} // Dùng cho Motor PWM
void MX_USART2_UART_Init(void) {} // Dùng cho Debug Telemetry
void SystemClock_Config(void) {}

// Giả lập HAL_UART_HandleTypeDef (cần được CubeMX định nghĩa)
void *huart2_mock = NULL;
extern uint16_t motor1_output, motor2_output, motor3_output, motor4_output;
extern MPU6050_t mpu;

// Gửi Telemetry qua UART (dựa trên tài liệu PDF)
void SendTelemetry(void) {
    char buffer[100];
    int len;

    len = sprintf(buffer, "R:%.2f P:%.2f Y:%.2f T:%d M1:%d M2:%d M3:%d M4:%d\r\n",
                  mpu.roll, mpu.pitch, mpu.yaw, rc_throttle,
                  motor1_output, motor2_output, motor3_output, motor4_output);

    // Giả lập gửi qua UART2
    HAL_UART_Transmit(huart2_mock, (uint8_t*)buffer, len, 100);
}

// Hàm chính của chương trình
int main(void) {
    // 1. Khởi tạo HAL Library
    HAL_Init();

    // 2. Cấu hình System Clock (Cần được CubeMX tạo)
    SystemClock_Config();

    // 3. Khởi tạo ngoại vi (Cần được CubeMX tạo)
    MX_GPIO_Init();
    MX_I2C1_Init(); 
    MX_TIM1_Init(); 
    MX_USART2_UART_Init();

    // 4. Khởi tạo Bộ điều khiển Chuyến bay
    FlightController_Init();
    
    // 5. Khởi động PWM cho động cơ
    Motor_StartPWM();

    // 6. Arm the motors (Cần thời gian chờ để ESC khởi động)
    for (int i = 1; i <= 4; i++) {
        Motor_SetSpeed(i, 0); // Đảm bảo PWM = 0
    }
    HAL_Delay(3000); // Đợi 3 giây cho ESCs (Giả lập)

    // Vòng lặp chính của ứng dụng
    uint32_t previous_time = HAL_GetTick();
    uint32_t previous_telemetry_time = HAL_GetTick();

    while (1) {
        uint32_t current_time = HAL_GetTick();

        // Chạy vòng lặp điều khiển chuyến bay ở tần số 100Hz (10ms)
        if (current_time - previous_time >= 10) {
            FlightController_Loop();
            previous_time = current_time;
        }

        // Gửi Telemetry (ví dụ: 10Hz)
        if (current_time - previous_telemetry_time >= 100) {
            SendTelemetry();
            previous_telemetry_time = current_time;
        }
        
        // --- Ở đây bạn có thể thêm các tác vụ nền khác ---
        // Ví dụ: Đọc điện áp pin, xử lý giao tiếp
    }
}
