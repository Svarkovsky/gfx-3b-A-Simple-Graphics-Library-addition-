//  gcc -o demo_alpha_channel demo_alpha_channel.c gfx.c -lX11 -lm

/* demo_alpha_channel.c - Alpha Channel Demo - Demoscene Style - Переписано для gfx.h */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <X11/Xlib.h> // Добавляем для работы с X11
#include "gfx.h"

// Глобальные переменные для размеров окна
int WINDOW_WIDTH;  // Global declaration
int WINDOW_HEIGHT; // Global declaration

// Function for rainbow color with alpha - БЕЗ ИЗМЕНЕНИЙ
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
        gfx_color_alpha(r, g, b, a);
        gfx_fill_rectangle(0, i * bar_height, width, bar_height);
    }
    gfx_color(255, 255, 255);
    gfx_string(10, 10, "1. Raster Bars (Classic)");
}

// 2. Sinus Scroller with alpha trail
void draw_sinus_scroller(int width, int height, float time) {
    char message[] = "GREETINGS FROM ALPHA CHANNEL DEMO - SINUS SCROLLER!";
    int message_len = strlen(message);
    int scroll_speed = 2;
    static int scroll_x = 0; // Инициализируем здесь, будет обновляться

    scroll_x -= scroll_speed;
    if (scroll_x < -message_len * 10) scroll_x = width;

    for (int i = 0; i < message_len; i++) {
        int x = scroll_x + i * 10;
        int y = height / 2 + (int)(sin(time + (float)x / 50) * 50);
        int r, g, b, a;
        float alpha_factor = 1.0f - (float)i / message_len * 0.8f;
        get_rainbow_color_alpha(time + (float)i / message_len * M_PI * 2, alpha_factor, &r, &g, &b, &a);
        gfx_color_alpha(r, g, b, a);
        gfx_string(x, y, &message[i]);
    }
    gfx_color(255, 255, 255);
    gfx_string(10, 40, "2. Sinus Scroller with Alpha Trail");
}

// 3. Enhanced Starfield with Smooth Fading Stars
void draw_starfield(int width, int height, float time) {
    static struct star {
        float x, y, z;
        float speed;
    } stars[300];

    if (stars[0].z == 0.0f) {
        for (int i = 0; i < 300; i++) {
            stars[i].x = (float)(rand() % width);
            stars[i].y = (float)(rand() % height);
            stars[i].z = (float)(rand() % 1000) / 1000.0f;
            stars[i].speed = stars[i].z * 3.0f + 0.5f;
        }
    }

    for (int i = 0; i < 300; i++) {
        stars[i].x -= stars[i].speed;

        if (stars[i].x < 0.0f) {
            stars[i].x = (float)width;
            stars[i].y = (float)(rand() % height);
            stars[i].z = (float)(rand() % 1000) / 1000.0f;
            stars[i].speed = stars[i].z * 3.0f + 0.5f;
        }

        float alpha_factor = stars[i].x / (float)width;
        if (alpha_factor < 0.3f) alpha_factor = 0.3f;

        int r, g, b, a;
        float color_phase = time * 0.5f + stars[i].z;
        get_rainbow_color_alpha(color_phase, alpha_factor, &r, &g, &b, &a);

        int draw_x = (int)stars[i].x;
        int draw_y = (int)stars[i].y;

        if (draw_x >= 0 && draw_x < width && draw_y >= 0 && draw_y < height) {
            gfx_color_alpha(r, g, b, a);
            gfx_point(draw_x, draw_y);

            if (alpha_factor > 0.7f) {
                int tail_length = (int)(stars[i].z * 4.0f);
                for (int t = 1; t <= tail_length && (draw_x + t) < width; t++) {
                    float tail_alpha = alpha_factor * (1.0f - (float)t / tail_length);
                    if (tail_alpha < 0.2f) break;
                    get_rainbow_color_alpha(color_phase, tail_alpha, &r, &g, &b, &a);
                    gfx_color_alpha(r, g, b, a);
                    gfx_point(draw_x + t, draw_y);
                }
            }
        }
    }

    gfx_color(255, 255, 255);
    gfx_string(10, 70, "3. Enhanced Starfield with Smooth Tails");
}

// 4. Wild Alpha Blobs Variation
void draw_wild_alpha_blobs(int width, int height, float time) {
    int num_blobs = 75;
    for (int i = 0; i < num_blobs; i++) {
        int x = rand() % width;
        int y = rand() % height;

        float base_radius = 30.0f;
        float radius_variation = 20.0f * sin(time * 2.0f + (float)i / num_blobs * M_PI * 4.0f);
        float radius = base_radius + radius_variation + (rand() % 10 - 5);
        if (radius < 10) radius = 10;

        int r, g, b, a;
        float color_phase = time * 0.8f + (float)i / num_blobs * M_PI * 2.0f;
        float alpha_factor = 0.6f + 0.4f * sin(time * 1.5f + (float)i / num_blobs * M_PI * 3.0f);
        if (alpha_factor < 0.1f) alpha_factor = 0.1f;

        get_rainbow_color_alpha(color_phase, alpha_factor, &r, &g, &b, &a);

        gfx_color_alpha(r, g, b, a);
        gfx_fill_circle(x, y, (int)radius, (int)radius);
    }

    gfx_color(255, 255, 255);
    gfx_string(10, 100, "4. Wild Alpha Blobs");
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
        if (alpha_factor < 0.1f) alpha_factor = 0.1f;
        get_rainbow_color_alpha(phase * 2, alpha_factor, &r, &g, &b, &a);
        gfx_color_alpha(r, g, b, a);
        gfx_fill_circle(x, y, (int)radius, (int)radius);
    }
    gfx_color(255, 255, 255);
    gfx_string(10, 130, "5. Glowing Circles (Bloom-like)");
}

int main() {
    // Получение размеров экрана через X11
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }
    int screen = DefaultScreen(display);
    WINDOW_WIDTH = DisplayWidth(display, screen);   // Инициализируем глобальную ширину
    WINDOW_HEIGHT = DisplayHeight(display, screen); // Инициализируем глобальную высоту
    XCloseDisplay(display);

    // Открываем окно с динамическими размерами
    gfx_open(WINDOW_WIDTH, WINDOW_HEIGHT, "Alpha Channel Demo - Demoscene Effects");
    gfx_clear_color(0, 0, 0);
    gfx_clear();
    float time = 0;
    int effect_mode = 0;

    while (1) {
        gfx_clear();

        switch (effect_mode) {
            case 0: draw_raster_bars(WINDOW_WIDTH, WINDOW_HEIGHT, time); break;
            case 1: draw_sinus_scroller(WINDOW_WIDTH, WINDOW_HEIGHT, time); break;
            case 2: draw_starfield(WINDOW_WIDTH, WINDOW_HEIGHT, time); break;
            case 3: draw_wild_alpha_blobs(WINDOW_WIDTH, WINDOW_HEIGHT, time); break;
            case 4: draw_glow_circles(WINDOW_WIDTH, WINDOW_HEIGHT, time); break;
            

            default: effect_mode = 0; break;
        }

        gfx_flush();
        usleep(16666); // ~60 FPS
        time += 0.02;
        if (time > 2 * M_PI * 10) time -= 2 * M_PI * 10;

        if (gfx_event_waiting()) {
            char key = gfx_wait();
            if (key == 'q') break;
            if (key == 'n') effect_mode = (effect_mode + 1) % 5;
            if (key == 'p') effect_mode = (effect_mode - 1 + 5) % 5;
        }
    }
    return 0;
}
