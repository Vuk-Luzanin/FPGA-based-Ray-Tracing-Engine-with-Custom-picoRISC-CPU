#pragma once

#include "vec3.h"

#define CAMERA_WIDTH 800
#define CAMERA_HEIGHT 600

extern vec3 * const camera_position;
extern real * const camera_pitch;
extern real * const camera_yaw;
extern const real * const camera_cos_pitch;
extern const real * const camera_sin_pitch;
extern const real * const camera_cos_yaw;
extern const real * const camera_sin_yaw;
extern const vec3 * const camera_right;
extern const vec3 * const camera_up;
extern const vec3 * const camera_forward;

void camera_init();
void camera_update();
vec3 camera_get_ray_direction(real u, real v);
