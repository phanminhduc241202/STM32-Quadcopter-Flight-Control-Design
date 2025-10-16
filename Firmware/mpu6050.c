#include "mpu6050.h"
#include <math.h>
#include <stdio.h>

// Giả lập HAL API (Cần được thay thế bằng các hàm HAL thực tế)
#define HAL_I2C_Mem_Write(hi2c, Addr, Reg, Size, Data, Len, Timeout) 0
#define HAL_I2C_Mem_Read(hi2c, Addr, Reg, Size, Data, Len, Timeout) 0
#define HAL_I2C_IsDeviceReady(hi2c, Addr, Trials, Timeout) 0 // 0=OK, 1=Error

// Định nghĩa giá trị M_PI
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// --- Dữ liệu Mock ---
void *hi2c1_mock = NULL; 

// Khởi tạo MPU6050 (dựa trên tài liệu PDF)
int MPU6050_Init(void) {
    uint8_t data;

    // 1. Kiểm tra thiết bị có sẵn không (Giả lập)
    if (HAL_I2C_IsDeviceReady(hi2c1_mock, MPU6050_ADDR, 1, 1000) != 0) {
        // Lỗi giao tiếp
        return 1; 
    }

    // 2. Thức dậy MPU6050 (PWR_MGMT_1_REG)
    data = 0x00;
    HAL_I2C_Mem_Write(hi2c1_mock, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &data, 1, 1000);

    // 3. Set DATA RATE 1KHz (SMPLRT_DIV_REG = 7)
    data = 0x07;
    HAL_I2C_Mem_Write(hi2c1_mock, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &data, 1, 1000);

    // 4. Configure Accel (+/-4g) (ACCEL_CONFIG_REG = 0x08)
    data = 0x08;
    HAL_I2C_Mem_Write(hi2c1_mock, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 1000);

    // 5. Configure Gyro (500 deg/s) (GYRO_CONFIG_REG = 0x08)
    data = 0x08;
    HAL_I2C_Mem_Write(hi2c1_mock, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 1000);

    return 0; // Thành công
}

// Đọc tất cả các thanh ghi cảm biến
void MPU6050_Read_All(MPU6050_t *DataStruct) {
    uint8_t data[14];
    
    // Đọc 14 byte từ ACCEL_XOUT_H_REG
    HAL_I2C_Mem_Read(hi2c1_mock, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, data, 14, 1000);

    // Kết hợp các byte để tạo giá trị 16-bit
    DataStruct->Accel_X_RAW = (int16_t) (data[0] << 8 | data[1]);
    DataStruct->Accel_Y_RAW = (int16_t) (data[2] << 8 | data[3]);
    DataStruct->Accel_Z_RAW = (int16_t) (data[4] << 8 | data[5]);
    
    // Bỏ qua thanh ghi nhiệt độ (data[6], data[7])
    
    DataStruct->Gyro_X_RAW = (int16_t) (data[8] << 8 | data[9]);
    DataStruct->Gyro_Y_RAW = (int16_t) (data[10] << 8 | data[11]);
    DataStruct->Gyro_Z_RAW = (int16_t) (data[12] << 8 | data[13]);

    // Chuyển đổi giá trị thô sang đơn vị vật lý
    // Độ nhạy Accel (+/-4g): 8192 LSB/g
    DataStruct->Ax = DataStruct->Accel_X_RAW / 8192.0f;
    DataStruct->Ay = DataStruct->Accel_Y_RAW / 8192.0f;
    DataStruct->Az = DataStruct->Accel_Z_RAW / 8192.0f;

    // Độ nhạy Gyro (500 deg/s): 65.5 LSB/(deg/s)
    DataStruct->Gx = DataStruct->Gyro_X_RAW / 65.5f;
    DataStruct->Gy = DataStruct->Gyro_Y_RAW / 65.5f;
    DataStruct->Gz = DataStruct->Gyro_Z_RAW / 65.5f;
}

// Cập nhật ước tính thái độ (Sử dụng Bộ lọc Bổ sung - Complementary Filter)
void UpdateAttitude(MPU6050_t *DataStruct, float dt) {
    // Hệ số bộ lọc bổ sung (alpha). Chọn 0.98 cho Gyro (lọc ngắn hạn) và 0.02 cho Accel (lọc dài hạn)
    const float ALPHA = 0.98f; 
    const float ALPHA_COMPLEMENT = 1.0f - ALPHA;

    // 1. Tính toán Roll và Pitch từ Accelerometer (Roll_acc, Pitch_acc)
    // Roll: Xoay quanh trục X. Phụ thuộc vào Ay và Az.
    float roll_acc = atan2f(DataStruct->Ay, DataStruct->Az) * 180.0f / M_PI;
    // Pitch: Xoay quanh trục Y. Phụ thuộc vào Ax và (Ay^2 + Az^2)
    float pitch_acc = atan2f(-DataStruct->Ax, sqrtf(DataStruct->Ay * DataStruct->Ay + DataStruct->Az * DataStruct->Az)) * 180.0f / M_PI;

    // 2. Tích hợp Gyro (Tính góc hiện tại dựa trên tốc độ góc và thời gian)
    // Dùng Gyro để tính sự thay đổi tức thời
    DataStruct->roll += DataStruct->Gx * dt;
    DataStruct->pitch += DataStruct->Gy * dt;
    DataStruct->yaw += DataStruct->Gz * dt; // Yaw rate -> Yaw angle

    // 3. Kết hợp bằng Bộ lọc Bổ sung
    // Gyro cung cấp phản hồi nhanh, Accel cung cấp sự điều chỉnh dài hạn
    DataStruct->roll = ALPHA * (DataStruct->roll) + ALPHA_COMPLEMENT * roll_acc;
    DataStruct->pitch = ALPHA * (DataStruct->pitch) + ALPHA_COMPLEMENT * pitch_acc;

    // Yaw thường chỉ dựa vào Gyro vì Accel không thể xác định hướng Yaw (trừ khi có từ kế)
    // Nếu drone cần duy trì Yaw, ta sẽ cần thêm từ kế (Magnetometer).
}
