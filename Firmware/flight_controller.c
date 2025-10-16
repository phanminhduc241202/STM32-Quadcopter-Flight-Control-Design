#include "flight_controller.h"
#include "motor_control.h"
#include "rc_input.h"
#include <stdio.h>

// Định nghĩa PID controllers cho mỗi trục
PID_t pid_roll, pid_pitch, pid_yaw;

// Định nghĩa cấu trúc dữ liệu IMU
MPU6050_t mpu;

// Đầu ra động cơ cuối cùng (dùng cho Telemetry)
uint16_t motor1_output, motor2_output, motor3_output, motor4_output;

// Khởi tạo Bộ điều khiển Chuyến bay
void FlightController_Init(void) {
    // 1. Khởi tạo MPU6050
    MPU6050_Init(); 

    // 2. Khởi tạo PID (Các giá trị Kp, Ki, Kd chỉ là ví dụ - Cần được tinh chỉnh)
    // Giả sử giới hạn đầu ra tối đa là 400 (phụ thuộc vào dải PWM)
    const float OUTPUT_LIMIT = 400.0f; 

    // Roll/Pitch PID: Kp=2.0, Ki=0.01, Kd=0.5
    PID_Init(&pid_roll, 2.0f, 0.01f, 0.5f, OUTPUT_LIMIT); 
    PID_Init(&pid_pitch, 2.0f, 0.01f, 0.5f, OUTPUT_LIMIT); 
    
    // Yaw PID: Kp=3.0, Ki=0.005, Kd=0.0 (Yaw thường cần I nhỏ hơn và Kd ít hơn)
    PID_Init(&pid_yaw, 3.0f, 0.005f, 0.0f, OUTPUT_LIMIT); 
}

// Xử lý Failsafe (dựa trên tài liệu PDF)
void Failsafe_Check(void) {
    // Giả lập HAL_GetTick()
    uint32_t current_time = HAL_GetTick(); 
    
    // Nếu không nhận được tín hiệu RC trong 500ms
    if (current_time - RC_GetLastTime() > 500) {
        // Giảm ga từ từ để hạ cánh
        if (rc_throttle > 50) {
            rc_throttle -= 5;
        } else {
            rc_throttle = 0;
        }
        
        // Đặt Roll, Pitch, Yaw về trung tâm
        rc_roll = RC_MID_POINT;
        rc_pitch = RC_MID_POINT;
        rc_yaw = RC_MID_POINT;
    }
}

// Vòng lặp Điều khiển Chuyến bay
void FlightController_Loop(void) {
    // 1. Kiểm tra Failsafe
    Failsafe_Check();

    // 2. Đọc và cập nhật cảm biến
    MPU6050_Read_All(&mpu);
    UpdateAttitude(&mpu, DT_S);

    // 3. Tính toán góc mong muốn (Desired angles) từ đầu vào RC
    // Giả sử dải RC là 0-1000, trung tâm là 500.
    // Chuyển đổi 0-1000 sang dải góc mong muốn (ví dụ: -40 đến +40 độ cho Roll/Pitch)
    // float desired_angle = (rc_value - RC_MID_POINT) * Scale_Factor;
    float desired_roll = (rc_roll - RC_MID_POINT) * 0.08f;      // 0.08f -> +/-40 độ
    float desired_pitch = (rc_pitch - RC_MID_POINT) * 0.08f;    // 0.08f -> +/-40 độ
    
    // Yaw rate mong muốn (Gz)
    float desired_yaw_rate = (rc_yaw - RC_MID_POINT) * 0.4f; // 0.4f -> tốc độ quay mong muốn

    // 4. Tính toán đầu ra PID
    float roll_output = PID_Calculate(&pid_roll, desired_roll, mpu.roll, DT_S);
    float pitch_output = PID_Calculate(&pid_pitch, desired_pitch, mpu.pitch, DT_S);
    float yaw_output = PID_Calculate(&pid_yaw, desired_yaw_rate, mpu.Gz, DT_S);

    // 5. Tính toán đầu ra động cơ cuối cùng (Cấu hình X-Quadcopter)
    // Base Throttle (Ga cơ sở)
    uint16_t base_throttle = rc_throttle;

    // Áp dụng PID Corrections vào Base Throttle
    // Motor 1: Trước-Phải (Front-Right)
    motor1_output = (uint16_t)(base_throttle - roll_output + pitch_output + yaw_output); 
    
    // Motor 2: Sau-Trái (Back-Left)
    motor2_output = (uint16_t)(base_throttle + roll_output - pitch_output + yaw_output);

    // Motor 3: Trước-Trái (Front-Left)
    motor3_output = (uint16_t)(base_throttle + roll_output + pitch_output - yaw_output);
    
    // Motor 4: Sau-Phải (Back-Right)
    motor4_output = (uint16_t)(base_throttle - roll_output - pitch_output - yaw_output); 
    
    // Cần đảm bảo các giá trị nằm trong dải PWM (ví dụ: 0-1000)
    // Hàm SetMotorSpeed sẽ đảm bảo giới hạn này.

    // 6. Đặt tốc độ động cơ
    Motor_SetSpeed(1, motor1_output);
    Motor_SetSpeed(2, motor2_output);
    Motor_SetSpeed(3, motor3_output);
    Motor_SetSpeed(4, motor4_output);
}
