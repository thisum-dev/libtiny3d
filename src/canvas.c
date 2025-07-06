#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "canvas.h"

canvas_t* create_canvas(int width, int height) {
    canvas_t* c = malloc(sizeof(canvas_t));
    c->width = width;
    c->height = height;
    
    c->pixels = malloc(height * sizeof(float*));
    for (int i = 0; i < height; i++) {
        c->pixels[i] = calloc(width, sizeof(float)); // start all pixels at 0.0
    }
    return c;
}

void free_canvas(canvas_t* c) {
    for (int i = 0; i < c->height; i++) {
        free(c->pixels[i]);
    }
    free(c->pixels);
    free(c);
}

static float maxf(float a, float b) {
    return a > b ? a : b;
}

void set_pixel_f(canvas_t* canvas, float x, float y, float intensity) {
    int x0 = (int)floor(x);
    int y0 = (int)floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    float a = x - x0;
    float b = y - y0;

    float wA = (1 - a) * (1 - b);
    float wB = a * (1 - b);
    float wC = (1 - a) * b;
    float wD = a * b;

    if (x0 >= 0 && y0 >= 0 && x0 < canvas->width && y0 < canvas->height)
        canvas->pixels[y0][x0] += intensity * wA;

    if (x1 >= 0 && y0 >= 0 && x1 < canvas->width && y0 < canvas->height)
        canvas->pixels[y0][x1] += intensity * wB;

    if (x0 >= 0 && y1 >= 0 && x0 < canvas->width && y1 < canvas->height)
        canvas->pixels[y1][x0] += intensity * wC;

    if (x1 >= 0 && y1 >= 0 && x1 < canvas->width && y1 < canvas->height)
        canvas->pixels[y1][x1] += intensity * wD;
}

void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness) {
    float dx = x1 - x0;
    float dy = y1 - y0;
    float length = sqrtf(dx * dx + dy * dy);

    if (length == 0.0f) return;

    float dir_x = dx / length;
    float dir_y = dy / length;
    float perp_x = -dir_y;
    float perp_y = dir_x;

    int steps = (int)(maxf(fabsf(dx), fabsf(dy)) * 2.0f);
    float x_inc = dx / steps;
    float y_inc = dy / steps;

    for (int i = 0; i <= steps; i++) {
        float cx = x0 + i * x_inc;
        float cy = y0 + i * y_inc;

        for (float t = -thickness / 2.0f; t <= thickness / 2.0f; t += 0.5f) {
            float px = cx + t * perp_x;
            float py = cy + t * perp_y;
            set_pixel_f(canvas, px, py, 1.0f);
        }
    }
}

void save_canvas_as_ppm(canvas_t* canvas, const char* filename) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    // Write PPM header
    fprintf(file, "P6\n%d %d\n255\n", canvas->width, canvas->height);

    // Write pixel data
    for (int y = 0; y < canvas->height; y++) {
        for (int x = 0; x < canvas->width; x++) {
            unsigned char pixel[3];
            // Convert grayscale to RGB (same value for R, G, B)
            unsigned char value = (unsigned char)(fminf(canvas->pixels[y][x], 1.0f) * 255);
            pixel[0] = value;
            pixel[1] = value;
            pixel[2] = value;
            fwrite(pixel, 1, 3, file);
        }
    }

    fclose(file);
}