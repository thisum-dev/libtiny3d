#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "math3d.h"
#include "canvas.h"
#include "renderer.h"

#define WIDTH 800
#define HEIGHT 600

mesh_t load_obj_as_mesh(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        // Try with relative path if direct open fails
        char relative_path[256];
        snprintf(relative_path, sizeof(relative_path), "../%s", filename);
        file = fopen(relative_path, "r");
        
        if (!file) {
            fprintf(stderr, "Error: Could not open OBJ file '%s'\n", filename);
            fprintf(stderr, "Please ensure:\n");
            fprintf(stderr, "1. The file exists in the same directory as the executable\n");
            fprintf(stderr, "2. The file is named correctly (case sensitive)\n");
            fprintf(stderr, "3. You have read permissions for the file\n");
            exit(EXIT_FAILURE);
        }
    }

    mesh_t mesh = {0};
    int vertex_count = 0;
    int edge_count = 0;
    char line[256];

    // First pass: Count vertices and potential edges
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            vertex_count++;
        } else if (line[0] == 'f' && line[1] == ' ') {
            // Count vertices in this face to estimate edges
            int vertices_in_face = 0;
            char *ptr = line + 2;
            while (*ptr && *ptr != '\n') {
                if (*ptr == ' ') vertices_in_face++;
                ptr++;
            }
            edge_count += vertices_in_face; // Each vertex in face creates an edge
        }
    }

    // Allocate memory
    mesh.vertices = malloc(sizeof(vertex_t) * vertex_count);
    if (!mesh.vertices) {
        fprintf(stderr, "Memory allocation failed for vertices\n");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    mesh.edges = malloc(sizeof(edge_t) * edge_count);
    if (!mesh.edges) {
        fprintf(stderr, "Memory allocation failed for edges\n");
        free(mesh.vertices);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    mesh.vertex_count = vertex_count;
    mesh.edge_count = 0; // Will count as we add edges

    // Second pass: Read data
    rewind(file);
    int vertex_index = 0;
    int edge_index = 0;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            // Parse vertex
            float x, y, z;
            if (sscanf(line + 2, "%f %f %f", &x, &y, &z) != 3) {
                fprintf(stderr, "Error parsing vertex at line: %s", line);
                continue;
            }
            mesh.vertices[vertex_index].position = vec3_from_cartesian(x, y, z);
            mesh.vertices[vertex_index].intensity = 1.0f;
            vertex_index++;
        } else if (line[0] == 'f' && line[1] == ' ') {
            // Parse face and create edges
            int indices[32];  // Support up to 32 vertices per face
            int count = 0;
            char *token = strtok(line + 2, " ");
            
            while (token && count < 32) {
                int v_idx;
                if (sscanf(token, "%d", &v_idx) == 1) {
                    indices[count++] = v_idx - 1;  // Convert to 0-based index
                }
                token = strtok(NULL, " ");
            }

            // Create edges between consecutive vertices
            for (int i = 0; i < count; i++) {
                int next_i = (i + 1) % count;
                if (indices[i] >= 0 && indices[i] < vertex_count &&
                    indices[next_i] >= 0 && indices[next_i] < vertex_count) {
                    mesh.edges[edge_index].v0 = indices[i];
                    mesh.edges[edge_index].v1 = indices[next_i];
                    mesh.edges[edge_index].depth = 0.0f;
                    edge_index++;
                }
            }
        }
    }

    mesh.edge_count = edge_index;
    fclose(file);
    
    printf("Loaded mesh with %d vertices and %d edges\n", mesh.vertex_count, mesh.edge_count);
    return mesh;
}

int main() {
    const char *obj_filename = "soccer.obj";
    
    printf("Attempting to load %s...\n", obj_filename);
    mesh_t soccer_ball = load_obj_as_mesh(obj_filename);
    canvas_t *canvas = create_canvas(WIDTH, HEIGHT);

    const int num_frames = 60;
    for (int frame = 0; frame < num_frames; frame++) {
        float angle = (2.0f * M_PI * frame) / num_frames;
        
        // Create transformation matrix
        mat4_t rotation_y = mat4_rotate_y(angle);
        mat4_t rotation_x = mat4_rotate_x(angle * 0.5f); // Additional rotation for better visualization
        mat4_t scale = mat4_scale(2.0f, 2.0f, 2.0f);
        mat4_t transform = mat4_multiply(scale, mat4_multiply(rotation_x, rotation_y));

        // Clear canvas
        clear_canvas(canvas, 0.0f);

        // Render using the renderer functions
        render_wireframe(canvas, &soccer_ball, transform);

        char filename[64];
        snprintf(filename, sizeof(filename), "frame_%02d.ppm", frame);
        if (!save_canvas_as_ppm(canvas, filename)) {
            fprintf(stderr, "Failed to save frame %d\n", frame);
        } else {
            printf("Saved %s\n", filename);
        }
    }

    free_canvas(canvas);
    free_mesh(&soccer_ball);
    printf("Animation completed successfully.\n");
    return 0;
}