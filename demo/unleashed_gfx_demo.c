// gcc -o unleashed_gfx_demo unleashed_gfx_demo.c gfx.c -lX11 -lm

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <X11/Xlib.h>
#include "gfx.h"

int WINDOW_WIDTH;  // Global declaration
int WINDOW_HEIGHT; // Global declaration


// --- Helper Functions ---

// Function to get enhanced color (for mode 0)
void get_enhanced_color(float phase, float value, int *r, int *g, int *b) {
    float freq = 0.3;
    *r = fabs(sin(freq * phase + 0)) * 255 * value;
    *g = fabs(sin(freq * phase + 2.094)) * 255 * value;
    *b = fabs(sin(freq * phase + 4.188)) * 255 * value;
}

// Function to get rainbow color
void get_rainbow_color(float phase, int *r, int *g, int *b) {
    float red = sin(phase + 0) * 0.5 + 0.5;
    float green = sin(phase + 2.094) * 0.5 + 0.5; // 2pi/3
    float blue = sin(phase + 4.188) * 0.5 + 0.5;  // 4pi/3
    *r = (int)(red * 255);
    *g = (int)(green * 255);
    *b = (int)(blue * 255);
}

// Function to get pulse color for Mode 2
typedef struct {
    int r, g, b;
} Color;

void get_pulse_color(float phase, float distance, Color *color) {
    float intensity = sin(phase + distance * 0.1) * 0.5 + 0.5;
    color->r = (int)(sin(phase) * 127 + 128 * intensity);
    color->g = (int)(sin(phase + 2.094) * 127 + 128 * intensity);
    color->b = (int)(sin(phase + 4.188) * 127 + 128 * intensity);
}

// Function to draw pulse effect for Mode 2
void draw_pulse_effect(int width, int height, float phase, int mouse_x, int mouse_y) {
    Color color;

    for (int x = 0; x < width; x += 8) {
        for (int y = 0; y < height; y += 8) {
            float dist = sqrt(pow(x - mouse_x, 2) + pow(y - mouse_y, 2));
            float size = 2 + sin(phase + dist * 0.05) * 2;

            get_pulse_color(phase, dist, &color);
            gfx_color(color.r, color.g, color.b);

            if (dist < 200) {
                gfx_circle(x, y, size, size);
            }
        }
    }
}


// --- Mode Functions ---

// Mode 0: Hypnotic Swirling Lines (Enhanced)
void draw_swirling_lines(int width, int height, float phase) {
    const int layers = 5;
    for (int l = 0; l < layers; l++) {
        float layer_phase = phase * (l + 1) * 0.2;
        for (int i = 0; i < 200; i++) {
            float angle1 = (float)i / 200 * 8 * M_PI + layer_phase;
            float angle2 = angle1 * 2.5 + sin(layer_phase);
            int x1 = width / 2 + cos(angle1) * (width / 8 + width / 16 * sin(phase * 0.5 + l));
            int y1 = height / 2 + sin(angle1) * (height / 8 + height / 16 * cos(phase * 0.3 + l));
            int x2 = width / 2 + cos(angle2) * (width / 4 + width / 10 * sin(phase * 0.7 + l));
            int y2 = height / 2 + sin(angle2) * (height / 4 + height / 10 * cos(phase * 0.9 + l));
            int r, g, b;
            get_enhanced_color(angle1 + phase, 0.7 + 0.3 * sin(phase), &r, &g, &b);
            gfx_color(r, g, b);
            gfx_line(x1, y1, x2, y2);
        }
    }
}

// Mode 1: Expanding Circles - Concentric circles pulsing
void draw_expanding_circles(int width, int height, float color_phase, int circle_count) {
    int r, g, b;
    for (int i = 0; i < circle_count; i++) {
        float radius = (float)i / circle_count * width / 4 + (sin(color_phase) * width / 16);
        get_rainbow_color(color_phase + (float)i / circle_count * M_PI, &r, &g, &b);
        gfx_color(r, g, b);
        gfx_circle(width / 2, height / 2, (int)radius, (int)radius);
    }
}

