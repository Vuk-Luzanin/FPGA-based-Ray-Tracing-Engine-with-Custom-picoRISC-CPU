#include "scene.h"
#include <math.h>
#include <stdlib.h>

vec3 renderer_canvas[CAMERA_HEIGHT][CAMERA_WIDTH];

static void renderer_sort_spheres(const sphere *spheres[], int num_of_spheres) {
    int pos = num_of_spheres;
    while (pos != 0) {
        int bound = pos;
        vec3 temp;
        pos = 0;
        for (int i = 0; i < bound - 1; i++) {
            vec3 temp;
            temp = vec3_subtract(&spheres[i]->center, camera_position);
            real dist1 = vec3_length2(&temp);
            temp = vec3_subtract(&spheres[i + 1]->center, camera_position);
            real dist2 = vec3_length2(&temp);
            if (dist1 < dist2) {
                const sphere *temp = spheres[i];
                spheres[i] = spheres[i + 1];
                spheres[i + 1] = temp;
                pos = i + 1;
            }
        }
    }
}

static void rendered_render_sphere_projected(const sphere *s) {
    vec3 c = s->center;

    if (c.z < 0) {
        return;
    }

    real A = vec3_length2(&c) - s->radius2;

    int i_min = 0;
    int j_min = 0;
    int i_max = CAMERA_HEIGHT - 1;
    int j_max = CAMERA_WIDTH - 1;

    for (int y = i_min; y <= i_max; y++) {
        for (int x = j_min; x <= j_max; x++) {
            const real rx = (x - 400) << 7;
            const real ry = (y - 300) << 7;
            const real temp1 = real_mul(c.x, rx) + real_mul(c.y, ry) + c.z;
            if (real_mul(temp1, temp1) >= real_mul(A, real_mul(rx, rx) + real_mul(ry, ry) + 0x00010000)) {
                renderer_canvas[y][x] = s->mat.color;
            }
        }
    }
}

static void renderer_render_sphere(const sphere *s) {
    vec3 camera_to_sphere = vec3_subtract(&s->center, camera_position);

    sphere projected;
    projected.center.x = vec3_dot(&camera_to_sphere, camera_right);
    projected.center.y = vec3_dot(&camera_to_sphere, camera_up);
    projected.center.z = vec3_dot(&camera_to_sphere, camera_forward);
    projected.radius = s->radius;
    projected.radius2 = s->radius2;
    projected.mat = s->mat;

    rendered_render_sphere_projected(&projected);
}

static void renderer_render_floor() {
    vec3 floor_color = material_color_floor1[material_light_mode];
    real c = *camera_cos_pitch;
    real s = *camera_sin_pitch;
    for (int y = 0; y < CAMERA_HEIGHT; y++) {
        for (int x = 0; x < CAMERA_WIDTH; x++) {
            if (c * (299 - y) > (s << 9)) {
                renderer_canvas[y][x] = floor_color;
            }
        }
    }
}

void renderer_render() {
    const sphere *spheres[MAX_SPHERES];
    int num_of_spheres = *scene_num_of_spheres;
    for (int i = 0; i < num_of_spheres; i++) {
        spheres[i] = &scene_spheres[i];
    }
    renderer_sort_spheres(spheres, num_of_spheres);
    for (int i = 0; i < CAMERA_HEIGHT; i++) {
        vec3 sky_color = scene_get_sky_color(camera_get_ray_direction(0, (i - 300) << 7).y);
        for (int j = 0; j < CAMERA_WIDTH; j++) {
            renderer_canvas[i][j] = sky_color;
        }
    }
    renderer_render_floor();
    for (int i = 0; i < num_of_spheres; i++) {
        if (spheres[i] == *scene_marked_sphere_address) {
            sphere s = *spheres[i];
            s.mat.color = *material_color_highlight;
            renderer_render_sphere(&s);
        }
        else {
            renderer_render_sphere(spheres[i]);
        }
    }
    if (*scene_ghost_mode) {
        sphere ghost_projected;
        ghost_projected.center.x = 0;
        ghost_projected.center.y = 0;
        ghost_projected.center.z = SCENE_SPAWN_DISTANCE + *scene_ghost_radius;
        ghost_projected.radius = *scene_ghost_radius;
        ghost_projected.radius2 = real_mul(*scene_ghost_radius, *scene_ghost_radius);
        ghost_projected.mat.color.x = 0x0000ffff;
        ghost_projected.mat.color.y = 0;
        ghost_projected.mat.color.z = 0;
        ghost_projected.mat.reflectivity = 0;
        rendered_render_sphere_projected(&ghost_projected);
    }
}
