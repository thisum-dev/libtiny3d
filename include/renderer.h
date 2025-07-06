#ifndef RENDERER_H
#define RENDERER_H

#include "canvas.h"
#include "math3d.h"

typedef struct {
    vec3_t position;
    float intensity; // For lighting (0.0 to 1.0)
} vertex_t;

typedef struct {
    int v0, v1; // Vertex indices
    float depth; // Average depth for sorting
} edge_t;

typedef struct {
    vertex_t* vertices;
    int vertex_count;
    edge_t* edges;
    int edge_count;
} mesh_t;


// Rendering functions
mesh_t create_cube_mesh(float size);
mesh_t load_obj_mesh(const char* filename);
void project_vertex(vertex_t* vertex, mat4_t transform);
int clip_to_circular_viewport(canvas_t* canvas, float x, float y);
void render_wireframe(canvas_t* canvas, mesh_t* mesh, mat4_t transform);
mesh_t generate_soccer_ball(float radius);

// Mesh utilities
mesh_t create_cube_mesh(float size);
void free_mesh(mesh_t* mesh);

#endif