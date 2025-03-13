// gcc -o unleashed_demo_alpha unleashed_demo_alpha.c gfx.c -lX11 -lm



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "gfx.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Function to get a rainbow color from a spectrum with alpha
void get_rainbow_color_alpha(float phase, float alpha_factor, int *r, int *g, int *b, int *a) {
    float red = sin(phase + 0) * 0.5 + 0.5;
    float green = sin(phase + 2.094) * 0.5 + 0.5; // 2.094 ~= 2pi/3
    float blue = sin(phase + 4.188) * 0.5 + 0.5; // 4.188 ~= 4pi/3

    *r = (int)(red * 255);
    *g = (int)(green * 255);
    *b = (int)(blue * 255);
    *a = (int)(alpha_factor * 255); // Set alpha here
}

int main() {
    gfx_open(WINDOW_WIDTH, WINDOW_HEIGHT, "Unleashed GFX Demo with Alpha"); // Updated window title
    gfx_clear_color(0, 0, 0);
    gfx_clear();
    srand(time(NULL));

    int demo_mode = 0;
    float color_phase = 0;
    int line_count = 100;
    int circle_count = 50;
    int rect_count = 30;
    char message[] = "Unleashed GFX Library Demo with Alpha!"; // Updated message
    int message_scroll_x = WINDOW_WIDTH;
    int message_scroll_y = 30;
    int wobble_offset = 0;
    int wobble_direction = 1;

    while (1) {
        gfx_clear();
        int r, g, b, a; // Added alpha 'a'

        color_phase += 0.02;
        if (color_phase > 2 * M_PI) color_phase -= 2 * M_PI;

        switch (demo_mode) {
            case 0: // Swirling Lines with Alpha
                for (int i = 0; i < line_count; i++) {
                    float angle = (float)i / line_count * 2 * M_PI + color_phase;
                    int x1 = WINDOW_WIDTH / 2 + (int)(cos(angle) * 150);
                    int y1 = WINDOW_HEIGHT / 2 + (int)(sin(angle) * 150);
                    int x2 = WINDOW_WIDTH / 2 + (int)(cos(angle + color_phase * 2) * 200);
                    int y2 = WINDOW_HEIGHT / 2 + (int)(sin(angle + color_phase * 2) * 200);
                    get_rainbow_color_alpha(angle + color_phase, 0.7f, &r, &g, &b, &a); // Added alpha factor
                    gfx_color_alpha(r, g, b, a); // Using gfx_color_alpha
                    gfx_line(x1, y1, x2, y2);
                }
                gfx_color(255, 255, 255); // White for text
                gfx_string(10, 10, "Mode 0: Swirling Lines (Alpha)"); // Updated text
                break;

            case 1: // Expanding Circles with Alpha
                for (int i = 0; i < circle_count; i++) {
                    float radius = (float)i / circle_count * 150 + (sin(color_phase) * 30);
                    get_rainbow_color_alpha(color_phase + (float)i/circle_count * M_PI, 0.5f, &r, &g, &b, &a); // Added alpha factor
                    gfx_color_alpha(r, g, b, a); // Using gfx_color_alpha
                    gfx_circle(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, (int)radius, (int)radius);
                }
                gfx_color(255, 255, 255); // White for text
                gfx_string(10, 10, "Mode 1: Expanding Circles (Alpha)"); // Updated text
                break;

            case 2: // Pixel Sampler with Alpha - display color under mouse
                gfx_color(255, 255, 255); // White for text
                gfx_string(10, 10, "Mode 2: Pixel Sampler (Alpha) - Move mouse over window"); // Updated text
                gfx_string(10, 30, "Press mouse button to change color");
                for (int x = 0; x < WINDOW_WIDTH; x += 10) {
                    for (int y = 0; y < WINDOW_HEIGHT; y += 10) {
                        get_rainbow_color_alpha((float)x/WINDOW_WIDTH * 2 * M_PI + (float)y/WINDOW_HEIGHT * 2 * M_PI + color_phase, 0.3f, &r, &g, &b, &a); // Added alpha factor
                        gfx_color_alpha(r, g, b, a); // Using gfx_color_alpha
                        gfx_point(x, y);
                    }
                }
                int mouse_x = gfx_xpos();
                int mouse_y = gfx_ypos();
                int pixel_color = GetPix(mouse_x, mouse_y);
                int pr = (pixel_color >> 16) & 0xFF;
                int pg = (pixel_color >> 8) & 0xFF;
                int pb = pixel_color & 0xFF;
                char color_text[50];
                sprintf(color_text, "RGB at (%d, %d): R=%d, G=%d, B=%d", mouse_x, mouse_y, pr, pg, pb);
                gfx_color(255, 255, 255); // White for text
                gfx_string(10, 50, color_text);
                break;

            case 3: // Moving Rectangles with Alpha
                gfx_color(255, 255, 255); // White for text
                gfx_string(10, 10, "Mode 3: Moving Rectangles (Alpha)"); // Updated text
                for (int i = 0; i < rect_count; i++) {
                    static int rect_positions[30][2];
                    static int rect_velocities[30][2];
                    if (rect_positions[i][0] == 0 && rect_positions[i][1] == 0) {
                        rect_positions[i][0] = rand() % WINDOW_WIDTH;
                        rect_positions[i][1] = rand() % WINDOW_HEIGHT;
                        rect_velocities[i][0] = (rand() % 5) - 2;
                        rect_velocities[i][1] = (rand() % 5) - 2;
                         if (rect_velocities[i][0] == 0 && rect_velocities[i][1] == 0) {
                            rect_velocities[i][0] = 1;
                        }
                    }
                    rect_positions[i][0] += rect_velocities[i][0];
                    rect_positions[i][1] += rect_velocities[i][1];
                    if (rect_positions[i][0] < 0 || rect_positions[i][0] > WINDOW_WIDTH - 40) rect_velocities[i][0] = -rect_velocities[i][0];
                    if (rect_positions[i][1] < 0 || rect_positions[i][1] > WINDOW_HEIGHT - 30) rect_velocities[i][1] = -rect_velocities[i][1];

                    get_rainbow_color_alpha(color_phase + (float)i/rect_count * M_PI * 2, 0.6f, &r, &g, &b, &a); // Added alpha factor
                    gfx_color_alpha(r, g, b, a); // Using gfx_color_alpha
                    gfx_fill_rectangle(rect_positions[i][0], rect_positions[i][1], 40, 30);
                }
                break;

            case 4: // Text Scroller with Alpha Trail
                gfx_color(255, 255, 255); // White for text
                gfx_string(10, 10, "Mode 4: Text Scroller (Alpha Trail)"); // Updated text
                // Text Scroller now with alpha trail effect
                for (int i = 0; i < strlen(message); i++) {
                    int x = message_scroll_x + i * 8;
                    float alpha_factor = 1.0f - (float)i / strlen(message) * 0.8f; // Alpha trail
                    get_rainbow_color_alpha(color_phase + (float)i / strlen(message) * M_PI, alpha_factor, &r, &g, &b, &a);
                    gfx_color_alpha(r, g, b, a);
                    gfx_string(x, message_scroll_y, &message[i]);
                }

                message_scroll_x -= 3;
                if (message_scroll_x < -strlen(message) * 8) message_scroll_x = WINDOW_WIDTH; // Assuming ~8 pixels per char width
                break;

            case 5: // Window Wobble with Alpha Circles
                gfx_color(255, 255, 255); // White for text
                gfx_string(10, 10, "Mode 5: Window Wobble (Alpha Circles)"); // Updated text
                for (int i = 0; i < 20; i++) {
                    get_rainbow_color_alpha(color_phase + (float)i/20 * M_PI * 2, 0.4f, &r, &g, &b, &a); // Added alpha factor
                    gfx_color_alpha(r, g, b, a); // Using gfx_color_alpha
                    gfx_fill_circle(WINDOW_WIDTH / 2 + (int)(sin((float)i/20 * 2 * M_PI + color_phase) * 50), WINDOW_HEIGHT / 2, 20, 20);
                }
                wobble_offset += wobble_direction;
                if (abs(wobble_offset) > 10) wobble_direction = -wobble_direction;
                gfx_move_win_r(0, wobble_offset, 0, 0, 0); // Wobble horizontally
                break;

            case 6: // Mixed Graphics with Alpha - All shapes together
                gfx_color(255, 255, 255); // White for text
                gfx_string(10, 10, "Mode 6: Mixed Graphics (Alpha)"); // Updated text
                get_rainbow_color_alpha(color_phase, 0.7f, &r, &g, &b, &a); gfx_color_alpha(r,g,b, a); // Added alpha factor & Using gfx_color_alpha
                gfx_fill_rectangle(50, WINDOW_HEIGHT - 80, 100, 50);
                get_rainbow_color_alpha(color_phase + M_PI/2, 0.7f, &r, &g, &b, &a); gfx_color_alpha(r,g,b, a); // Added alpha factor & Using gfx_color_alpha
                gfx_circle(WINDOW_WIDTH - 100, 100, 60, 60);
                get_rainbow_color_alpha(color_phase + M_PI, 0.7f, &r, &g, &b, &a); gfx_color_alpha(r,g,b, a); // Added alpha factor & Using gfx_color_alpha
                gfx_line(200, 200, WINDOW_WIDTH - 200, WINDOW_HEIGHT - 200);
                get_rainbow_color_alpha(color_phase + 3*M_PI/2, 0.7f, &r, &g, &b, &a); gfx_color_alpha(r,g,b, a); // Added alpha factor & Using gfx_color_alpha
                for (int i = 0; i < 50; ++i) {
                    get_rainbow_color_alpha(color_phase + (float)i/50 * M_PI, 0.8f, &r, &g, &b, &a); // Alpha for points
                    gfx_color_alpha(r, g, b, a);
                    gfx_point(rand() % WINDOW_WIDTH, rand() % WINDOW_HEIGHT);
                }
                gfx_color(255,255,255); gfx_string(WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT/2, "GFX Mashup Alpha!"); // Updated text
                break;

            case 7: // Key and Mouse Input Display with Alpha Background
                // Semi-transparent background for text
                get_rainbow_color_alpha(color_phase * 2, 0.1f, &r, &g, &b, &a); // Soft background color
                gfx_color_alpha(r, g, b, a);
                gfx_fill_rectangle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

                gfx_color(255, 255, 255); // White for text
                gfx_string(10, 10, "Mode 7: Input Display (Alpha BG) - Press keys, click mouse"); // Updated text
                gfx_string(10, 30, "Last key/button event:");
                if (gfx_event_waiting()) {
                    char event_char = gfx_wait();
                    char input_text[100];
                    if (event_char > 31 && event_char < 127) { // Printable character
                        sprintf(input_text, "Key pressed: '%c' (ASCII: %d), Mouse at (%d, %d)", event_char, event_char, gfx_xpos(), gfx_ypos());
                    } else if (event_char >= 1 && event_char <= 3) { // Mouse buttons
                        sprintf(input_text, "Mouse button %d pressed at (%d, %d)", event_char, gfx_xpos(), gfx_ypos());
                        if (demo_mode == 2) { // Special action in pixel sampler mode
                            color_phase += 0.5; // Change color phase on click in mode 2
                        }
                    } else { // Non-printable key
                        sprintf(input_text, "Key code: %d, Mouse at (%d, %d)", event_char, gfx_xpos(), gfx_ypos());
                    }
                    gfx_color(255, 255, 255);
                    gfx_string(10, 50, input_text);
                }
                break;


            default:
                demo_mode = 0;
                break;
        }

        gfx_flush();
        usleep(20000);

        if (gfx_event_waiting()) {
            char key = gfx_wait();
            if (key == 'q') break;
            if (key == ' ') {
                demo_mode = (demo_mode + 1) % 8; // Cycle through 8 modes now
                printf("Demo mode: %d\n", demo_mode);
                if (demo_mode == 5) wobble_offset = 0; // Reset wobble offset when entering wobble mode
            }
            if (key == 'c') {
                gfx_clear();
            }
        }
    }

    return 0;
}
