#ifndef MATH3D_H
#define MATH3D_H

typedef struct {
    float x, y, z;         // Cartesian
    float r, theta, phi;   // Spherical
} vec3_t;

typedef struct {
    float m[4][4];         // 4×4 Matrix (column-major)
} mat4_t;

// Vector functions
vec3_t vec3_from_spherical(float r, float theta, float phi);
vec3_t vec3_from_cartesian(float x, float y, float z);  // Add this missing declaration
vec3_t vec3_normalize_fast(vec3_t v);
vec3_t vec3_slerp(vec3_t a, vec3_t b, float t);

// Additional vector operations
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_subtract(vec3_t a, vec3_t b);
vec3_t vec3_scale(vec3_t v, float s);
float vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_cross(vec3_t a, vec3_t b);
float vec3_length(vec3_t v);

// Matrix functions
mat4_t mat4_identity();
mat4_t mat4_translate(float tx, float ty, float tz);
mat4_t mat4_scale(float sx, float sy, float sz);
mat4_t mat4_rotate_xyz(float rx, float ry, float rz);
mat4_t mat4_frustum_asymmetric(float l, float r, float b, float t, float n, float f);
mat4_t mat4_multiply(mat4_t A, mat4_t B);

// Additional matrix operations
vec3_t mat4_transform_vec3(mat4_t m, vec3_t v);
mat4_t mat4_perspective(float fov, float aspect, float near, float far);
mat4_t mat4_look_at(vec3_t eye, vec3_t center, vec3_t up);

#endif