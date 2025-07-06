#include "math3d.h"
#include <math.h>
#include <float.h>
#include <stdint.h>

// Helper function to update spherical coordinates from Cartesian
static void update_spherical(vec3_t* v) {
    v->r = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
    if (v->r > 1e-6f) {
        v->theta = acosf(v->z / v->r);  // theta: angle from z-axis
        v->phi = atan2f(v->y, v->x);    // phi: angle in xy-plane
    } else {
        v->theta = 0.0f;
        v->phi = 0.0f;
    }
}

// Helper function to update Cartesian coordinates from spherical
static void update_cartesian(vec3_t* v) {
    v->x = v->r * sinf(v->theta) * cosf(v->phi);
    v->y = v->r * sinf(v->theta) * sinf(v->phi);
    v->z = v->r * cosf(v->theta);
}

// ---------------- Vector Functions ----------------

vec3_t vec3_from_spherical(float r, float theta, float phi) {
    vec3_t v;
    v.r = r;
    v.theta = theta;
    v.phi = phi;
    update_cartesian(&v);
    return v;
}

vec3_t vec3_from_cartesian(float x, float y, float z) {
    vec3_t v;
    v.x = x;
    v.y = y;
    v.z = z;
    update_spherical(&v);
    return v;
}

vec3_t vec3_normalize_fast(vec3_t v) {
    float len_sq = v.x * v.x + v.y * v.y + v.z * v.z;
    if (len_sq < 1e-8f) return v;  // Avoid division by zero
    
    // Fast inverse square root trick (Quake III algorithm)
    float x2 = len_sq * 0.5f;
    float y = len_sq;
    
    // Evil floating point bit level hacking
    union {
        float f;
        uint32_t i;
    } conv = {.f = y};
    
    conv.i = 0x5f3759df - (conv.i >> 1);  // Magic number!
    y = conv.f;
    
    // Newton-Raphson iteration for better precision
    y = y * (1.5f - (x2 * y * y));   // 1st iteration
    y = y * (1.5f - (x2 * y * y));   // 2nd iteration (optional, for more precision)
    
    vec3_t norm;
    norm.x = v.x * y;
    norm.y = v.y * y;
    norm.z = v.z * y;
    update_spherical(&norm);
    return norm;
}

vec3_t vec3_slerp(vec3_t a, vec3_t b, float t) {
    // Normalize input vectors
    a = vec3_normalize_fast(a);
    b = vec3_normalize_fast(b);
    
    float dot = a.x * b.x + a.y * b.y + a.z * b.z;
    dot = fminf(fmaxf(dot, -1.0f), 1.0f);  // Clamp to avoid numerical errors

    float theta = acosf(fabsf(dot));
    float sin_theta = sinf(theta);
    
    // If vectors are nearly parallel, use linear interpolation
    if (sin_theta < 1e-5f) {
        vec3_t result;
        result.x = (1 - t) * a.x + t * b.x;
        result.y = (1 - t) * a.y + t * b.y;
        result.z = (1 - t) * a.z + t * b.z;
        update_spherical(&result);
        return result;
    }

    float w1 = sinf((1 - t) * theta) / sin_theta;
    float w2 = sinf(t * theta) / sin_theta;

    vec3_t result;
    result.x = w1 * a.x + w2 * b.x;
    result.y = w1 * a.y + w2 * b.y;
    result.z = w1 * a.z + w2 * b.z;
    update_spherical(&result);
    return result;
}

// Additional vector operations you'll need
vec3_t vec3_add(vec3_t a, vec3_t b) {
    return vec3_from_cartesian(a.x + b.x, a.y + b.y, a.z + b.z);
}

vec3_t vec3_subtract(vec3_t a, vec3_t b) {
    return vec3_from_cartesian(a.x - b.x, a.y - b.y, a.z - b.z);
}

vec3_t vec3_scale(vec3_t v, float s) {
    return vec3_from_cartesian(v.x * s, v.y * s, v.z * s);
}

