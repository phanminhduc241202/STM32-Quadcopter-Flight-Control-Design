#ifndef FLIGHT_CONTROLLER_H
#define FLIGHT_CONTROLLER_H

#include "drone_types.h"
#include "pid.h"
#include "mpu6050.h"

// Thời gian chu kỳ điều khiển (10ms = 100Hz)
#define DT_S 0.01f

void FlightController_Init(void);
void FlightController_Loop(void);

#endif // FLIGHT_CONTROLLER_H
