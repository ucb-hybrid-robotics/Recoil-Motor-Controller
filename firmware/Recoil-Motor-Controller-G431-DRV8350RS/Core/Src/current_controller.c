
#include "current_controller.h"

void CurrentController_init(CurrentController *controller) {
  controller->current_filter_alpha = 0.2;

  controller->i_q_kp = 30.;
  controller->i_q_ki = 0.01; // 0.01

  controller->i_d_kp = 30.;
  controller->i_d_ki = 0.01;

  controller->i_q_measured = 0;
  controller->i_d_measured = 0;
}

void CurrentController_update(CurrentController *controller, Mode mode, float sin_theta, float cos_theta, float v_bus) {
  FOC_clarkTransform(
    &controller->i_alpha_measured,
    &controller->i_beta_measured,
    controller->i_a_measured,
    controller->i_b_measured,
    controller->i_c_measured);

  float i_q;
  float i_d;

  FOC_parkTransform(
    &i_q,
    &i_d,
    controller->i_alpha_measured,
    controller->i_beta_measured,
    sin_theta, cos_theta);

  controller->i_q_measured = controller->current_filter_alpha * (i_q - controller->i_q_measured);
  controller->i_d_measured = controller->current_filter_alpha * (i_d - controller->i_d_measured);

  if (mode != MODE_IQD_OVERRIDE) {
    controller->i_q_setpoint = controller->i_q_target - controller->i_q_measured;
    controller->i_d_setpoint = controller->i_d_target - controller->i_d_measured;
    controller->i_q_integrator = clampf(
        controller->i_q_integrator + controller->i_q_ki * controller->i_q_setpoint, -5, 5);
    controller->i_d_integrator = clampf(
        controller->i_d_integrator + controller->i_d_ki * controller->i_d_setpoint, -5, 5);
  }
  else {
    controller->i_q_integrator = 0;
    controller->i_d_integrator = 0;
  }

  if (mode != MODE_VQD_OVERRIDE) {
    controller->v_q_setpoint =
        controller->i_q_kp * controller->i_q_setpoint + controller->i_q_integrator;
    controller->v_d_setpoint =
        controller->i_d_kp * controller->i_d_setpoint + controller->i_d_integrator;
  }

  // clamp voltage
  if (v_bus > 0) {
    // CSVPWM over modulation
    float v_max_sq = v_bus * v_bus * 1.15;
    float v_norm = (
        (controller->v_q_setpoint * controller->v_q_setpoint)
        + (controller->v_d_setpoint * controller->v_d_setpoint)
        );
    if (v_norm > v_max_sq) {
      float k = sqrtf(fabsf(v_norm / v_max_sq));
      controller->v_q_setpoint *= k;
      controller->v_d_setpoint *= k;
    }
  }

  if (mode != MODE_VALPHABETA_OVERRIDE && mode != MODE_CALIBRATION) {
    // calibration mode needs to override v_alpha_setpoint and v_beta_setpoint
    FOC_invParkTransform(
      &controller->v_alpha_setpoint,
      &controller->v_beta_setpoint,
      controller->v_q_setpoint,
      controller->v_d_setpoint,
      sin_theta, cos_theta);
  }

  if (mode != MODE_VABC_OVERRIDE) {
    FOC_invClarkSVPWM(
      &controller->v_a_setpoint,
      &controller->v_b_setpoint,
      &controller->v_c_setpoint,
      controller->v_alpha_setpoint,
      controller->v_beta_setpoint);
  }
}