// Mode 2: Enhanced Pixel Sampler - RGB under mouse, click to shift
void draw_pixel_sampler(int width, int height, float phase) {
    
    int mouse_x = gfx_xpos(), mouse_y = gfx_ypos();

    for (int y = 0; y < height; y++) {
        int gradient = (int)(y * 50.0 / height);
        gfx_color(gradient, gradient, gradient);
        gfx_line(0, y, width, y);
    }

    draw_pulse_effect(width, height, phase, mouse_x, mouse_y);

    int pixel_color = GetPix(mouse_x, mouse_y);
    int r = (pixel_color >> 16) & 0xFF;
    int g = (pixel_color >> 8) & 0xFF;
    int b = pixel_color & 0xFF;

    gfx_color(0, 0, 0);
    gfx_fill_rectangle(width - 150, 10, 140, 100);
    gfx_color(r, g, b);
    gfx_fill_rectangle(width - 140, 20, 120, 30);

    gfx_color(200, 200, 200);
    gfx_rectangle(width - 150, 10, 140, 100);

    char info2[100]; 
    sprintf(info2, "R: %3d G: %3d B: %3d", r, g, b);
    gfx_color(255, 255, 255);
    gfx_string(width - 140, 70, info2);

    sprintf(info2, "X: %3d Y: %3d", mouse_x, mouse_y);
    gfx_color(255, 255, 255);
    gfx_string(width - 140, 90, info2);

    gfx_color(255, 255, 255);
    gfx_string(10, 40, "Click: Pulse | 'q': Quit"); 

    gfx_color(255 - r, 255 - g, 255 - b);
    gfx_line(mouse_x - 10, mouse_y, mouse_x + 10, mouse_y);
    gfx_line(mouse_x, mouse_y - 10, mouse_x, mouse_y + 10);

    gfx_color(255, 255, 255);
    char info[100];
    sprintf(info, "Mode 2: Enhanced Pixel Sampler - RGB under mouse, click to pulse"); // 
    gfx_string(10, 20, info);
}

// Mode 3: Moving Rectangles - Bouncing rectangles
void draw_moving_rectangles(int width, int height, float color_phase, int rect_count) {
    static int rect_positions[30][2], rect_velocities[30][2];
    int r, g, b;
    for (int i = 0; i < rect_count; i++) {
        if (rect_positions[i][0] == 0 && rect_positions[i][1] == 0) {
            rect_positions[i][0] = rand() % width;
            rect_positions[i][1] = rand() % height;
            rect_velocities[i][0] = (rand() % 5) - 2;
            rect_velocities[i][1] = (rand() % 5) - 2;
            if (rect_velocities[i][0] == 0 && rect_velocities[i][1] == 0) rect_velocities[i][0] = 1;
        }
        rect_positions[i][0] += rect_velocities[i][0];
        rect_positions[i][1] += rect_velocities[i][1];
        if (rect_positions[i][0] < 0 || rect_positions[i][0] > width - width / 20) rect_velocities[i][0] = -rect_velocities[i][0];
        if (rect_positions[i][1] < 0 || rect_positions[i][1] > height - height / 20) rect_velocities[i][1] = -rect_velocities[i][1];
        get_rainbow_color(color_phase + (float)i / rect_count * M_PI * 2, &r, &g, &b);
        gfx_color(r, g, b);
        gfx_fill_rectangle(rect_positions[i][0], rect_positions[i][1], width / 20, height / 20);
    }
}

// Mode 4: Dynamic Text Rhyme - Rhyme with sinusoidal & wave distortion motion
void draw_text_scroller(int width, int height, float color_phase, int message_scroll_x) {
    char *rhyme[4] = {
        "Twinkle, twinkle, little star,", // Line 1
        "How I wonder what you are.",      // Line 2
        "Up above the world so high,",    // Line 3
        "Like a diamond in the sky."      // Line 4
    };
    int r, g, b;
    int num_lines = 4;
    float amplitude_y = height / 10.0;
    float phase_shift_step = 2 * M_PI / 3.0;
    float letter_wave_amplitude = 5.0;

    for (int i = 0; i < num_lines; i++) {
        int x_line_start = message_scroll_x;
        int y_center = height / 2 + i * 40;
        float y_offset = sin(color_phase + i * phase_shift_step) * amplitude_y;
        int y_line = y_center + (int)y_offset;

        get_rainbow_color(color_phase + i * 0.5, &r, &g, &b);
        gfx_color(r, g, b);

        // Очистка фона для текста
        gfx_color(0, 0, 0);
        gfx_fill_rectangle(x_line_start - 10, y_line - 20, strlen(rhyme[i]) * 16, 40);

        gfx_color(r, g, b);

        int current_x = x_line_start;
        for (int j = 0; j < strlen(rhyme[i]); j++) {
            float letter_x_offset = sin(color_phase * 2.0 + (float)j * 0.8) * letter_wave_amplitude;
            char current_char[2] = {rhyme[i][j], '\0'}; 
            gfx_string(current_x + (int)letter_x_offset, y_line, current_char); 
            current_x += 16; 
        }
    }
}


