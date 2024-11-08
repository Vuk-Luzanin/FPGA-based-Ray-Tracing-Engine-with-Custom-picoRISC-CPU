#pragma once

#include "sphere.h"
#include "camera.h"
#include "material.h"

#define MAX_DEPTH 5                     //MAX_DEPTH - koliko puta pratimo odbijanje zraka
#define MAX_SPHERES 50
#define SCENE_SPAWN_DISTANCE 0x00010000
#define SCENE_GHOST_RADIUS_INIT 0x00008000
#define SCENE_GHOST_RADIUS_INIT2 0x00004000
#define SCENE_GHOST_RADIUS_MIN 0x00002000
#define SCENE_GHOST_RADIUS_MIN2 0x00000400

extern vec3 renderer_canvas[CAMERA_HEIGHT][CAMERA_WIDTH];

extern sphere scene_spheres[MAX_SPHERES];
extern real * const scene_ghost_radius;
extern real * const scene_ghost_radius2;
extern int * const scene_ghost_mode;
extern int * const scene_num_of_spheres;
extern sphere ** const scene_marked_sphere_address;

vec3 scene_get_sky_color(real direction_y);
vec3 scene_trace_ray(const ray *r, int depth);
void scene_reset();
void scene_add_sphere(const vec3 *position, real radius);
void scene_remove_sphere(sphere *sp);

void renderer_render();

