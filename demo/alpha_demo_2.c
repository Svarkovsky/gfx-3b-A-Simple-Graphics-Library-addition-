// gcc -o alpha_demo_2 alpha_demo_2.c gfx.c -lX11 -lm

// 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "gfx.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Function for rainbow color with alpha
void get_rainbow_color_alpha(float phase, float alpha_factor, int *r, int *g, int *b, int *a) {
    float red = sin(phase + 0) * 0.5 + 0.5;
    float green = sin(phase + 2.094) * 0.5 + 0.5;
    float blue = sin(phase + 4.188) * 0.5 + 0.5;

    *r = (int)(red * 255);
    *g = (int)(green * 255);
    *b = (int)(blue * 255);
    *a = (int)(alpha_factor * 255);
}

// 1. Raster Bars effect (classic demoscene)
void draw_raster_bars(int width, int height, float time) {
    int bar_height = 20;
    int num_bars = height / bar_height;
    for (int i = 0; i < num_bars; i++) {
        float phase = time + (float)i / num_bars * M_PI * 2;
        int r, g, b, a;
        get_rainbow_color_alpha(phase, 0.7f, &r, &g, &b, &a);
        gfx_double_buffer_fill_rectangle(0, i * bar_height, width, bar_height, r, g, b, a);
    }
    gfx_double_buffer_fill_rectangle(10, 10, 200, 20, 255, 255, 255, 255); // White background for text
    gfx_string(10, 10, "1. Raster Bars (Classic)");
}

// 2. Sinus Scroller with alpha trail
void draw_sinus_scroller(int width, int height, float time) {
    char message[] = "GREETINGS FROM ALPHA CHANNEL DEMO - SINUS SCROLLER!";
    int message_len = strlen(message);
    int scroll_speed = 2;
    static int scroll_x = WINDOW_WIDTH; // Start from right edge

    scroll_x -= scroll_speed;
    if (scroll_x < -message_len * 10) scroll_x = WINDOW_WIDTH;

    for (int i = 0; i < message_len; i++) {
        int x = scroll_x + i * 10;
        int y = height / 2 + (int)(sin(time + (float)x / 50) * 50);
        int r, g, b, a;
        float alpha_factor = 1.0f - (float)i / message_len * 0.8f; // Alpha trail
        get_rainbow_color_alpha(time + (float)i / message_len * M_PI * 2, alpha_factor, &r, &g, &b, &a);
        // No direct alpha support for strings, so we use a point-based approximation
        gfx_double_buffer_point(x, y, r, g, b, a);
    }
    gfx_double_buffer_fill_rectangle(10, 40, 250, 20, 255, 255, 255, 255); // White background for text
    gfx_string(10, 40, "2. Sinus Scroller with Alpha Trail");
}

// 3. Starfield with fading stars
void draw_starfield(int width, int height, float time) {
    static struct star {
        int x, y;
        float z;
    } stars[200]; // Static array of stars

    if (stars[0].z == 0) { // Initialize stars only once
        for (int i = 0; i < 200; i++) {
            stars[i].x = rand() % width;
            stars[i].y = rand() % height;
            stars[i].z = (float)(rand() % 1000) / 1000.0f; // Z-depth for speed
        }
    }

    for (int i = 0; i < 200; i++) {
        stars[i].x -= (int)(stars[i].z * 5 + 1); // Speed based on Z-depth
        if (stars[i].x < 0) {
            stars[i].x = width;
            stars[i].y = rand() % height;
        }
        int r, g, b, a;
        float alpha_factor = stars[i].z; // Fading based on Z-depth
        get_rainbow_color_alpha(stars[i].z * time * 5, alpha_factor, &r, &g, &b, &a);
        gfx_double_buffer_point(stars[i].x, stars[i].y, r, g, b, a);
    }
    gfx_double_buffer_fill_rectangle(10, 70, 220, 20, 255, 255, 255, 255); // White background for text
    gfx_string(10, 70, "3. Starfield with Fading Stars");
}

