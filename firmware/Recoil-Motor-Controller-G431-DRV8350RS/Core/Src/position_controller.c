/*
 * position_controller.c
 *
 *  Created on: Aug 27, 2022
 *      Author: TK
 */

#include "position_controller.h"

void PositionController_init(PositionController *controller) {
  controller->position_kp = 0.1;
  controller->position_ki = 0;//0.0000001;
  controller->position_kd = 0.;

  controller->torque_limit = 0.01;

  controller->acceleration_limit = 1;//1;

  controller->velocity_limit = 3;

  controller->position_limit_lower = -100;
  controller->position_limit_upper = 100;

  controller->velocity_setpoint = 0;
  controller->position_setpoint = 0;

  controller->position_integrator = 0;
  controller->velocity_integrator = 0;
}

void PositionController_update(PositionController *controller, Mode mode) {
  //  acceleration = trajectory_follower(command_position, command_velocity)
  //  control_velocity = command_velocity OR control_velocity + acceleration * dt OR 0.0
  //  control_position = command_position OR control_position + control_velocity * dt
  //  position_error = control_position - feedback_position
  //  velocity_error = control_velocity - feedback_velocity
  //  position_integrator = limit(position_integrator + ki * position_error * dt, ilimit)
  //  torque = position_integrator +
  //           kp * kp_scale * position_error +
  //           kd * kd_scale * velocity_error +
  //           command_torque

  float dt = 1 / 8000.;
//
////  controller->position_target = clampf(
////      controller->position_target,
////      controller->position_limit_lower,
////      controller->position_limit_upper);
//
//  controller->velocity_target = clampf(
//      controller->velocity_target,
//      -controller->velocity_limit,
//      controller->velocity_limit);
//
//  float position_target_error = controller->position_target - controller->position_measured;
////  float velocity_target_error = controller->velocity_target - controller->velocity_measured;
//
//  float acceleration = controller->acceleration_limit;
//
//  if (fabs(controller->velocity_measured) > controller->velocity_limit) {
//    acceleration = copysignf(acceleration, -controller->velocity_measured);
//  }
//  else {
//    if ((controller->velocity_measured * position_target_error) > 0) {
//      float decel_distance = (controller->velocity_measured * controller->velocity_measured) / (2 * acceleration);
//      controller->position_limit_upper = decel_distance;
//      if (fabs(position_target_error) >= decel_distance) {
//        if (fabs(controller->velocity_measured) < controller->velocity_limit) {
//          acceleration = copysignf(acceleration, position_target_error);
//        }
//        else {
//          acceleration = 0;
//        }
//      }
//      else {
//        acceleration = copysignf(acceleration, -controller->velocity_measured);
//      }
//    }
//    else {
//      acceleration = copysignf(acceleration, -controller->velocity_measured);
//    }
//  }
//  controller->position_limit_lower = acceleration;
//
//  controller->velocity_setpoint = controller->velocity_setpoint + acceleration * dt;
//
//  controller->velocity_setpoint = clampf(
//      controller->velocity_setpoint,
//      -controller->velocity_limit,
//      controller->velocity_limit);
//
////  if (fabs(position_target_error) < 0.01 * M_PI) {
////    controller->position_setpoint = controller->position_target;
////  }
////  else {
//    controller->position_setpoint = controller->position_setpoint + controller->velocity_setpoint * dt;
////  }
//
//  float position_error = controller->position_setpoint - controller->position_measured;
//  float velocity_error = controller->velocity_setpoint - controller->velocity_measured;
//
//  controller->position_integrator += controller->position_ki * position_error;
//
//  controller->position_integrator = clampf(controller->position_integrator, -2*M_PI, 2*M_PI);
//
//  controller->torque_target =
//      controller->position_kp * position_error
//    + controller->position_kd * velocity_error
//    + controller->position_integrator;

  float accel = 30;

  float k_p = 0.002;
  float k_d = 0.001;

  float dx = controller->position_target - controller->position_measured;
  controller->dx = dx;
  float decel_distance = (controller->velocity_measured * controller->velocity_measured) / (2 * accel);
  controller->decel_distance = decel_distance;
  if (controller->velocity_measured * dx > 0) {
    if (fabs(dx) > 1.2 * decel_distance) { // accelerate
      accel = copysignf(accel, dx);
    }
    else { // decelerate
      accel = copysignf(accel, -controller->velocity_measured);
    }
  }
  else {
    accel = copysignf(accel, -controller->velocity_measured);
  }


  controller->velocity_setpoint += accel * dt;
  controller->position_setpoint += controller->velocity_setpoint * dt;
//  controller->position_setpoint = clampf(controller->position_setpoint, -2*M_PI, 2*M_PI);

  if (mode != MODE_TORQUE) {
    controller->torque_setpoint = k_p * (controller->position_setpoint - controller->position_measured) + k_d * (controller->velocity_setpoint - controller->velocity_measured);

    controller->accel = accel;

    controller->torque_setpoint = clampf(
        controller->torque_setpoint,
        -controller->torque_limit,
        controller->torque_limit);


//    controller->torque_setpoint = (controller->position_controller.torque_setpoint * (float)controller->motor.kv_rating) / 8.3;
//    controller->current_controller.i_d_target = 0;

//    controller->torque_setpoint =
//        controller->torque_target - controller->torque_measured;
  }
  else {
    controller->torque_setpoint = controller->torque_target;
  }
}
