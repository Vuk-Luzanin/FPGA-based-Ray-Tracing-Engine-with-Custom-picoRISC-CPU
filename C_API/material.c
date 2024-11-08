#include "material.h"

static vec3 material_color_rgb(uint32_t rgb) {
    vec3 result;
    result.x = (uint32_t)((rgb & 0xff0000)) >> 8;
    result.y = (uint32_t)((rgb & 0x00ff00));
    result.z = (uint32_t)((rgb & 0x0000ff)) << 8;
    return result;
    /*
    real r = (uint32_t) ((rgb & 0xff0000)) >> 8;
    real g = (uint32_t) ((rgb & 0x00ff00));
    real b = (uint32_t) ((rgb & 0x0000ff)) << 8;
    return vec3_init_values(r, g, b);
    */
}

int material_light_mode;

vec3 material_color_sky[2];
vec3 material_color_horizon[2];
vec3 material_color_floor1[2];
vec3 material_color_floor2[2];
vec3 material_color_floor_average[2];

vec3 material_color_sphere[4];

vec3 material_internal_color_highlight = {0x0000ffff, 0, 0};

extern const vec3* const material_color_highlight = &material_internal_color_highlight;

void material_init_colors() {
    material_light_mode = 0;

    material_color_sky[0] = material_color_rgb(0x87CEFB);  // Light blue for normal mode
    material_color_sky[1] = material_color_rgb(0x1f009F);  // Neon magenta (purple) for retro mode

    material_color_horizon[0] = material_color_rgb(0xFFFF00);  // Orange-red for normal sunset horizon
    material_color_horizon[1] = material_color_rgb(0xFF00FF);  // Bright yellow for retro mode

    material_color_floor1[0] = material_color_rgb(0xB0E0E6);  // Light foggy blue for normal mode (for a sky-like ground effect)
    material_color_floor1[1] = material_color_rgb(0x00FFFF);  // Bright cyan for retro mode

    material_color_floor2[0] = material_color_rgb(0x2F4F6F);  // Dark gray-blue for normal mode (to mimic sky reflection)
    material_color_floor2[1] = material_color_rgb(0x8A2BE2);  // Neon purple for retro mode

    vec3 mid;
    mid = vec3_add(&material_color_floor1[0], &material_color_floor2[0]);
    mid.x = mid.x >> 1;
    mid.y = mid.y >> 1;
    mid.z = mid.z >> 1;
    material_color_floor_average[0] = mid;
    mid = vec3_add(&material_color_floor1[1], &material_color_floor2[1]);
    mid.x = mid.x >> 1;
    mid.y = mid.y >> 1;
    mid.z = mid.z >> 1;
    material_color_floor_average[1] = mid;

    material_color_sphere[0] = material_color_rgb(0xee003f);
    material_color_sphere[1] = material_color_rgb(0x00ee25);
    material_color_sphere[2] = material_color_rgb(0x0052ee);
    material_color_sphere[3] = material_color_rgb(0x4600D2);

}
