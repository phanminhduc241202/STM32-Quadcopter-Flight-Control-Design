#ifndef DRONE_TYPES_H
#define DRONE_TYPES_H

#include <stdint.h>

// Định nghĩa cấu trúc dữ liệu cho MPU6050 và ước tính thái độ
typedef struct {
    // Giá trị cảm biến thô
    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;

    // Giá trị vật lý (g và deg/s)
    float Ax;
    float Ay;
    float Az;
    float Gx;
    float Gy;
    float Gz;

    // Ước tính thái độ (Attitude)
    float roll;
    float pitch;
    float yaw;

} MPU6050_t;

// Định nghĩa cấu trúc dữ liệu cho bộ điều khiển PID
typedef struct {
    float Kp;               // Hệ số Tỷ lệ
    float Ki;               // Hệ số Tích phân
    float Kd;               // Hệ số Đạo hàm

    float error;            // Lỗi hiện tại
    float previous_error;   // Lỗi trước đó
    float integral;         // Tích phân lỗi
    float derivative;       // Đạo hàm lỗi

    float output;           // Đầu ra PID
    float output_limit;     // Giới hạn đầu ra tối đa

} PID_t;

#endif // DRONE_TYPES_H
