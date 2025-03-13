// gcc -o test_blend_pixel test_blend_pixel.c gfx.c -lX11 -lm -O3 -march=native


#include "gfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

// Define window dimensions
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 400

int main() {
    gfx_open(WINDOW_WIDTH, WINDOW_HEIGHT, "Back Buffer & Blend Pixel Demo (No Lib Mod)");
    gfx_double_buffer_init();

    int center_x = WINDOW_WIDTH / 2;
    int center_y = WINDOW_HEIGHT / 2;
    float time = 0.0f;

    while (1) {
        gfx_double_buffer_clear(0, 0, 0); // Clear back buffer to black

        // --- Indirect back_buffer_data manipulation (Red diagonal line using gfx_double_buffer_fill_rectangle) ---
        gfx_color_alpha(255, 0, 0, 255); // Red, opaque
        for (int i = 0; i < WINDOW_WIDTH; i++) {
            int y = i * WINDOW_HEIGHT / WINDOW_WIDTH;
            if (y < WINDOW_HEIGHT) {
                gfx_double_buffer_point(i, y, 255, 0, 0, 255); // Draw red points for diagonal line
            }
        }

        // --- Test blend_pixel function indirectly using gfx_double_buffer_fill_circle_alpha ---
        int radius = 50;
        int num_circles = 3;
        int colors[3][3] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}}; // Red, Green, Blue
        int alphas[] = {150, 180, 200}; // Different alpha values for circles

        for (int i = 0; i < num_circles; i++) {
            int x_offset = (int)(cos(time + i * 2.0f) * 50);
            int y_offset = (int)(sin(time + i * 2.0f) * 50);
            int circle_x = center_x + x_offset;
            int circle_y = center_y + y_offset;
            int alpha = alphas[i]; // Semi-transparent, varying alpha

            gfx_double_buffer_fill_circle_alpha(circle_x, circle_y, radius, colors[i][0], colors[i][1], colors[i][2], alpha);
        }


        gfx_double_buffer_swap();
        usleep(20000); // 20ms delay for animation
        time += 0.02f;

        if (gfx_event_waiting()) {
            gfx_wait();
            break; // Exit on key press
        }
    }

    gfx_double_buffer_cleanup();
    return 0;
}
