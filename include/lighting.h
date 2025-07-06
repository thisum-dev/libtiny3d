#ifndef LIGHTING_H
#define LIGHTING_H

#include "math3d.h"

typedef struct {
    vec3_t direction;  // Light direction (should be normalized)
    float intensity;   // Range: 0.0 to 1.0
} light_t;

#define MAX_LIGHTS 4

// Basic Lambertian lighting for one light
float compute_lambert_intensity(vec3_t edge_dir, light_t light);

// Combine multiple lights
float compute_lighting_multiple(vec3_t edge_dir, light_t* lights, int light_count);

#endif
