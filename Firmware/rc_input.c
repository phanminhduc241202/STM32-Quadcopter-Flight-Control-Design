#include "rc_input.h"

// Giả lập HAL API
#define HAL_GetTick() 0 // Cần được thay thế bằng HAL_GetTick() thực tế

// Các biến lưu trữ giá trị RC
uint16_t rc_throttle = 0;
uint16_t rc_roll = 0;
uint16_t rc_pitch = 0;
uint16_t rc_yaw = 0;

// Biến theo dõi thời gian nhận tín hiệu RC cuối cùng
static uint32_t last_rc_time = 0;

// Xử lý tín hiệu RC (thường được gọi từ Input Capture Callback hoặc PPM Decoder)
void RC_ProcessInput(uint8_t channel_index, uint16_t pulse_width) {
    // Giả sử thứ tự kênh là 0: Throttle, 1: Roll, 2: Pitch, 3: Yaw
    switch (channel_index) {
        case 0:
            rc_throttle = pulse_width;
            break;
        case 1:
            rc_roll = pulse_width;
            break;
        case 2:
            rc_pitch = pulse_width;
            break;
        case 3:
            rc_yaw = pulse_width;
            break;
        default:
            break;
    }
    RC_UpdateRCTime();
}

// Cập nhật thời gian nhận tín hiệu RC cuối cùng (Dùng cho Failsafe)
void RC_UpdateRCTime(void) {
    last_rc_time = HAL_GetTick();
}

// Lấy thời gian RC cuối cùng
uint32_t RC_GetLastTime(void) {
    return last_rc_time;
}