float vec3_dot(vec3_t a, vec3_t b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3_t vec3_cross(vec3_t a, vec3_t b) {
    return vec3_from_cartesian(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

float vec3_length(vec3_t v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

// ---------------- Matrix Functions ----------------

mat4_t mat4_identity() {
    mat4_t m = {0};
    for (int i = 0; i < 4; i++)
        m.m[i][i] = 1.0f;
    return m;
}

mat4_t mat4_translate(float tx, float ty, float tz) {
    mat4_t m = mat4_identity();
    m.m[3][0] = tx;
    m.m[3][1] = ty;
    m.m[3][2] = tz;
    return m;
}

mat4_t mat4_scale(float sx, float sy, float sz) {
    mat4_t m = {0};
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;
    m.m[3][3] = 1.0f;
    return m;
}

mat4_t mat4_rotate_xyz(float rx, float ry, float rz) {
    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);

    // Rotation around X
    mat4_t rotX = mat4_identity();
    rotX.m[1][1] = cx; rotX.m[1][2] = -sx;
    rotX.m[2][1] = sx; rotX.m[2][2] = cx;

    // Rotation around Y
    mat4_t rotY = mat4_identity();
    rotY.m[0][0] = cy; rotY.m[0][2] = sy;
    rotY.m[2][0] = -sy; rotY.m[2][2] = cy;

    // Rotation around Z
    mat4_t rotZ = mat4_identity();
    rotZ.m[0][0] = cz; rotZ.m[0][1] = -sz;
    rotZ.m[1][0] = sz; rotZ.m[1][1] = cz;

    // Combine all rotations (order: Z * Y * X)
    mat4_t temp = mat4_multiply(rotZ, rotY);
    mat4_t final = mat4_multiply(temp, rotX);
    return final;
}

mat4_t mat4_frustum_asymmetric(float l, float r, float b, float t, float n, float f) {
    mat4_t m = {0};
    m.m[0][0] = (2 * n) / (r - l);
    m.m[1][1] = (2 * n) / (t - b);
    m.m[2][0] = (r + l) / (r - l);
    m.m[2][1] = (t + b) / (t - b);
    m.m[2][2] = -(f + n) / (f - n);
    m.m[2][3] = -1.0f;
    m.m[3][2] = -(2 * f * n) / (f - n);
    return m;
}

// Multiply two 4x4 matrices: C = A * B
mat4_t mat4_multiply(mat4_t A, mat4_t B) {
    mat4_t result = {0};
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            for (int k = 0; k < 4; k++) {
                result.m[col][row] += A.m[k][row] * B.m[col][k];
            }
        }
    }
    return result;
}

// Transform a vector by a matrix (useful for applying transformations)
vec3_t mat4_transform_vec3(mat4_t m, vec3_t v) {
    float x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0];
    float y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1];
    float z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2];
    float w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3];
    
    // Perspective divide
    if (fabsf(w) > 1e-6f) {
        x /= w;
        y /= w;
        z /= w;
    }
    
    return vec3_from_cartesian(x, y, z);
}

// Create a perspective projection matrix
mat4_t mat4_perspective(float fov, float aspect, float near, float far) {
    mat4_t m = {0};
    float f = 1.0f / tanf(fov/2);
    
    m.m[0][0] = f / aspect;
    m.m[1][1] = f;
    m.m[2][2] = (far+near)/(near-far);
    m.m[2][3] = -1;
    m.m[3][2] = (2*far*near)/(near-far);
    
    return m;
}

// Create a look-at matrix for camera positioning
mat4_t mat4_look_at(vec3_t eye, vec3_t center, vec3_t up) {
    vec3_t f = vec3_normalize_fast(vec3_subtract(center, eye));
    vec3_t s = vec3_normalize_fast(vec3_cross(f, up));
    vec3_t u = vec3_cross(s, f);
    
    mat4_t m = mat4_identity();
    m.m[0][0] = s.x;
    m.m[0][1] = u.x;
    m.m[0][2] = -f.x;
    
    m.m[1][0] = s.y;
    m.m[1][1] = u.y;
    m.m[1][2] = -f.y;
    
    m.m[2][0] = s.z;
    m.m[2][1] = u.z;
    m.m[2][2] = -f.z;
    
    m.m[3][0] = -vec3_dot(s, eye);
    m.m[3][1] = -vec3_dot(u, eye);
    m.m[3][2] = vec3_dot(f, eye);
    
    return m;
}