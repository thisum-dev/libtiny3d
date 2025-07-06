#ifndef CANVAS_H
#define CANVAS_H

typedef struct {
    int width;
    int height;
    float** pixels; // 2D array: brightness at each pixel [0.0 to 1.0]
} canvas_t;

// Function declarations
canvas_t* create_canvas(int width, int height);
void free_canvas(canvas_t* canvas);
void set_pixel_f(canvas_t* canvas, float x, float y, float intensity);
void draw_line_f(canvas_t* canvas, float x0, float y0, float x1, float y1, float thickness);
void save_canvas_as_ppm(canvas_t* canvas, const char* filename);

#endif