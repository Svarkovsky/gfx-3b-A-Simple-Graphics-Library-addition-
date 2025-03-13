// gcc -o alpha_channel alpha_channel.c gfx.c -lX11 -lm -O3 -march=native


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "gfx.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main() {
    gfx_open(WINDOW_WIDTH, WINDOW_HEIGHT, "Alpha Channel Demo");

    // Simple demo using gfx_color_alpha
    for (int i = 0; i < 200; i++) {
        // Calculate a color with varying alpha
        int red = i % 256;
        int green = (i * 2) % 256;
        int blue = (i * 3) % 256;
        int alpha = i; // Vary alpha from 0 to 199

        // Set the drawing color using gfx_color_alpha
        gfx_color_alpha(red, green, blue, alpha);

        // Draw a filled rectangle
        gfx_fill_rectangle(50 + i * 2, 50 + i, 20, 20);

        gfx_flush(); // Force draw
        usleep(10000); // Delay for 10ms
    }

    gfx_wait(); // Wait for a key press
    return 0;
}
