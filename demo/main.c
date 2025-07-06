#include <stdio.h>
#include <math.h>
#include "tiny3d.h"

#define WIDTH 800
#define HEIGHT 800
#define CENTER_X (WIDTH/2.0f)
#define CENTER_Y (HEIGHT/2.0f)
#define RADIUS (WIDTH/2.5f)
#define PI 3.14159265358979323846f

int main() {
    // Create canvas
    canvas_t* canvas = create_canvas(WIDTH, HEIGHT);
    
    // Draw clock face with lines at 15° intervals
    for (int angle = 0; angle < 360; angle += 15) {
        float radians = angle * PI / 180.0f;
        float end_x = CENTER_X + RADIUS * cosf(radians);
        float end_y = CENTER_Y + RADIUS * sinf(radians);
        
        draw_line_f(canvas, CENTER_X, CENTER_Y, end_x, end_y, 2.0f);
    }
    
    // Save the result
    save_canvas_as_ppm(canvas, "clock_face.ppm");
    
    // Clean up
    free_canvas(canvas);
    
    printf("Clock face generated as clock_face.ppm\n");
    return 0;
}