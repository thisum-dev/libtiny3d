#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "math3d.h"
#include "canvas.h"

#define WIDTH 800
#define HEIGHT 600
#define CENTER_X (WIDTH/2.0f)
#define CENTER_Y (HEIGHT/2.0f)

// Cube vertices (8 corners)
vec3_t cube_vertices[] = {
    {-1, -1, -1},  // 0
    { 1, -1, -1},  // 1
    { 1,  1, -1},  // 2
    {-1,  1, -1},  // 3
    {-1, -1,  1},  // 4
    { 1, -1,  1},  // 5
    { 1,  1,  1},  // 6
    {-1,  1,  1}   // 7
};

// Cube edges (12 edges connecting the vertices)
int cube_edges[][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, // Bottom face
    {4, 5}, {5, 6}, {6, 7}, {7, 4}, // Top face
    {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical edges
};

void draw_cube(canvas_t* canvas, mat4_t transform) {
    vec3_t projected[8];
    for (int i = 0; i < 8; i++) {
        vec3_t transformed = mat4_transform_vec3(transform, cube_vertices[i]);
        float z = transformed.z + 5.0f;
        projected[i].x = transformed.x / z * 200.0f + CENTER_X;
        projected[i].y = transformed.y / z * 200.0f + CENTER_Y;
    }

    for (int i = 0; i < 12; i++) {
        int v0 = cube_edges[i][0];
        int v1 = cube_edges[i][1];
        draw_line_f(canvas, 
                   projected[v0].x, projected[v0].y,
                   projected[v1].x, projected[v1].y,
                   1.5f);
    }
}

int main() {
    canvas_t* canvas = create_canvas(WIDTH, HEIGHT);
    
    const int num_frames = 60;
    for (int frame = 0; frame < num_frames; frame++) {
        float angle = (2.0f * M_PI * frame) / num_frames;  // Full 360° rotation
        mat4_t rotation = mat4_rotate_xyz(0.0f, angle, 0.0f);  // Rotate around Y-axis
        mat4_t translation = mat4_translate(0.0f, 0.0f, 0.0f);
        mat4_t transform = mat4_multiply(translation, rotation);
        
        // Clear canvas
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                canvas->pixels[y][x] = 0.0f;
            }
        }

        draw_cube(canvas, transform);

        char filename[64];
        sprintf(filename, "frame_%02d.ppm", frame);
        save_canvas_as_ppm(canvas, filename);
        
        printf("Saved %s\n", filename);
    }
    
    free_canvas(canvas);
    printf("All frames saved.\n");
    
    return 0;
}
