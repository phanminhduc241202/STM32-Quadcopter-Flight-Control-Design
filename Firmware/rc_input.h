#ifndef RC_INPUT_H
#define RC_INPUT_H

#include <stdint.h>

// Giá trị RC mặc định (khoảng giữa, ví dụ 500)
#define RC_MID_POINT 500

extern uint16_t rc_throttle;
extern uint16_t rc_roll;
extern uint16_t rc_pitch;
extern uint16_t rc_yaw;

void RC_ProcessInput(uint8_t channel_index, uint16_t pulse_width);
void RC_UpdateRCTime(void);
uint32_t RC_GetLastTime(void);

#endif // RC_INPUT_H
