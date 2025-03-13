// gcc -o demo_alpha_channel_double_buffer demo_alpha_channel_double_buffer.c gfx.c -lX11 -lm -O3 -march=native
// 
// 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <X11/Xlib.h>
#include "gfx.h"

int WINDOW_WIDTH = 1280;
int WINDOW_HEIGHT = 800;

void get_rainbow_color_alpha(float phase, float alpha_factor, int *r, int *g, int *b, int *a) {
    float red = sin(phase) * 0.5 + 0.5;
    float green = sin(phase + 2.094) * 0.5 + 0.5;
    float blue = sin(phase + 4.188) * 0.5 + 0.5;

    *r = (int)(red * 255);
    *g = (int)(green * 255);
    *b = (int)(blue * 255);
    *a = (int)(alpha_factor * 255);
}

// Оптимизированная функция для отрисовки сцены с альфа-каналом
void draw_optimized_alpha_scene(int width, int height, float time) {
    int step = 16; // Большой шаг для скорости

    // Отрисовка фона с альфа-каналом
    for (int y = 0; y < height; y += step) {
        for (int x = 0; x < width; x += step) {
            float phase = time + (x + y) * 0.01f;
            float alpha_factor = 0.5f + 0.5f * sin(phase);
            int r, g, b, a;
            get_rainbow_color_alpha(phase, alpha_factor * 0.8f, &r, &g, &b, &a);
            gfx_double_buffer_fill_rectangle(x, y, step, step, r, g, b, a);
        }
    }

    // Отрисовка круга с альфа-каналом
    int x = width / 2 + (int)(cos(time) * width * 0.2f);
    int y = height / 2 + (int)(sin(time) * height * 0.2f);
    int radius = 30;
    float alpha_factor = 0.5f + 0.5f * cos(time);
    int r, g, b, a;
    get_rainbow_color_alpha(time, alpha_factor * 0.8f, &r, &g, &b, &a);
    gfx_double_buffer_fill_circle(x, y, radius, r, g, b, a);
}

// Оптимизированная функция для пульсирующих волн с альфа-каналом
void draw_optimized_pulsating_waves(int width, int height, float time) {
    int step = 20; // Большой шаг для скорости

    // Отрисовка фона с альфа-каналом
    for (int y = 0; y < height; y += step) {
        for (int x = 0; x < width; x += step) {
            float distance = fabs((x - width / 2) + (y - height / 2)) * 0.005f;
            float phase = time + distance;
            float alpha_factor = 0.4f + 0.6f * sin(phase);
            int r, g, b, a;
            get_rainbow_color_alpha(phase, alpha_factor * 0.8f, &r, &g, &b, &a);
            gfx_double_buffer_fill_rectangle(x, y, step, step, r, g, b, a);
        }
    }

    // Отрисовка круга с альфа-каналом
    int x = width / 2 + (int)(cos(time * 1.5f) * width * 0.15f);
    int y = height / 2 + (int)(sin(time * 1.5f) * height * 0.15f);
    int radius = 25;
    float alpha_factor = 0.5f + 0.5f * sin(time * 2.0f);
    int r, g, b, a;
    get_rainbow_color_alpha(time * 2.0f, alpha_factor * 0.8f, &r, &g, &b, &a);
    gfx_double_buffer_fill_circle(x, y, radius, r, g, b, a);
}

// Третья демонстрация: плазменный шум с альфа-каналом
void draw_plasma_noise(int width, int height, float time) {
    int step = 10; // Меньший шаг для более детализированного эффекта

    for (int y = 0; y < height; y += step) {
        for (int x = 0; x < width; x += step) {
            // Генерация плазменного шума на основе синусоидальных функций
            float noise = sin(x * 0.01f + time) + sin(y * 0.01f + time) + sin((x + y) * 0.01f + time);
            float alpha_factor = 0.5f + 0.5f * sin(noise + time);

            int r, g, b, a;
            get_rainbow_color_alpha(noise + time, alpha_factor, &r, &g, &b, &a);

            // Рисуем прямоугольник с текущим цветом и альфа-каналом
            gfx_double_buffer_fill_rectangle(x, y, step, step, r, g, b, a);
        }
    }
}