// Mode 5: Dynamic 3D Pulse - Pulsating Depth (REPLACED Window Wobble)
void draw_dynamic_pulse_3d(int width, int height, float time) {
    static float circle_positions[30][3];
    static int initialized = 0;

    if (!initialized) {
        for (int i = 0; i < 30; i++) {
            circle_positions[i][0] = rand() % width;
            circle_positions[i][1] = rand() % height;
            circle_positions[i][2] = (float)rand() / RAND_MAX;
        }
        initialized = 1;
    }

    gfx_clear();

    int center_x = width / 2, center_y = height / 2;
    for (int r = 0; r < width / 2; r += 10) {
        float gradient_val = (float)r / (width / 2) * 0.2;
        int bg_r, bg_g, bg_b;
        get_enhanced_color(time * 0.3 + gradient_val * 3, gradient_val, &bg_r, &bg_g, &bg_b);
        gfx_color(bg_r, bg_g, bg_b);
        gfx_fill_circle(center_x, center_y, r, r);
    }

    float wobble_amplitude_x = 15 + 10 * sin(time * 0.5);
    float wobble_amplitude_y = 8 + 5 * cos(time * 0.7);
    float wobble_x = wobble_amplitude_x * sin(time * 1.0);
    float wobble_y = wobble_amplitude_y * cos(time * 1.2);
    gfx_move_win_r(0, (int)wobble_x, (int)wobble_y, 0, 0);

    for (int i = 0; i < 30; i++) {
        float z = circle_positions[i][2];
        float depth_factor = 0.5 + z * 0.5;
        float pulse_factor = 1.0 + 0.3 * sin(time * 2.0 + z * 5);
        int radius = (int)((width / 30) * depth_factor * pulse_factor);
        int x = (int)(circle_positions[i][0] + wobble_x * depth_factor * 0.5);
        int y = (int)(circle_positions[i][1] + wobble_y * depth_factor * 0.5);

        int r, g, b;
        get_rainbow_color(time + z * 3 + (float)i / 30 * M_PI * 2, &r, &g, &b);
        gfx_color(r, g, b);
        gfx_fill_circle(x, y, radius, radius);

        circle_positions[i][2] += 0.015 * depth_factor;
        if (circle_positions[i][2] > 1.2) {
            circle_positions[i][2] = 0.0;
            circle_positions[i][0] = rand() % width;
            circle_positions[i][1] = rand() % height;
        }
    }

    gfx_color(255, 255, 255);
    char info[100];
    sprintf(info, "Mode 5: Dynamic 3D Pulse - Pulsating Depth");
    gfx_string(10, 30, info);
}

// Mode 6: Cosmic Voyage - Speeding through the Universe
void draw_mixed_graphics(int width, int height, float time) {
    static float star_positions[500][3];
    static float nebula_positions[10][4];
    static int initialized = 0;

    if (!initialized) {
        for (int i = 0; i < 500; i++) {
            star_positions[i][0] = rand() % width;
            star_positions[i][1] = rand() % height;
            star_positions[i][2] = (float)rand() / RAND_MAX;
        }
        for (int i = 0; i < 10; i++) {
            nebula_positions[i][0] = rand() % width;
            nebula_positions[i][1] = rand() % height;
            nebula_positions[i][2] = 50 + rand() % 100;
            nebula_positions[i][3] = (float)rand() / RAND_MAX * 2 * M_PI;
        }
        initialized = 1;
    }

    gfx_clear_color(0, 0, 0);
    gfx_clear();

    for (int i = 0; i < 500; i++) {
        float z = star_positions[i][2];
        int star_size = (int)(1 + z * 2);
        int brightness = (int)(150 + z * 105);
        gfx_color(brightness, brightness, brightness);
        gfx_fill_rectangle(star_positions[i][0], star_positions[i][1], star_size, star_size);

        star_positions[i][0] -= 1 * (1 + z * 2) ;
        if (star_positions[i][0] < 0) {
            star_positions[i][0] = width + star_size;
            star_positions[i][1] = rand() % height;
            star_positions[i][2] = (float)rand() / RAND_MAX;
        }
    }

    for (int i = 0; i < 10; i++) {
        int nebula_r, nebula_g, nebula_b;
        get_rainbow_color(nebula_positions[i][3] + time * 0.2, &nebula_r, &nebula_g, &nebula_b);
        gfx_color(nebula_r, nebula_g, nebula_b);
        int nebula_size = nebula_positions[i][2] + 20 * sin(time + nebula_positions[i][3]);
        for (int j = 0; j < 10; j++) {
            gfx_circle(nebula_positions[i][0], nebula_positions[i][1], nebula_size - j * (nebula_size / 10), nebula_size - j * (nebula_size / 10));
        }

        nebula_positions[i][0] -= 0.5;
        nebula_positions[i][1] += sin(time * 0.1 + nebula_positions[i][3]) * 0.3;
        if (nebula_positions[i][0] < -nebula_positions[i][2]) {
            nebula_positions[i][0] = width + nebula_positions[i][2];
            nebula_positions[i][1] = rand() % height;
            nebula_positions[i][2] = 50 + rand() % 100;
            nebula_positions[i][3] = (float)rand() / RAND_MAX * 2 * M_PI;
        }
        nebula_positions[i][3] += 0.01;
    }

    gfx_color(255, 255, 255);
    char info[100];
    sprintf(info, "Mode 6: Cosmic Voyage - Speeding through the Universe");
    gfx_string(10, 30, info);
}

