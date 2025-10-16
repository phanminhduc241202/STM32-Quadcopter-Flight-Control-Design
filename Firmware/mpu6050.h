#ifndef MPU6050_H
#define MPU6050_H

#include "drone_types.h"

// Định nghĩa địa chỉ và thanh ghi MPU6050
#define MPU6050_ADDR        0xD0
#define PWR_MGMT_1_REG      0x6B
#define SMPLRT_DIV_REG      0x19
#define ACCEL_CONFIG_REG    0x1C
#define GYRO_CONFIG_REG     0x1B
#define ACCEL_XOUT_H_REG    0x3B
#define WHO_AM_I_REG        0x75

// Giả lập HAL_I2C_HandleTypeDef (cần được CubeMX định nghĩa)
extern void *hi2c1_mock;

int MPU6050_Init(void);
void MPU6050_Read_All(MPU6050_t *DataStruct);
void UpdateAttitude(MPU6050_t *DataStruct, float dt);

#endif // MPU6050_H