// 4. Plasma effect (simplified, alpha blended)
void draw_plasma(int width, int height, float time) {
    for (int y = 0; y < height; y += 4) { // Reduced step for speed
        for (int x = 0; x < width; x += 4) {
            float plasma_value = sin(x / 50.0 + time) + sin(y / 50.0 + time) + sin((x + y) / 30.0 + time);
            plasma_value = (plasma_value + 3.0) / 6.0; // Normalize to 0-1 range
            int r, g, b, a;
            get_rainbow_color_alpha(plasma_value * M_PI * 4, plasma_value * 0.8f, &r, &g, &b, &a);
            gfx_double_buffer_fill_rectangle(x, y, 4, 4, r, g, b, a);
        }
    }
    gfx_double_buffer_fill_rectangle(10, 100, 220, 20, 255, 255, 255, 255); // White background for text
    gfx_string(10, 100, "4. Plasma Effect (Alpha Blended)");
}

// 5. Glowing Circles (bloom-like effect using alpha)
void draw_glow_circles(int width, int height, float time) {
    int num_glow_circles = 20;
    for (int i = 0; i < num_glow_circles; i++) {
        float phase = time + (float)i / num_glow_circles * M_PI * 2;
        float radius = 60 + 40 * sin(phase * 3);
        int x = width / 2 + (int)(150 * cos(phase * 0.8));
        int y = height / 2 + (int)(100 * sin(phase * 1.2));
        int r, g, b, a;
        float alpha_factor = 0.9f - radius / 200.0f;
        if (alpha_factor < 0.1f) alpha_factor = 0.1f; // Clamp alpha
        get_rainbow_color_alpha(phase * 2, alpha_factor, &r, &g, &b, &a);
        gfx_double_buffer_fill_circle(x, y, (int)radius, r, g, b, a);
    }
    gfx_double_buffer_fill_rectangle(10, 130, 230, 20, 255, 255, 255, 255); // White background for text
    gfx_string(10, 130, "5. Glowing Circles (Bloom-like)");
}

// 6. Moving Rectangles with color cycling
void draw_moving_rects(int width, int height, float time) {
    int num_rects = 10;
    for (int i = 0; i < num_rects; i++) {
        float rect_time = time + (float)i * 0.2f;
        int rect_width = 50 + (int)(20 * sin(rect_time * 2));
        int rect_height = 50 + (int)(20 * cos(rect_time * 3));
        int x = (int)(width * 0.5f + 100 * cos(rect_time * 1.5f)) - rect_width / 2;
        int y = (int)(height * 0.5f + 80 * sin(rect_time * 2.5f)) - rect_height / 2;
        int r, g, b, a;
        get_rainbow_color_alpha(rect_time * 4, 1.0f, &r, &g, &b, &a);
        gfx_double_buffer_fill_rectangle(x, y, rect_width, rect_height, r, g, b, a);
    }
    gfx_double_buffer_fill_rectangle(10, 160, 180, 20, 255, 255, 255, 255); // White background for text
    gfx_string(10, 160, "6. Moving Rectangles");
}

int main() {
    gfx_open(WINDOW_WIDTH, WINDOW_HEIGHT, "Alpha Channel Demo - Demoscene Effects");
    gfx_double_buffer_init(); // Initialize double buffering
    float time = 0;
    int effect_mode = 0;

    while (1) {
        gfx_double_buffer_clear(0, 0, 0); // Clear back buffer

        switch (effect_mode) {
            case 0: draw_raster_bars(WINDOW_WIDTH, WINDOW_HEIGHT, time); break;
            case 1: draw_sinus_scroller(WINDOW_WIDTH, WINDOW_HEIGHT, time); break;
            case 2: draw_starfield(WINDOW_WIDTH, WINDOW_HEIGHT, time); break;
            case 3: draw_plasma(WINDOW_WIDTH, WINDOW_HEIGHT, time); break;
            case 4: draw_glow_circles(WINDOW_WIDTH, WINDOW_HEIGHT, time); break;
            case 5: draw_moving_rects(WINDOW_WIDTH, WINDOW_HEIGHT, time); break;
            default: effect_mode = 0; break;
        }

        gfx_double_buffer_swap(); // Swap buffers to display

        usleep(16666); // Approximately 60 FPS
        time += 0.02;
        if (time > 2 * M_PI * 10) time -= 2 * M_PI * 10;

        if (gfx_event_waiting()) {
            char key = gfx_wait();
            if (key == 'q') break;
            if (key == 'n') effect_mode = (effect_mode + 1) % 6;
            if (key == 'p') effect_mode = (effect_mode - 1 + 6) % 6;
        }
    }
    gfx_double_buffer_cleanup(); // Clean up double buffering
    return 0;
}