// Mode 7: Interactive Input Visualizer - Circles and lines
void draw_input_visualizer(int width, int height, char *input_text) {
    static int last_x = -1, last_y = -1;
    static float hue = 0.0f;
    static int bg_color_r = 0, bg_color_g = 0, bg_color_b = 0;
    static int circle_radius = 10;
    static int circle_growth_rate = 1;

    hue += 0.005f;
    if (hue > 1.0f) hue -= 1.0f;
    float r_bg_f, g_bg_f, b_bg_f;
    float val = 0.1f + 0.05f * sin(hue * 2 * M_PI * 2);
    get_enhanced_color(hue * 2 * M_PI, val, &bg_color_r, &bg_color_g, &bg_color_b);
    gfx_clear_color(bg_color_r, bg_color_g, bg_color_b);
    gfx_clear();

    gfx_color(255, 255, 255);
    gfx_string(10, 30, "Mode 7: Interactive Input Visualizer");
    gfx_string(10, 50, input_text);

    int mouse_x = gfx_xpos(), mouse_y = gfx_ypos();

    if (last_x != -1 && last_y != -1) {
        float dist = sqrt(pow(mouse_x - last_x, 2) + pow(mouse_y - last_y, 2));
        float pulse_phase = fmod(hue * 5.0, 2 * M_PI);
        int line_r, line_g, line_b;
        get_rainbow_color(pulse_phase + hue * 2 * M_PI, &line_r, &line_g, &line_b);
        gfx_color(line_r, line_g, line_b);
        gfx_line(last_x, last_y, mouse_x, mouse_y);

        gfx_color(255, 255, 255);
        for (int i = 0; i < dist; i += 15) {
            float t = (float)i / dist;
            int point_x = last_x + (mouse_x - last_x) * t;
            int point_y = last_y + (mouse_y - last_y) * t;
            gfx_point(point_x, point_y);
        }
    }

    int current_circle_radius = circle_radius + (int)(10 * sin(hue * 2.0));
    int circle_r, circle_g, circle_b;
    get_rainbow_color(hue * 2 * M_PI, &circle_r, &circle_g, &circle_b);
    gfx_color(circle_r, circle_g, circle_b);
    gfx_circle(mouse_x, mouse_y, current_circle_radius, current_circle_radius);

    for (int i = 1; i <= 5; i++) {
        int echo_radius = current_circle_radius + i * 5;
        float echo_hue = hue + (float)i / 10.0f;
        if (echo_hue > 1.0f) echo_hue -= 1.0f;
        int echo_r, echo_g, echo_b;
        get_rainbow_color(echo_hue * 2 * M_PI, &echo_r, &echo_g, &echo_b);
        gfx_color(echo_r, echo_g, echo_b);
        gfx_circle(mouse_x, mouse_y, echo_radius, echo_radius);
    }
    gfx_color(255, 255, 255);

    circle_radius += circle_growth_rate;
    if (circle_radius > 30 || circle_radius < 5) {
        circle_growth_rate = -circle_growth_rate;
    }
    if (circle_radius < 5) circle_radius = 5;

    last_x = mouse_x;
    last_y = mouse_y;
}

