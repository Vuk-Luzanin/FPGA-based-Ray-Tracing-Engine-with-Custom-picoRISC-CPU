#pragma once

#include "vec3.h"

typedef struct {
    vec3 color;
    real reflectivity;  // 1.0 for a perfect mirror, 0.0 for a diffuse surface
} material;

void material_init_colors();

extern int material_light_mode;

extern vec3 material_color_sky[2];
extern vec3 material_color_horizon[2];
extern vec3 material_color_floor1[2];
extern vec3 material_color_floor2[2];
extern vec3 material_color_floor_average[2];
extern vec3 material_color_sphere[4];
extern const vec3 * const material_color_highlight;