// Четвертая демонстрация: прозрачные слои
void draw_transparent_layers(int width, int height, float time) {
    // Фоновый слой (светлый градиент)
    for (int y = 0; y < height; y++) {
        int r = 255;
        int g = 255;
        int b = 255 - (int)((float)y / height * 100); // Светлый градиент
        int a = 255; // Полностью непрозрачный
        gfx_double_buffer_fill_rectangle(0, y, width, 1, r, g, b, a);
    }

    // Первый полупрозрачный слой (круг)
    int x1 = width / 4 + (int)(cos(time) * 100);
    int y1 = height / 2 + (int)(sin(time) * 100);
    int radius1 = 100;
    gfx_double_buffer_fill_circle(x1, y1, radius1, 200, 200, 255, 128); // Полупрозрачный голубой

    // Второй полупрозрачный слой (прямоугольник)
    int x2 = width / 2 + (int)(sin(time) * 100);
    int y2 = height / 4 + (int)(cos(time) * 100);
    gfx_double_buffer_fill_rectangle(x2, y2, 200, 200, 255, 200, 200, 128); // Полупрозрачный розовый
}

// Пятая демонстрация: эффект "стекла" или "воды"
void draw_glass_effect(int width, int height, float time) {
    // Фоновый слой (светлый градиент)
    for (int y = 0; y < height; y++) {
        int r = 255;
        int g = 255;
        int b = 255 - (int)((float)y / height * 100); // Светлый градиент
        int a = 255; // Полностью непрозрачный
        gfx_double_buffer_fill_rectangle(0, y, width, 1, r, g, b, a);
    }

    // Эффект "стекла" (полупрозрачный прямоугольник с волнами)
    for (int y = 0; y < height; y += 10) {
        for (int x = 0; x < width; x += 10) {
            float wave = sin(x * 0.05f + time) * 10 + sin(y * 0.05f + time) * 10;
            int a = 128 + (int)(sin(wave) * 50); // Прозрачность с волнами
            gfx_double_buffer_fill_rectangle(x, y, 10, 10, 200, 230, 255, a); // Светло-голубой
        }
    }
}

