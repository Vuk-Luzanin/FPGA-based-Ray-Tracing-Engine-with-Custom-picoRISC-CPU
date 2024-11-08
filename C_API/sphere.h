#pragma once

#include "vec3.h"
#include "material.h"

typedef struct {
    vec3 center;
    real radius;
    real radius2;
    material mat;
} sphere;

int sphere_intersect(const sphere *s, const ray *r, real *t);
