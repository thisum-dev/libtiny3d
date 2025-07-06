#include "renderer.h"
#include "lighting.h"
#include "canvas.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Light setup (global or can be passed)
static light_t scene_light = {
    .direction = { .x = 0.5f, .y = 0.5f, .z = -1.0f },  // Diagonal light
    .intensity = 1.0f
};

// Apply full transformation to vertex, including projection
void project_vertex(vertex_t* vertex, mat4_t transform) {
    vertex->position = mat4_transform_vec3(transform, vertex->position);
}

// Clip point to circular viewport
int clip_to_circular_viewport(canvas_t* canvas, float x, float y) {
    float cx = canvas->width / 2.0f;
    float cy = canvas->height / 2.0f;
    float radius = fminf(cx, cy) * 0.9f;
    float dx = x - cx;
    float dy = y - cy;
    return (dx * dx + dy * dy) <= (radius * radius);
}

// Free memory associated with mesh
void free_mesh(mesh_t* mesh) {
    if (mesh) {
        if (mesh->vertices) free(mesh->vertices);
        if (mesh->edges) free(mesh->edges);
        mesh->vertex_count = 0;
        mesh->edge_count = 0;
    }
}

// Main wireframe rendering function (with lighting)
void render_wireframe(canvas_t* canvas, mesh_t* mesh, mat4_t transform) {
    // Step 1: Project all vertices
    for (int i = 0; i < mesh->vertex_count; i++) {
        project_vertex(&mesh->vertices[i], transform);
    }

    // Step 2: Draw all edges with lighting
    for (int i = 0; i < mesh->edge_count; i++) {
        vertex_t* v0 = &mesh->vertices[mesh->edges[i].v0];
        vertex_t* v1 = &mesh->vertices[mesh->edges[i].v1];

        // Convert to screen space
        float x0 = v0->position.x * canvas->width * 0.4f + canvas->width / 2;
        float y0 = v0->position.y * canvas->height * 0.4f + canvas->height / 2;
        float x1 = v1->position.x * canvas->width * 0.4f + canvas->width / 2;
        float y1 = v1->position.y * canvas->height * 0.4f + canvas->height / 2;

        if (clip_to_circular_viewport(canvas, x0, y0) &&
            clip_to_circular_viewport(canvas, x1, y1)) {

            // Compute direction from v0 to v1 for lighting
            vec3_t edge_dir = vec3_subtract(v1->position, v0->position);
            float brightness = compute_lambert_intensity(edge_dir, scene_light);

            // Optional: clamp brightness to avoid invisible lines
            if (brightness < 0.05f) brightness = 0.05f;

            // Use brightness to scale line thickness or intensity
            draw_line_f(canvas, x0, y0, x1, y1, 1.5f * brightness);
        }
    }
}

// Create a cube mesh centered at origin
mesh_t create_cube_mesh(float size) {
    float s = size / 2.0f;

    mesh_t mesh;
    mesh.vertex_count = 8;
    mesh.edge_count = 12;
    mesh.vertices = malloc(sizeof(vertex_t) * mesh.vertex_count);
    mesh.edges = malloc(sizeof(edge_t) * mesh.edge_count);

    // Cube vertices (same order as your cube wireframe)
    vec3_t positions[8] = {
        {-s, -s, -s}, {s, -s, -s}, {s, s, -s}, {-s, s, -s},
        {-s, -s,  s}, {s, -s,  s}, {s, s,  s}, {-s, s,  s}
    };

    for (int i = 0; i < 8; i++) {
        mesh.vertices[i].position = positions[i];
        mesh.vertices[i].intensity = 1.0f; // optional
    }

    // Cube edges
    int edge_indices[12][2] = {
        {0,1},{1,2},{2,3},{3,0}, // back face
        {4,5},{5,6},{6,7},{7,4}, // front face
        {0,4},{1,5},{2,6},{3,7}  // vertical
    };

    for (int i = 0; i < 12; i++) {
        mesh.edges[i].v0 = edge_indices[i][0];
        mesh.edges[i].v1 = edge_indices[i][1];
        mesh.edges[i].depth = 0.0f; // unused
    }

    return mesh;
}