// Mode 8: Mandelbrot Set
typedef struct { int iter; int r, g, b; } Pixel;
void init_mandelbrot(Pixel *pixels, int width, int height, double zoom, double offset_x, double offset_y) {
    const int max_iter = 100;
    double scale = 4.0 / (width * zoom);
    for (int y = 0; y < height; y += 2) {
        for (int x = 0; x < width; x += 2) {
            double real = (x - width / 2.0) * scale + offset_x;
            double imag = (y - height / 2.0) * scale + offset_y;
            double zx = 0.0, zy = 0.0;
            int iter = 0;
            while (zx * zx + zy * zy < 4.0 && iter < max_iter) {
                double zxtemp = zx * zx - zy * zy + real;
                zy = 2 * zx * zy + imag;
                zx = zxtemp;
                iter++;
            }
            int idx = (y / 2) * (width / 2) + (x / 2);
            pixels[idx].iter = iter;
            if (iter == max_iter) {
                pixels[idx].r = 0; pixels[idx].g = 0; pixels[idx].b = 0;
            } else {
                get_rainbow_color((float)iter / max_iter * 6.28, &pixels[idx].r, &pixels[idx].g, &pixels[idx].b);
            }
            gfx_color(pixels[idx].r, pixels[idx].g, pixels[idx].b);
            gfx_fill_rectangle(x, y, 2, 2);
        }
    }
}
void update_mandelbrot(Pixel *pixels, int width, int height, double time, double zoom, double offset_x, double offset_y) {
    const int max_iter = 100;
    for (int y = 0; y < height; y += 2) {
       // for (int x = 0; x < width; x < width; x += 2) { // Corrected for loop here
	for (int x = 0; x < width; x += 2) { // Corrected for loop here
            int idx = (y / 2) * (width / 2) + (x / 2);
            if (pixels[idx].iter != max_iter) {
                get_rainbow_color((float)pixels[idx].iter / max_iter * 6.28 + time, &pixels[idx].r, &pixels[idx].g, &pixels[idx].b);
                gfx_color(pixels[idx].r, pixels[idx].g, pixels[idx].b);
                gfx_fill_rectangle(x, y, 2, 2);
            }
        }
    }
    char info[100];
    sprintf(info, "Mode 8: Mandelbrot Set - Zoom: %.2f, Offset: (%.2f, %.2f)", 4.0 / (width * zoom), offset_x, offset_y);
    gfx_color(255, 255, 255);
    gfx_string(10, 30, info);
}

// Mode 9: Lissajous Curves
void draw_lissajous(int width, int height, float phase) {
    const int num_points = 1000;
    float a = 3.0 + sin(phase * 0.5), b = 4.0 + cos(phase * 0.3), delta = phase;
    for (int i = 0; i < num_points; i++) {
        float t = (float)i / num_points * 2 * M_PI;
        int x = width / 2 + (int)(width / 6 * sin(a * t + delta));
        int y = height / 2 + (int)(height / 6 * sin(b * t));
        int r, g, b;
        get_rainbow_color(t + phase, &r, &g, &b);
        gfx_color(r, g, b);
        gfx_point(x, y);
    }
}

// Mode 10: Wave Interference - with text display
void draw_wave_interference(int width, int height, float time) {
    static float last_amplitude[1920 * 1080 / 25];
    int idx = 0;
    float source_x[3] = {width / 4 + sin(time) * width / 8, width / 2, 3 * width / 4 + cos(time) * width / 8};
    float source_y[3] = {height / 2, height / 4 + sin(time * 1.5) * height / 8, 3 * height / 4};
    for (int y = 0; y < height; y += 5) {
        for (int x = 0; x < width; x += 5) {
            float amplitude = 0.0;
            for (int s = 0; s < 3; s++) {
                float dx = x - source_x[s];
                float dy = y - source_y[s];
                float r = sqrt(dx * dx + dy * dy);
                amplitude += sin(r / 20.0 - time * 2.0);
            }
            if (idx < width * height / 25) {
                amplitude = (amplitude + last_amplitude[idx]) / 2;
                last_amplitude[idx] = amplitude;
                idx++;
            }
            int intensity = (int)((amplitude + 3.0) / 6.0 * 255);
            gfx_color(intensity, 255 - intensity, intensity);
            gfx_fill_rectangle(x, y, 5, 5);
        }
    }

    gfx_color(255, 255, 255);
    char info[100];
    sprintf(info, "Mode 10: Wave Interference - Time = %.2f", time);
    gfx_string(10, 30, info);
}

// Mode 11: Polar Graph
void draw_polar_graph(int width, int height, float phase) {
    const int num_points = 1000;
    for (int i = 0; i < num_points; i++) {
        float theta = (float)i / num_points * 2 * M_PI;
        float r = (width < height ? width : height) / 6 * sin(3 * theta + phase);
        int x = width / 2 + (int)(r * cos(theta));
        int y = height / 2 + (int)(r * sin(theta));
        int r_col, g, b;
        get_rainbow_color(theta + phase, &r_col, &g, &b);
        gfx_color(r_col, g, b);
        gfx_point(x, y);
    }
}

