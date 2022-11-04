/*
 * foc.h
 *
 *  Created on: Aug 2, 2022
 *      Author: TK
 */

#ifndef INC_POSITION_CONTROLLER_H_
#define INC_POSITION_CONTROLLER_H_

#include <stdint.h>

#include "foc_math.h"

typedef struct {
  float position_kp;
  float position_ki;
  float position_kd;
  float position_kdd;

  float torque_limit_upper;
  float torque_limit_lower;
  float velocity_limit_upper;
  float velocity_limit_lower;
  float position_limit_upper;
  float position_limit_lower;

  float acceleration_limit_upper;
  float acceleration_limit_lower;

  float torque_target;
  float torque_measured;
  float torque_setpoint;

  float velocity_target;
  float velocity_measured;
  float velocity_setpoint;

  float position_target;
  float position_measured;
  float position_setpoint;

  float acceleration_measured;
} PositionController;


void PositionController_init(PositionController *controller);

void PositionController_update(PositionController *controller);

#endif /* INC_POSITION_CONTROLLER_H_ */
