#include "vec3.h"
#include "real.h"
#include <math.h>

vec3 vec3_add(const vec3* v1, const vec3* v2) {
    vec3 result;
    result.x = v1->x + v2->x;
    result.y = v1->y + v2->y;
    result.z = v1->z + v2->z;
    return result;
    // return { real_add(v1->x, v2->x), real_add(v1->y, v2->y), real_add(v1->z, v2->z) };
}

vec3 vec3_subtract(const vec3 *v1, const vec3 *v2) {
    vec3 result;
    result.x = v1->x - v2->x;
    result.y = v1->y - v2->y;
    result.z = v1->z - v2->z;
    return result;
    //return vec3_init_values(real_sub(v1->x, v2->x), real_sub(v1->y, v2->y), real_sub(v1->z, v2->z));
}

vec3 vec3_multiply_scalar(const vec3 *v, real t) {
    vec3 result;
    result.x = real_mul(v->x, t);
    result.y = real_mul(v->y, t);
    result.z = real_mul(v->z, t);
    return result;
    //return vec3_init_values(real_mul(v->x, t), real_mul(v->y, t), real_mul(v->z, t));
}

vec3 vec3_multiply_vec3(const vec3 *v1, const vec3 *v2) {
    vec3 result;
    result.x = real_mul(v1->x, v2->x);
    result.y = real_mul(v1->y, v2->y);
    result.z = real_mul(v1->z, v2->z);
    return result;
    //return vec3_init_values(real_mul(v1->x, v2->x), real_mul(v1->y, v2->y), real_mul(v1->z, v2->z));
}

real vec3_length2(const vec3 *v) {
    real result;
    result = real_mul(v->x, v->x) + real_mul(v->y, v->y) + real_mul(v->z, v->z);
    return result;
    //return real_add(real_add(real_mul(v->x, v->x), real_mul(v->y, v->y)), real_mul(v->z, v->z));
}

real vec3_length(const vec3 *v) {
    return real_sqrt(vec3_length2(v));
}

vec3 vec3_normalized(const vec3 *v) {
    return vec3_multiply_scalar(v, real_sqrt_inv(vec3_length2(v)));
}

real vec3_dot(const vec3 *v1, const vec3 *v2) {
    real result;
    result = real_mul(v1->x, v2->x) + real_mul(v1->y, v2->y) + real_mul(v1->z, v2->z);
    return result;
    //return real_add(real_add(real_mul(v1->x, v2->x), real_mul(v1->y, v2->y)), real_mul(v1->z, v2->z));
}