// Mode 12: Enhanced Lorenz Attractor - Dynamic chaos
void draw_enhanced_lorenz(int width, int height, float time) {
    static float x = 1.0, y = 1.0, z = 1.0;
    static float history_x[2000], history_y[2000], history_z[2000];
    static int history_index = 0;
    float sigma = 10.0, rho = 28.0, beta = 8.0 / 3.0;
    float dt = 0.01;

    for (int i = 0; i < height; i++) {
        float gradient_val = (float)i / height * 0.1;
        int bg_r, bg_g, bg_b;
        get_enhanced_color(time * 0.5 + gradient_val * 5, gradient_val, &bg_r, &bg_g, &bg_b);
        gfx_color(bg_r, bg_g, bg_b);
        gfx_line(0, i, width, i);
    }

    float dx = sigma * (y - x) * dt;
    float dy = (x * (rho - z) - y) * dt;
    float dz = (x * y - beta * z) * dt;
    x += dx; y += dy; z += dz;

    history_x[history_index] = x;
    history_y[history_index] = y;
    history_z[history_index] = z;
    history_index = (history_index + 1) % 2000;

    for (int i = 0; i < 2000; i++) {
        int index = (history_index - i + 2000) % 2000;
        float hx = history_x[index];
        float hy = history_y[index];
        float hz = history_z[index];

        int px = width / 2 + (int)(hx * width / 80);
        int py = height / 2 - (int)(hy * height / 80);

        int r, g, b;
        float color_phase = hz * 0.1 + time * 1.5 + (float)i / 200.0f;
        get_rainbow_color(color_phase, &r, &g, &b);

        int line_thickness = 3 - i / 500;
        if (line_thickness < 1) line_thickness = 1;

        gfx_color(r, g, b);
        if (i > 0) {
            int prev_index = (history_index - i + 2001) % 2000;
            int prev_px = width / 2 + (int)(history_x[prev_index] * width / 80);
            int prev_py = height / 2 - (int)(history_y[prev_index] * height / 80);
            for (int thickness_offset = 0; thickness_offset < line_thickness; thickness_offset++) {
                gfx_line(prev_px + thickness_offset, prev_py, px + thickness_offset, py);
                gfx_line(prev_px, prev_py + thickness_offset, px, py + thickness_offset);
            }
        }
    }

    gfx_color(255, 255, 255);
    char info[100];
    sprintf(info, "Mode 12: Enhanced Lorenz Attractor - Time = %.2f", time);
    gfx_string(10, 30, info);
}


// --- Digger and Cherry Functions ---
#define NUM_DIGGERS 5
#define NUM_CHERRIES 10

typedef struct { int x, y; int color_r, color_g, color_b; int direction; } Digger;
typedef struct { int x, y; int eaten; } Cherry;

Digger diggers[NUM_DIGGERS];
Cherry cherries[NUM_CHERRIES];

void init_diggers_and_cherries(int width, int height) {
    for (int i = 0; i < NUM_DIGGERS; i++) {
        diggers[i].x = rand() % width;
        diggers[i].y = rand() % height;
        get_rainbow_color((float)i / NUM_DIGGERS * 2 * M_PI, &diggers[i].color_r, &diggers[i].color_g, &diggers[i].color_b);
        diggers[i].direction = rand() % 4;
    }
    for (int i = 0; i < NUM_CHERRIES; i++) {
        cherries[i].x = rand() % width;
        cherries[i].y = rand() % height;
        cherries[i].eaten = 0;
    }
}

void update_diggers_and_cherries(int width, int height) {
    for (int i = 0; i < NUM_DIGGERS; i++) {
        // Движение диггеров
        int speed = 2;
        if (diggers[i].direction == 0) diggers[i].x += speed;
        else if (diggers[i].direction == 1) diggers[i].x -= speed;
        else if (diggers[i].direction == 2) diggers[i].y -= speed;
        else if (diggers[i].direction == 3) diggers[i].y += speed;

        // Смена направления у краев экрана
        if (diggers[i].x < 0) diggers[i].direction = 0;
        if (diggers[i].x > width) diggers[i].direction = 1;
        if (diggers[i].y < 0) diggers[i].direction = 3;
        if (diggers[i].y > height) diggers[i].direction = 2;

        // Поедание вишен
        for (int j = 0; j < NUM_CHERRIES; j++) {
            if (!cherries[j].eaten && abs(diggers[i].x - cherries[j].x) < 10 && abs(diggers[i].y - cherries[j].y) < 10) {
                cherries[j].eaten = 1;
                diggers[i].direction = rand() % 4; // Случайное изменение направления после еды
            }
        }
    }
}

