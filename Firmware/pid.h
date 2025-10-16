#ifndef PID_H
#define PID_H

#include "drone_types.h"

void PID_Init(PID_t *PID, float Kp, float Ki, float Kd, float output_limit);
float PID_Calculate(PID_t *PID, float setpoint, float measured_value, float dt);

#endif // PID_H
