#include "pid.h"

// Khởi tạo các tham số PID
void PID_Init(PID_t *PID, float Kp, float Ki, float Kd, float output_limit) {
    PID->Kp = Kp;
    PID->Ki = Ki;
    PID->Kd = Kd;

    PID->error = 0;
    PID->previous_error = 0;
    PID->integral = 0;
    PID->derivative = 0;
    PID->output = 0;
    PID->output_limit = output_limit;
}

// Tính toán đầu ra PID
float PID_Calculate(PID_t *PID, float setpoint, float measured_value, float dt) {
    // 1. Tính toán Lỗi
    PID->error = setpoint - measured_value;

    // 2. Tính toán Tích phân
    PID->integral += PID->error * dt;

    // Giới hạn tích phân để tránh windup
    if (PID->integral > PID->output_limit) PID->integral = PID->output_limit;
    else if (PID->integral < -PID->output_limit) PID->integral = -PID->output_limit;

    // 3. Tính toán Đạo hàm
    // Đảm bảo dt không phải là 0 để tránh chia cho 0
    if (dt > 0.0f) {
        PID->derivative = (PID->error - PID->previous_error) / dt;
    } else {
        PID->derivative = 0;
    }

    // 4. Tính toán đầu ra PID
    PID->output = (PID->Kp * PID->error) +
                  (PID->Ki * PID->integral) +
                  (PID->Kd * PID->derivative);

    // 5. Giới hạn đầu ra
    if (PID->output > PID->output_limit) PID->output = PID->output_limit;
    else if (PID->output < -PID->output_limit) PID->output = -PID->output_limit;

    // 6. Cập nhật lỗi trước đó
    PID->previous_error = PID->error;

    return PID->output;
}