void draw_diggers_and_cherries() {
    for (int i = 0; i < NUM_CHERRIES; i++) {
        if (!cherries[i].eaten) {
            gfx_color(255, 0, 0); // Красные вишни
            gfx_fill_circle(cherries[i].x, cherries[i].y, 5, 5);
        }
    }
    for (int i = 0; i < NUM_DIGGERS; i++) {
        gfx_color(diggers[i].color_r, diggers[i].color_g, diggers[i].color_b);
        gfx_fill_rectangle(diggers[i].x - 8, diggers[i].y - 5, 16, 10); // Тело диггера - прямоугольник
        gfx_fill_circle(diggers[i].x - 5, diggers[i].y + 5, 3, 3); // Ноги - круги
        gfx_fill_circle(diggers[i].x + 5, diggers[i].y + 5, 3, 3);
    }
}


int main() {
    // Получение размеров экрана через X11
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }
    int screen = DefaultScreen(display);
    WINDOW_WIDTH = DisplayWidth(display, screen);   // Initialize global WINDOW_WIDTH
    WINDOW_HEIGHT = DisplayHeight(display, screen); // Initialize global WINDOW_HEIGHT
    XCloseDisplay(display);


    gfx_open(WINDOW_WIDTH, WINDOW_HEIGHT, "- Unleashed GFX Demo - ");
    gfx_clear_color(0, 0, 0);
    gfx_clear();
    srand(time(NULL));

    int demo_mode = -1;
    float color_phase = 0;
    int line_count = 100;
    int circle_count = 50;
    int rect_count = 30;
    char message[] = "Unleashed GFX Library Demo!";
    int message_scroll_x = WINDOW_WIDTH;
    int wobble_offset = 0, wobble_direction = 1;
    double zoom = 1.0, offset_x = -0.5, offset_y = 0.0;
    Pixel *pixels = (Pixel *)malloc((WINDOW_WIDTH / 2) * (WINDOW_HEIGHT / 2) * sizeof(Pixel));
    char input_text[100] = "";

    // --- NEW CODE - variables for Mode 2 ---
    float phase = 0;
    int pulse_trigger = 0;
    // --- END NEW CODE ---


    char *mode_descriptions[] = {
        "Mode 0: Hypnotic Swirling Lines - Multi-layered swirling lines",
        "Mode 1: Expanding Circles - Concentric circles pulsing",
        "Mode 2: Enhanced Pixel Sampler - Pulse effect, RGB readout", 
        "Mode 3: Moving Rectangles - Bouncing rectangles",
        "Mode 4: Dynamic Text Rhyme - English rhyme with wave motion",
        "Mode 5: Dynamic 3D Pulse - Pulsating Depth",
        "Mode 6: Cosmic Voyage - Speeding through the Universe",
        "Mode 7: Interactive Input Visualizer - Circles and lines",
        "Mode 8: Mandelbrot Set - Fractal with zoom/pan",
        "Mode 9: Lissajous Curves - Parametric curves",
        "Mode 10: Wave Interference - Superposition of waves",
        "Mode 11: Polar Graph - 3-petal rose",
        "Mode 12: Enhanced Lorenz Attractor - Dynamic chaos"
    };

    // Initial screen
    init_diggers_and_cherries(WINDOW_WIDTH, WINDOW_HEIGHT);
    while (demo_mode == -1) {
        gfx_clear();
        draw_diggers_and_cherries();

        gfx_color(255, 255, 255);
        gfx_string(10, 10, "- Unleashed GFX Demo -");
        char res[50];
        sprintf(res, "Resolution: %d x %d", WINDOW_WIDTH, WINDOW_HEIGHT);
        gfx_string(10, 40, res);
        gfx_string(10, 70, "Press 'n' to start, 'q' to quit");
        gfx_string(10, 100, "Modes (0-12):");
        for (int i = 0; i < 13; i++) {
            gfx_string(10, 130 + i * 30, mode_descriptions[i]);
        }
        gfx_string(10, 130 + 13 * 30, "Controls: 'n' next, 'p' prev, 'c' clear, 'q' quit");
        gfx_string(10, 130 + 14 * 30, "Mandelbrot: +/- zoom, a/d/w/s pan");
        gfx_flush();
        usleep(20000);
        update_diggers_and_cherries(WINDOW_WIDTH, WINDOW_HEIGHT);
        if (gfx_event_waiting()) {
            char key = gfx_wait();
            if (key == 'q') { free(pixels); return 0; }
            if (key == 'n') demo_mode = 0;
        }
    }

    if (demo_mode == 8) init_mandelbrot(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, zoom, offset_x, offset_y);

    while (1) {
        if (demo_mode != 8 && demo_mode != 10) gfx_clear();
        int r, g, b;
        color_phase += 0.02;
        if (color_phase > 2 * M_PI) color_phase -= 2 * M_PI;

        gfx_color(255, 255, 255);
        gfx_string(10, 10, mode_descriptions[demo_mode]);

        switch (demo_mode) {
            case 0: draw_swirling_lines(WINDOW_WIDTH, WINDOW_HEIGHT, color_phase); break;
            case 1: draw_expanding_circles(WINDOW_WIDTH, WINDOW_HEIGHT, color_phase, circle_count); break;
            case 2: draw_pixel_sampler(WINDOW_WIDTH, WINDOW_HEIGHT, color_phase + pulse_trigger); break; // Updated Mode 2
            case 3: draw_moving_rectangles(WINDOW_WIDTH, WINDOW_HEIGHT, color_phase, rect_count); break;
            case 4: draw_text_scroller(WINDOW_WIDTH, WINDOW_HEIGHT, color_phase, message_scroll_x); break;
            case 5: draw_dynamic_pulse_3d(WINDOW_WIDTH, WINDOW_HEIGHT, color_phase); break;
            case 6: draw_mixed_graphics(WINDOW_WIDTH, WINDOW_HEIGHT, color_phase); break;
            case 7: draw_input_visualizer(WINDOW_WIDTH, WINDOW_HEIGHT, input_text); break;
            case 8: update_mandelbrot(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, color_phase, zoom, offset_x, offset_y); break;
            case 9: draw_lissajous(WINDOW_WIDTH, WINDOW_HEIGHT, color_phase); break;
            case 10: draw_wave_interference(WINDOW_WIDTH, WINDOW_HEIGHT, color_phase); break;
            case 11: draw_polar_graph(WINDOW_WIDTH, WINDOW_HEIGHT, color_phase); break;
            case 12: draw_enhanced_lorenz(WINDOW_WIDTH, WINDOW_HEIGHT, color_phase); break;
            default: demo_mode = 0; break;
        }

        if (demo_mode == 4) {
            message_scroll_x -= 2;
            if (message_scroll_x < -WINDOW_WIDTH) message_scroll_x = WINDOW_WIDTH;
        } else {
            message_scroll_x = WINDOW_WIDTH;
        }

        gfx_flush();
        usleep(20000);

        if (gfx_event_waiting()) {
            char key = gfx_wait();
            if (key == 'q') { free(pixels); break; }
            if (key == 'n') {
                demo_mode = (demo_mode + 1) % 13;
                if (demo_mode == 8) init_mandelbrot(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, zoom, offset_x, offset_y);
                strcpy(input_text, "");
            }
            if (key == 'p') {
                demo_mode = (demo_mode - 1 + 13) % 13;
                if (demo_mode == 8) init_mandelbrot(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, zoom, offset_x, offset_y);
                strcpy(input_text, "");
            }
            if (key == 'c') gfx_clear();
            if (demo_mode == 8) {
                if (key == '+' || key == '=') { zoom *= 1.2; init_mandelbrot(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, zoom, offset_x, offset_y); }
                if (key == '-') { zoom /= 1.2; if (zoom < 0.1) zoom = 0.1; init_mandelbrot(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, zoom, offset_x, offset_y); }
                if (key == 'a') { offset_x -= 0.1 / zoom; init_mandelbrot(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, zoom, offset_x, offset_y); }
                if (key == 'd') { offset_x += 0.1 / zoom; init_mandelbrot(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, zoom, offset_x, offset_y); }
                if (key == 'w') { offset_y -= 0.1 / zoom; init_mandelbrot(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, zoom, offset_y, offset_y); }
                if (key == 's') { offset_y += 0.1 / zoom; init_mandelbrot(pixels, WINDOW_WIDTH, WINDOW_HEIGHT, zoom, offset_x, offset_y); }
            }
            // --- NEW CODE - Mouse click event for Mode 2 ---
            if (demo_mode == 2) {
                if (key == 1) { // Mouse button 1 (left click)
                    pulse_trigger = 2;
                }
            }
            // --- END NEW CODE ---
              if (key > 31 && key < 127) {
                sprintf(input_text, "Key: '%c' (ASCII: %d), Mouse: (%d, %d)", key, key, gfx_xpos(), gfx_ypos());
            } else if (key >= 1 && key <= 3) {
                sprintf(input_text, "Mouse Button %d at (%d, %d)", key, key, gfx_xpos(), gfx_ypos());
            } else {
                sprintf(input_text, "Key Code: %d, Mouse: (%d, %d)", key, gfx_xpos(), gfx_ypos());
            }
            printf("Demo mode: %d\n", demo_mode);
        }

        // --- NEW CODE - Phase and pulse_trigger update ---
        phase += 0.05;
        if (pulse_trigger > 0) pulse_trigger -= 0.1;
        if (phase > 2 * M_PI) phase -= 2 * M_PI;
        // --- END NEW CODE ---
    }

    free(pixels);
    return 0;
}
