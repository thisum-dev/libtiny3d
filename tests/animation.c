// animation.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "math3d.h"
#include "canvas.h"

#define WIDTH 800
#define HEIGHT 600
#define CENTER_X (WIDTH / 2.0f)
#define CENTER_Y (HEIGHT / 2.0f)
#define NUM_FRAMES 120
#define CIRCLE_RADIUS 2.5f  // Radius of circular motion

// Cube vertices (8 corners)
vec3_t cube_vertices[] = {
    {-1, -1, -1},
    { 1, -1, -1},
    { 1,  1, -1},
    {-1,  1, -1},
    {-1, -1,  1},
    { 1, -1,  1},
    { 1,  1,  1},
    {-1,  1,  1}
};

// Cube edges (12 edges connecting the vertices)
int cube_edges[][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0},
    {4, 5}, {5, 6}, {6, 7}, {7, 4},
    {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

// Draw a cube with transformation matrix
void draw_cube(canvas_t* canvas, mat4_t transform) {
    vec3_t projected[8];
    for (int i = 0; i < 8; i++) {
        vec3_t transformed = mat4_transform_vec3(transform, cube_vertices[i]);
        float z = transformed.z + 7.0f;  // Camera distance
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

    for (int frame = 0; frame < NUM_FRAMES; frame++) {
        // Clear canvas
        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                canvas->pixels[y][x] = 0.0f;
            }
        }

        float t = (float)frame / NUM_FRAMES;
        float angle = t * 2.0f * M_PI;

        // Cube 1: Clockwise
        float x1 = CIRCLE_RADIUS * cosf(angle);
        float y1 = CIRCLE_RADIUS * sinf(angle);
        mat4_t translate1 = mat4_translate(x1, y1, 0.0f);
        mat4_t rotate1 = mat4_rotate_xyz(angle * 2, angle * 2, angle * 2);
        mat4_t transform1 = mat4_multiply(translate1, rotate1);

        // Cube 2: Counterclockwise
        float x2 = CIRCLE_RADIUS * cosf(-angle);
        float y2 = CIRCLE_RADIUS * sinf(-angle);
        mat4_t translate2 = mat4_translate(x2, y2, 0.0f);
        mat4_t rotate2 = mat4_rotate_xyz(-angle * 2, -angle * 2, -angle * 2);
        mat4_t transform2 = mat4_multiply(translate2, rotate2);

        // Draw both cubes
        draw_cube(canvas, transform1);
        draw_cube(canvas, transform2);

        // Save frame
        char filename[64];
        sprintf(filename, "frame_%03d.ppm", frame);
        save_canvas_as_ppm(canvas, filename);
        printf("Saved %s\n", filename);
    }

    free_canvas(canvas);
    printf("Animation completed.\n");
    return 0;
}
