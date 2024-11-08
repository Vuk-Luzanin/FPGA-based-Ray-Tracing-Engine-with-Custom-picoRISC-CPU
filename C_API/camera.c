#include "camera.h"
#include "real.h"
#include "vec3.h"
#include <math.h>

static real real_cos_taylor(real x) {
    real result = 0x00010000;
    x = real_mul(x, x);
    real term = -x >> 1;
    result = real_add(result, term);

    real x2 = real_mul(x, x);
    term = (int64_t)x2 * 178956970 >> 32;
    result = real_add(result, term);

    real x3 = real_mul(x, x2);
    term = (int64_t)x3 * (-5965232) >> 32;
    result = real_add(result, term);

    return result;
}

static real real_cos(real x) {
    if (x < 0) {
        x = -x;
    }
    const int flip_sign = x >= REAL_PI_HALF && x < REAL_PI_THREE_HALFS;
    if (x > REAL_PI_HALF) {
        if (x <= REAL_PI) {
            x = REAL_PI - x;
        }
        else if (x <= REAL_PI_THREE_HALFS) {
            x = x - REAL_PI;
        }
        else {
            x = REAL_PI_TWO - x;
        }

    }

    real result = real_cos_taylor(x);
    if (flip_sign) {
        result = -result;
    }
    return result;
}

static real real_sin(real x) {
    return real_cos(REAL_PI_HALF - x);
}

vec3 camera_internal_position;
real camera_internal_pitch;
real camera_internal_yaw;
real camera_internal_cos_pitch;
real camera_internal_sin_pitch;
real camera_internal_cos_yaw;
real camera_internal_sin_yaw;
vec3 camera_internal_right;
vec3 camera_internal_up;
vec3 camera_internal_forward;

vec3 * const camera_position   = &camera_internal_position;
real * const camera_pitch = &camera_internal_pitch;
real * const camera_yaw = &camera_internal_yaw;
const real * const camera_cos_pitch = &camera_internal_cos_pitch;
const real * const camera_sin_pitch = &camera_internal_sin_pitch;
const real * const camera_cos_yaw = &camera_internal_cos_yaw;
const real * const camera_sin_yaw = &camera_internal_sin_yaw;
const vec3 * const camera_right = &camera_internal_right;
const vec3 * const camera_up = &camera_internal_up;
const vec3 * const camera_forward = &camera_internal_forward;

void camera_init() {
    camera_position->x = 0;
    camera_position->y = 0x00030000;
    camera_position->z = 0;
    *camera_pitch = 0;
    *camera_yaw = 0;
    camera_update();
}

static void camera_update_angles() {
    camera_internal_cos_pitch = real_cos(*camera_pitch);
    camera_internal_sin_pitch = real_sin(*camera_pitch);
    camera_internal_cos_yaw = real_cos(*camera_yaw);
    camera_internal_sin_yaw = real_sin(*camera_yaw);
}

static void camera_update_right() {
    camera_internal_right.x = *camera_sin_yaw;
    camera_internal_right.y = 0;
    camera_internal_right.z = ~*camera_cos_yaw;
}

static void camera_update_up() {
    camera_internal_up.x = ~real_mul(*camera_sin_pitch, *camera_cos_yaw);
    camera_internal_up.y = *camera_cos_pitch;
    camera_internal_up.z = ~real_mul(*camera_sin_pitch, *camera_sin_yaw);
}

static void camera_update_forward() {
    camera_internal_forward.x = real_mul(*camera_cos_pitch, *camera_cos_yaw);
    camera_internal_forward.y = *camera_sin_pitch;
    camera_internal_forward.z = real_mul(*camera_cos_pitch, *camera_sin_yaw);
}

void camera_update() {
    camera_update_angles();
    camera_update_right();
    camera_update_up();
    camera_update_forward();
}

vec3 camera_get_ray_direction(real u, real v) {
    vec3 right_scaled = vec3_multiply_scalar(camera_right, u);
    vec3 up_scaled = vec3_multiply_scalar(camera_up, v);

    vec3 temp = vec3_add(camera_forward, &right_scaled);
    vec3 direction = vec3_add(&temp, &up_scaled);

    return vec3_normalized(&direction);
}
