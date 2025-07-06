#include "lighting.h"
#include <math.h>
#define LIGHT_BOOST 2.0f
float compute_lambert_intensity(vec3_t edge_dir, light_t light) {
    vec3_t edge_norm = vec3_normalize_fast(edge_dir);
    vec3_t light_norm = vec3_normalize_fast(light.direction);
    float dot = vec3_dot(edge_norm, light_norm);
    float intensity = fmaxf(0.0f, dot) * light.intensity;
    intensity *= LIGHT_BOOST;
    if (intensity > 1.0f) intensity = 1.0f;  // clamp
    return intensity;
    //return fmaxf(0.0f, dot) * light.intensity;
}

float compute_lighting_multiple(vec3_t edge_dir, light_t* lights, int light_count) {
    float total = 0.0f;
    for (int i = 0; i < light_count && i < MAX_LIGHTS; i++) {
        total += compute_lambert_intensity(edge_dir, lights[i]);
    }
    if (total > 1.0f) total = 1.0f;
    return total;
}