// Четвертая демонстрация: пересекающиеся и трансформирующиеся фигуры
void draw_transforming_shapes(int width, int height, float time) {
    // Фоновый слой (светлый градиент)
    for (int y = 0; y < height; y++) {
        int r = 255;
        int g = 255;
        int b = 255 - (int)((float)y / height * 100); // Светлый градиент
        int a = 255; // Полностью непрозрачный
        gfx_double_buffer_fill_rectangle(0, y, width, 1, r, g, b, a);
    }

    // Первая фигура: круг, который трансформируется в квадрат
    int x1 = width / 4 + (int)(cos(time) * 100);
    int y1 = height / 2 + (int)(sin(time) * 100);
    int radius1 = 100 + (int)(sin(time * 2) * 50); // Радиус изменяется со временем
    int size1 = 100 + (int)(sin(time * 2) * 50);  // Размер квадрата изменяется со временем

    // Рисуем круг или квадрат в зависимости от времени
    if (sin(time * 2) > 0) {
        gfx_double_buffer_fill_circle(x1, y1, radius1, 200, 200, 255, 128); // Полупрозрачный голубой круг
    } else {
        gfx_double_buffer_fill_rectangle(x1 - size1 / 2, y1 - size1 / 2, size1, size1, 200, 200, 255, 128); // Полупрозрачный голубой квадрат
    }

    // Вторая фигура: треугольник, который трансформируется в прямоугольник
    int x2 = width / 2 + (int)(sin(time) * 100);
    int y2 = height / 4 + (int)(cos(time) * 100);
    int width2 = 150 + (int)(sin(time * 1.5) * 50);  // Ширина прямоугольника изменяется со временем
    int height2 = 100 + (int)(cos(time * 1.5) * 50); // Высота прямоугольника изменяется со временем

    // Рисуем треугольник или прямоугольник в зависимости от времени
    if (cos(time * 1.5) > 0) {
        // Рисуем треугольник
        int x_points[] = {x2, x2 + width2 / 2, x2 - width2 / 2};
        int y_points[] = {y2 - height2 / 2, y2 + height2 / 2, y2 + height2 / 2};
        gfx_double_buffer_fill_polygon(x_points, y_points, 3, 255, 200, 200, 128); // Полупрозрачный розовый треугольник
    } else {
        gfx_double_buffer_fill_rectangle(x2 - width2 / 2, y2 - height2 / 2, width2, height2, 255, 200, 200, 128); // Полупрозрачный розовый прямоугольник
    }

    // Третья фигура: эллипс, который трансформируется в звезду
    int x3 = width * 3 / 4 + (int)(cos(time * 0.8) * 100);
    int y3 = height * 3 / 4 + (int)(sin(time * 0.8) * 100);
    int radius_x3 = 80 + (int)(sin(time * 1.2) * 40); // Горизонтальный радиус эллипса
    int radius_y3 = 120 + (int)(cos(time * 1.2) * 40); // Вертикальный радиус эллипса

    // Рисуем эллипс или звезду в зависимости от времени
    if (sin(time * 1.2) > 0) {
        gfx_double_buffer_fill_ellipse(x3, y3, radius_x3, radius_y3, 200, 255, 200, 128); // Полупрозрачный зелёный эллипс
    } else {
        // Рисуем звезду
        int star_points = 5;
        int star_radius1 = 80;
        int star_radius2 = 40;
        int x_points[10], y_points[10];
        for (int i = 0; i < star_points * 2; i++) {
            float angle = i * M_PI / star_points;
            int radius = (i % 2 == 0) ? star_radius1 : star_radius2;
            x_points[i] = x3 + (int)(cos(angle) * radius);
            y_points[i] = y3 + (int)(sin(angle) * radius);
        }
        gfx_double_buffer_fill_polygon(x_points, y_points, star_points * 2, 200, 255, 200, 128); // Полупрозрачная зелёная звезда
    }
}


int main() {
    gfx_open(WINDOW_WIDTH, WINDOW_HEIGHT, "Alpha Effects Demo");
    gfx_double_buffer_init();
    float time = 0;
    int effect_mode = 0;

    while (1) {
        gfx_double_buffer_clear(0, 0, 0);

        if (effect_mode == 0) {
            draw_optimized_alpha_scene(WINDOW_WIDTH, WINDOW_HEIGHT, time);
            gfx_set_title("6. Optimized Infinite Math Function - Flying & Looping");
        } else if (effect_mode == 1) {
            draw_optimized_pulsating_waves(WINDOW_WIDTH, WINDOW_HEIGHT, time);
            gfx_set_title("7. Optimized Pulsating Alpha Waves");
        } else if (effect_mode == 2) {
            draw_plasma_noise(WINDOW_WIDTH, WINDOW_HEIGHT, time);
            gfx_set_title("8. Plasma Noise with Alpha Channel");
        } else if (effect_mode == 3) {
            draw_transparent_layers(WINDOW_WIDTH, WINDOW_HEIGHT, time);
            gfx_set_title("9. Transparent Layers");
        } else if (effect_mode == 4) {
            draw_glass_effect(WINDOW_WIDTH, WINDOW_HEIGHT, time);
            gfx_set_title("10. Glass/Water Effect");
        } else if (effect_mode == 5) {
            draw_transforming_shapes(WINDOW_WIDTH, WINDOW_HEIGHT, time);
            gfx_set_title("11. Transparent Shapes");
        }





        gfx_double_buffer_swap();
        usleep(16666); // ~60 FPS
        time += 0.05; // Ускоряем анимацию для заметности
        if (time > 2 * M_PI * 10) time -= 2 * M_PI * 10;

        if (gfx_event_waiting()) {
            char key = gfx_wait();
            if (key == 'q') break;
            if (key == 'n') effect_mode = (effect_mode + 1) % 6; // Переключение между пятью эффектами
            if (key == 'p') effect_mode = (effect_mode - 1 + 6) % 6;
        }
    }

    gfx_double_buffer_cleanup();
    return 0;
}
