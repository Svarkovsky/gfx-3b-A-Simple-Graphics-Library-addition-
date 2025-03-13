/*

		-	gfx_demonstrations	-

*/


/*

gcc -o gfx_demonstrations gfx_demonstrations.c gfx.c -lX11 -lm -O3 -march=native -mtune=native

Or like this, to be more specific:
gcc -std=gnu11 -o gfx_demonstrations gfx_demonstrations.c gfx.c -lX11 -lm -O3 -march=native -mtune=native \
-msse3 -mssse3 -fno-exceptions -fomit-frame-pointer -flto -fvisibility=hidden -mfpmath=sse -ffast-math -pipe \
-s -ffunction-sections -fdata-sections -Wl,--gc-sections -fno-asynchronous-unwind-tables -Wl,--strip-all -DNDEBUG

*/

/*
MIT License

Copyright (c) Ivan Svarkovsky - 2025

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "gfx.h" // Подключение заголовочного файла gfx.h, содержащего объявления функций для графической библиотеки
#include <stdio.h>   // Подключение стандартной библиотеки ввода/вывода
#include <stdlib.h>  // Подключение стандартной библиотеки общего назначения (для функций, таких как exit, rand и т.д.)
#include <unistd.h>  // Подключение библиотеки POSIX API (для функций, таких как usleep)

#ifndef usleep
extern int usleep(unsigned int usec);
#endif

#include <time.h>    // Подключение библиотеки времени (для функций, связанных со временем, например, srand(time(NULL)))

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <math.h>    // Подключение математической библиотеки (для математических функций, таких как sin, cos, M_PI)
#include <string.h>  // Подключение библиотеки для работы со строками (для функций, таких как strlen, sprintf)

// Define window dimensions (16:9 aspect ratio)
#define WINDOW_WIDTH 1280  // Ширина окна в пикселях
#define WINDOW_HEIGHT 720 // Высота окна в пикселях

// Define text colors
#define TEXT_FG_COLOR_R 255 // Красная составляющая цвета текста (передний план)
#define TEXT_FG_COLOR_G 255 // Зеленая составляющая цвета текста (передний план)
#define TEXT_FG_COLOR_B 255 // Синяя составляющая цвета текста (передний план)

#define TEXT_BG_COLOR_R 0   // Красная составляющая цвета фона текста
#define TEXT_BG_COLOR_G 0   // Зеленая составляющая цвета фона текста
#define TEXT_BG_COLOR_B 100  // Синяя составляющая цвета фона текста

// Define text padding and line height
#define TEXT_PADDING_X 5  // Горизонтальный отступ текста от фона
#define TEXT_PADDING_Y 5  // Вертикальный отступ текста от фона
#define LINE_HEIGHT 15    // Высота строки текста

// Define MIN macro for standard C compatibility (if not already defined elsewhere)
#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b)) // Макрос MIN, возвращает меньшее из двух значений
#endif

extern void (*current_demo_function)(void); // External variable from gfx.c
// Объявление внешней переменной `current_demo_function`, которая является указателем на функцию, не принимающую аргументов и возвращающую void.
// Определена в gfx.c и используется для выбора текущей демонстрационной функции.

// Canvas parameters
int canvas_x, canvas_y, canvas_width, canvas_height;
// Переменные для хранения параметров канвы (области рисования) внутри окна:
// canvas_x, canvas_y - координаты верхнего левого угла канвы
// canvas_width, canvas_height - ширина и высота канвы

// Функция для получения радужных цветов с альфа-каналом
void get_rainbow_color_alpha(float t, float alpha_factor, int *r, int *g, int *b, int *a) {
    // Функция генерирует радужный цвет на основе времени `t` и фактора прозрачности `alpha_factor`.
    // Результат записывается в переданные указатели r, g, b, a (красный, зеленый, синий, альфа компоненты).
    *r = (int)(sin(t) * 127 + 128);               // Красный: синусоидальная функция времени для создания циклического изменения цвета
    *g = (int)(sin(t + 2 * M_PI / 3) * 127 + 128); // Зелёный: сдвиг фазы синусоиды на 120 градусов для создания радужного эффекта
    *b = (int)(sin(t + 4 * M_PI / 3) * 127 + 128); // Синий: сдвиг фазы синусоиды на 240 градусов
    *a = (int)(alpha_factor * 255);               // Альфа: прозрачность, определяется `alpha_factor` (от 0.0 до 1.0) и масштабируется до диапазона 0-255
}

// Function to initialize the canvas
void init_canvas() {
    // Функция инициализирует параметры канвы, определяя ее положение и размер внутри окна.
    float left_margin_percent = 0.30f;  // Процент левого отступа канвы от ширины окна
    float top_margin_percent = 0.20f;   // Процент верхнего отступа канвы от высоты окна
    float right_margin_percent = 0.04f;  // Процент правого отступа канвы от ширины окна
    float bottom_margin_percent = 0.03f; // Процент нижнего отступа канвы от высоты окна

    int left_margin = (int)(WINDOW_WIDTH * left_margin_percent);   // Вычисление левого отступа в пикселях
    int top_margin = (int)(WINDOW_HEIGHT * top_margin_percent);    // Вычисление верхнего отступа в пикселях
    int right_margin = (int)(WINDOW_WIDTH * right_margin_percent);  // Вычисление правого отступа в пикселях
    int bottom_margin = (int)(WINDOW_HEIGHT * bottom_margin_percent); // Вычисление нижнего отступа в пикселях

    canvas_x = left_margin;      // Установка X-координаты канвы
    canvas_y = top_margin;       // Установка Y-координаты канвы
    canvas_width = WINDOW_WIDTH - left_margin - right_margin;   // Вычисление ширины канвы
    canvas_height = WINDOW_HEIGHT - top_margin - bottom_margin; // Вычисление высоты канвы

    if (canvas_width < 0) canvas_width = 0;   // Проверка, чтобы ширина канвы не была отрицательной (если отступы слишком большие)
    if (canvas_height < 0) canvas_height = 0;  // Проверка, чтобы высота канвы не была отрицательной (если отступы слишком большие)
}

// Transform canvas coordinates to window coordinates
int transform_x(int canvas_x_coord) { return canvas_x + canvas_x_coord; }
// Функция преобразует X-координату канвы в X-координату окна.
// Добавляет X-смещение канвы (`canvas_x`) к заданной координате канвы.
int transform_y(int canvas_y_coord) { return canvas_y + canvas_y_coord; }
// Функция преобразует Y-координату канвы в Y-координату окна.
// Добавляет Y-смещение канвы (`canvas_y`) к заданной координате канвы.

// Draw text with background
void draw_text_with_background(int x, int y, const char *text) {
    // Функция рисует текст с фоном.
    // x, y - координаты нижнего левого угла текста (в координатах окна)
    // text - строка текста для отображения
    int text_width = gfx_textwidth(text); // Получение ширины текста в пикселях с помощью функции gfx_textwidth из gfx.h
    int rect_width = text_width + 2 * TEXT_PADDING_X; // Вычисление ширины прямоугольника фона с учетом горизонтальных отступов
    int rect_height = LINE_HEIGHT + 2 * TEXT_PADDING_Y; // Вычисление высоты прямоугольника фона с учетом вертикальных отступов и высоты строки

    gfx_color(TEXT_BG_COLOR_R, TEXT_BG_COLOR_G, TEXT_BG_COLOR_B); // Установка цвета фона текста
    gfx_fill_rectangle(x, y - LINE_HEIGHT - TEXT_PADDING_Y, rect_width, rect_height); // Рисование заполненного прямоугольника фона, координаты скорректированы для позиционирования фона за текстом
    gfx_color(TEXT_FG_COLOR_R, TEXT_FG_COLOR_G, TEXT_FG_COLOR_B); // Установка цвета текста (передний план)
    gfx_string(x + TEXT_PADDING_X, y - LINE_HEIGHT + TEXT_PADDING_Y, text); // Рисование текста с отступами внутри прямоугольника фона, координаты скорректированы для позиционирования текста внутри фона
}

// Base function to draw canvas frame and common text
void draw_base_frame(const char *title, const char *description) {
    // Функция рисует базовый фрейм для демонстраций: очищает экран, рисует рамку канвы и выводит заголовок и описание.
    // title - заголовок демонстрации
    // description - описание демонстрации
    gfx_clear(); // Очистка экрана (заполнение фоновым цветом, определенным в gfx.c)
    gfx_color(255, 255, 255); // Установка белого цвета для рамки
    gfx_rectangle(canvas_x, canvas_y, canvas_width, canvas_height); // Рисование прямоугольной рамки вокруг канвы
    draw_text_with_background(50, 50, title); // Вывод заголовка с фоном в указанных координатах
    draw_text_with_background(50, 80, description); // Вывод описания с фоном
    draw_text_with_background(50, 110, "Press any key to continue (Q to quit)."); // Вывод инструкции по управлению
}

// Demo functions (n0 to n8)
void n0() {
    // Демонстрация n0: Обзор GFX Library.
    gfx_clear(); // Очистка экрана
    gfx_color(255, 255, 255); // Установка белого цвета
    gfx_rectangle(canvas_x, canvas_y, canvas_width, canvas_height); // Рисование рамки канвы

    // Статический текст рисуем один раз перед циклом
    draw_text_with_background(50, 50, "n0: GFX Library Overview"); // Вывод заголовка демонстрации
    draw_text_with_background(50, 80, "Detailed info with a cosmic animation."); // Вывод описания демонстрации
    draw_text_with_background(50, 140, "Creators:"); // Вывод заголовка раздела "Creators"
    draw_text_with_background(70, 170, "- Douglas Thain (Original, 2011)"); // Вывод информации о создателе
    draw_text_with_background(70, 200, "- Ivan Svarkovsky (Expanded, 2019)"); // Вывод информации о расширении библиотеки
    draw_text_with_background(70, 230, "- Ivan Svarkovsky (Modernized, 2024)"); // Вывод информации о модернизации библиотеки
    draw_text_with_background(50, 260, "History:"); // Вывод заголовка раздела "History"
    draw_text_with_background(70, 290, "- V1: Basic shapes, Xlib wrapper"); // Вывод информации об истории версий библиотеки
    draw_text_with_background(70, 320, "- V2: Bug fixes, animation support"); // Вывод информации об истории версий библиотеки
    draw_text_with_background(70, 350, "- V3: Alpha, Double Buffering, XSHM"); // Вывод информации об истории версий библиотеки
    draw_text_with_background(50, 380, "Features:"); // Вывод заголовка раздела "Features"
    draw_text_with_background(70, 410, "- Points, Lines, Circles, Rectangles"); // Вывод информации о возможностях библиотеки
    draw_text_with_background(70, 440, "- Filled Shapes, Text, Color Control"); // Вывод информации о возможностях библиотеки
    draw_text_with_background(70, 470, "- Alpha Blending, Double Buffering"); // Вывод информации о возможностях библиотеки
    draw_text_with_background(50, 530, "Press any key to continue (Q to quit)."); // Вывод инструкции по управлению

    float time = 0.0f; // Переменная для хранения времени, используется для анимации
    static int scroll_x1 = 0;  // Позиция первой синусоиды по горизонтали, static для сохранения значения между вызовами функции
    static int scroll_x2;  // Объявление позиции второй синусоиды по горизонтали, static для сохранения значения между вызовами функции
    scroll_x2 = canvas_width / 2;  // Инициализация позиции второй синусоиды, сдвинута относительно первой
    static int direction1 = 1;  // Направление движения первой синусоиды (1 - вправо, -1 - влево), static для сохранения значения между вызовами функции
    static int direction2 = -1;  // Направление движения второй синусоиды, static для сохранения значения между вызовами функции

    while (1) { // Бесконечный цикл анимации
        // Вместо очистки, рисуем полупрозрачный черный прямоугольник для плавного исчезновения
        gfx_color_alpha(0, 0, 0, 50);  // Установка полупрозрачного черного цвета (альфа = 50)
        gfx_fill_rectangle(canvas_x + 1, canvas_y + 1, canvas_width - 2, canvas_height - 2); // Заполнение канвы полупрозрачным черным цветом для создания эффекта затухания

        // Синусоидальный скроллер
        char message[] = "GREETINGS FROM ALPHA CHANNEL DEMO - SINUS SCROLLER!"; // Строка сообщения для скроллера
        int message_len = strlen(message); // Длина строки сообщения
        int scroll_speed = 2; // Скорость скроллинга

        // Обновляем позиции синусоид в зависимости от направления
        scroll_x1 += direction1 * scroll_speed; // Изменение X-позиции первой синусоиды в зависимости от направления и скорости
        scroll_x2 += direction2 * scroll_speed; // Изменение X-позиции второй синусоиды

        // Меняем направление первой синусоиды при достижении границ
        if (scroll_x1 > canvas_width) {
            direction1 = -1;  // Достигли правой границы — идём влево
        } else if (scroll_x1 < -message_len * 10) {
            direction1 = 1;  // Достигли левой границы — идём вправо
        }

        // Меняем направление второй синусоиды при достижении границ
        if (scroll_x2 > canvas_width) {
            direction2 = -1;  // Достигли правой границы — идём влево
        } else if (scroll_x2 < -message_len * 10) {
            direction2 = 1;  // Достигли левой границы — идём вправо
        }

        // Рисуем первую синусоиду
        for (int i = 0; i < message_len; i++) { // Цикл по символам сообщения
            int char_x = canvas_x + 1 + (scroll_x1 + i * 10);  // +1 для отступа от рамки, вычисление X-координаты символа с учетом скроллинга и позиции символа в строке
            int char_y = canvas_y + canvas_height / 2 + (int)(sin(time + (float)(scroll_x1 + i * 10) / 50) * 150); // Вычисление Y-координаты символа на основе синусоиды, времени и X-позиции для создания волнообразного движения

            // Жёсткая проверка границ canvas (только по X)
            if (char_x >= canvas_x + 1 &&
                char_x + gfx_textwidth(&message[i]) <= canvas_x + canvas_width - 1)
            {
                int r, g, b, a; // Переменные для хранения компонент цвета RGBA
                float alpha_factor = 1.0f - (float)i / message_len * 0.8f; // Фактор прозрачности, уменьшается для символов в конце строки
                get_rainbow_color_alpha(time + (float)i / message_len * M_PI * 2, alpha_factor, &r, &g, &b, &a); // Получение радужного цвета с альфа-каналом
                gfx_color_alpha(r, g, b, a); // Установка цвета с альфа-каналом
                gfx_string(char_x, char_y, &message[i]); // Рисование символа
            }
        }

        // Рисуем вторую синусоиду (аналогично первой, но с другими параметрами)
        for (int i = 0; i < message_len; i++) {
            int char_x = canvas_x + 1 + (scroll_x2 + i * 15);  // +1 для отступа от рамки, другие параметры для второй синусоиды
            int char_y = canvas_y + canvas_height / 2 + (int)(sin(time + (float)(scroll_x2 + i * 20) / 120) * 260); // Другие параметры для второй синусоиды

            // Жёсткая проверка границ canvas (только по X)
            if (char_x >= canvas_x + 1 &&
                char_x + gfx_textwidth(&message[i]) <= canvas_x + canvas_width - 1)
            {
                int r, g, b, a;
                float alpha_factor = 1.0f - (float)i / message_len * 0.8f;
                get_rainbow_color_alpha(time + (float)i / message_len * M_PI * 2, alpha_factor, &r, &g, &b, &a);
                gfx_color_alpha(r, g, b, a);
                gfx_string(char_x, char_y, &message[i]);
            }
        }

        // Восстанавливаем белую рамку
        gfx_color(255, 255, 255); // Установка белого цвета
        gfx_rectangle(canvas_x, canvas_y, canvas_width, canvas_height); // Перерисовка рамки канвы поверх анимации

        gfx_flush(); // Обновление экрана (вывод нарисованного кадра)
        usleep(20000); // Пауза 20 миллисекунд для управления скоростью анимации
        time += 0.05f; // Увеличение времени для анимации

        if (gfx_event_waiting()) { // Проверка наличия событий ввода (нажатия клавиш)
            char key = gfx_wait(); // Ожидание события ввода и получение кода нажатой клавиши
            if (key == 'q' || key == 'Q') exit(0); // Выход из программы, если нажата клавиша 'q' или 'Q'
            break; // Выход из цикла анимации при нажатии любой другой клавиши
        }
    }
}

void n1() { // gfx_point()
    // Демонстрация n1: gfx_point() - рисование точек.
    draw_base_frame("n1: gfx_point()", "Drawing a grid of colored points."); // Рисование базового фрейма с заголовком и описанием
    int step = 20; // Шаг сетки точек в пикселях
    for (int x = 0; x < canvas_width; x += step) { // Цикл по X-координатам сетки
        for (int y = 0; y < canvas_height; y += step) { // Цикл по Y-координатам сетки
            gfx_color(x * 255 / canvas_width, y * 255 / canvas_height, 128); // Установка цвета точки, зависящего от ее положения в канве (градиент по X и Y)
            gfx_point(transform_x(x), transform_y(y)); // Рисование точки в преобразованных координатах окна
        }
    }
    gfx_flush(); // Обновление экрана
    gfx_wait();  // Ожидание нажатия клавиши перед завершением демонстрации
}

void n2() { // Centered Swirling Lines (Circular Proportions) - No algorithm.h
    // Демонстрация n2: gfx_line() - вращающиеся линии с центром и сохранением пропорций круга.
    draw_base_frame("n2: gfx_line()", "Swirling lines centered and with circular proportions."); // Рисование базового фрейма

    float color_phase = 0.0f; // Фаза цвета для анимации радуги
    int line_count = 15; // Количество линий
    int center_x = canvas_width / 2;  // Центр канвы по X
    int center_y = canvas_height / 2; // Центр канвы по Y
    float scale_factor = MIN(canvas_width, canvas_height) * 0.4f; // Масштабный фактор, основанный на меньшей стороне канвы для сохранения пропорций

    while (1) { // Бесконечный цикл анимации
        gfx_color_alpha(0, 0, 0, 50); // Установка полупрозрачного черного цвета
        gfx_fill_rectangle(canvas_x + 1, canvas_y + 1, canvas_width - 2, canvas_height - 2); // Заполнение канвы для эффекта затухания

        color_phase += 0.015f; // Изменение фазы цвета для анимации
        if (color_phase > 2 * M_PI) color_phase -= 2 * M_PI; // Циклическое изменение фазы

        for (int i = 0; i < line_count; i++) { // Цикл по линиям
            float angle = (float)i / line_count * 2 * M_PI + color_phase; // Вычисление угла для каждой линии, равномерно распределенных по кругу и вращающихся

            // Use canvas center and *same* scaling for X and Y to maintain circular proportions
            int x1 = transform_x(center_x + (int)(cos(angle) * scale_factor * 0.75)); // Начальная X-координата линии, ближе к центру
            int y1 = transform_y(center_y + (int)(sin(angle) * scale_factor * 0.75)); // Начальная Y-координата линии
            int x2 = transform_x(center_x + (int)(cos(angle + color_phase) * scale_factor)); // Конечная X-координата линии, дальше от центра и вращается
            int y2 = transform_y(center_y + (int)(sin(angle + color_phase) * scale_factor)); // Конечная Y-координата линии

            int r, g, b, a; // Переменные для компонент цвета RGBA
            get_rainbow_color_alpha(angle + color_phase, 0.7f, &r, &g, &b, &a); // Получение радужного цвета с альфа-каналом
            gfx_color_alpha(r, g, b, a); // Установка цвета с альфа-каналом
            gfx_line(x1, y1, x2, y2); // Рисование линии
        }

        gfx_color(255, 255, 255); // Установка белого цвета
        gfx_rectangle(canvas_x, canvas_y, canvas_width, canvas_height); // Перерисовка рамки канвы
        gfx_double_buffer_swap(); // Обмен буферов для двойной буферизации (плавная анимация)
        usleep(20000); // Пауза

        if (gfx_event_waiting()) { // Проверка событий ввода
            if (gfx_wait() == 'q') exit(0); // Выход по 'q'
            break; // Выход из цикла при нажатии любой другой клавиши
        }
    }
}

void n3() { // Circles
     // Демонстрация n3: gfx_circle() - пульсирующие круги.
     draw_base_frame("n3: gfx_circle()", "Pulsating circles."); // Рисование базового фрейма

    float color_phase = 0.0f; // Фаза цвета для радуги
    int circle_count = 10; // Количество кругов
    int center_x = canvas_width / 2; // Центр канвы по X
    int center_y = canvas_height / 2; // Центр канвы по Y
    float max_radius = MIN(canvas_width, canvas_height) * 0.45f; // Максимальный радиус кругов, основан на меньшей стороне канвы
    float time_var = 0.0f; // Переменная времени для пульсации размера
    int swirl_direction = 1; // 1 для вращения по часовой стрелке, -1 против часовой стрелки

    while (1) { // Бесконечный цикл анимации
        gfx_color_alpha(0, 0, 0, 50); // Полупрозрачный черный цвет
        gfx_fill_rectangle(canvas_x + 1, canvas_y + 1, canvas_width - 2, canvas_height - 2); // Заполнение канвы для эффекта затухания

        // Randomly change swirl direction
        if (rand() % 100 == 0) { // Смена направления вращения случайно, примерно каждые 100 кадров
            swirl_direction *= -1;
        }

        color_phase += 0.015f * swirl_direction; // Изменение фазы цвета, направление влияет на изменение
        if (color_phase > 2 * M_PI) color_phase -= 2 * M_PI; // Циклическая фаза
        if (color_phase < 0) color_phase += 2 * M_PI; // Удержание фазы в диапазоне 0-2PI

        for (int i = 0; i < circle_count; i++) { // Цикл по кругам
            float base_radius = (float)i / circle_count * max_radius; // Базовый радиус круга, увеличивается от центра
            float angle_offset = (float)i / circle_count * 2 * M_PI; // Угловое смещение для равномерного распределения кругов по кругу
            float swirl_angle = color_phase + angle_offset; // Угол вращения круга

            float size_modulation = 1.0f + 0.5f * sin(time_var * 2.0f + (float)i / circle_count * 4 * M_PI); // Модуляция размера круга на основе синусоиды времени и индекса круга для пульсации
            float dynamic_radius = (max_radius / circle_count) * 7.0f * size_modulation; // Динамический радиус с пульсацией

            int x = transform_x(center_x + (int)(cos(swirl_angle) * base_radius * 0.6)); // X-координата центра круга, вращается и удаляется от центра
            int y = transform_y(center_y + (int)(sin(swirl_angle) * base_radius * 0.6)); // Y-координата центра круга

            int r, g, b, a; // Компоненты цвета RGBA
            get_rainbow_color_alpha(color_phase + angle_offset + M_PI/4.0f, 0.5f, &r, &g, &b, &a); // Радужный цвет с альфа-каналом
            gfx_color_alpha(r, g, b, a); // Установка цвета
            gfx_circle(x, y, (int)dynamic_radius, (int)dynamic_radius); // Рисование круга
        }

        gfx_color(255, 255, 255); // Белый цвет
        gfx_rectangle(canvas_x, canvas_y, canvas_width, canvas_height); // Рамка канвы
        gfx_double_buffer_swap(); // Обмен буферов
        usleep(40000); // Пауза
        time_var += 0.025f; // Увеличение времени для пульсации

        if (gfx_event_waiting()) { // Проверка событий ввода
            if (gfx_wait() == 'q') exit(0); // Выход по 'q'
            break; // Выход при нажатии любой другой клавиши
        }
    }
}

void n4() { // gfx_rectangle()
    // Демонстрация n4: gfx_rectangle() - пульсирующие вложенные прямоугольники.
    draw_base_frame("n4: gfx_rectangle()", "Pulsating nested rectangles."); // Базовый фрейм

    int center_x = canvas_width / 2; // Центр канвы по X
    int center_y = canvas_height / 2; // Центр канвы по Y
    int num_rects = 7; // Количество прямоугольников
    int border_margin = 20; // Отступ от края канвы
    int rect_spacing = 15; // Расстояние между прямоугольниками
    float time_var = 0.0f; // Время для пульсации

    while (1) { // Анимационный цикл
        gfx_color_alpha(0, 0, 0, 50); // Полупрозрачный черный
        gfx_fill_rectangle(canvas_x + 1, canvas_y + 1, canvas_width - 2, canvas_height - 2); // Заполнение канвы для затухания

        int base_w = canvas_width - 2 * border_margin; // Базовая ширина самого большого прямоугольника
        int base_h = canvas_height - 2 * border_margin; // Базовая высота самого большого прямоугольника

        for (int i = 0; i < num_rects; i++) { // Цикл по прямоугольникам
            if (base_w <= 0 || base_h <= 0) break; // Прекращение рисования, если размеры стали нулевыми или отрицательными

            // Pulsating size factor
            float size_factor = 0.9f + 0.1f * sin(time_var * 2.0f + (float)i * 0.5f); // Фактор пульсации размера, синусоидальная функция времени и индекса

            int current_w = (int)((base_w - i * (base_w / num_rects) - rect_spacing) * size_factor); // Вычисление текущей ширины с учетом пульсации, уменьшения размера и расстояния
            int current_h = (int)((base_h - i * (base_h / num_rects) - rect_spacing) * size_factor); // Вычисление текущей высоты

            // Ensure width and height are not negative after scaling
            if (current_w < 1) current_w = 1; // Гарантия, что ширина не станет меньше 1
            if (current_h < 1) current_h = 1; // Гарантия, что высота не станет меньше 1


            int x = center_x - current_w / 2; // X-координата верхнего левого угла прямоугольника, центрирование
            int y = center_y - current_h / 2; // Y-координата верхнего левого угла

            gfx_color(255 - i * (255 / num_rects), i * (255 / num_rects), 128); // Цвет прямоугольника, меняется в зависимости от индекса
            gfx_rectangle(transform_x(x), transform_y(y), current_w, current_h); // Рисование прямоугольника

            base_w -= (canvas_width - 2 * border_margin) / num_rects + rect_spacing; // Уменьшение базовой ширины для следующего прямоугольника
            base_h -= (canvas_height - 2 * border_margin) / num_rects + rect_spacing; // Уменьшение базовой высоты
        }

        gfx_color(255, 255, 255); // Белый цвет
        gfx_rectangle(canvas_x, canvas_y, canvas_width, canvas_height); // Рамка канвы
        gfx_double_buffer_swap(); // Обмен буферов
        usleep(30000); // Пауза
        time_var += 0.025f; // Увеличение времени для пульсации

        if (gfx_event_waiting()) { // Проверка событий ввода
            if (gfx_wait() == 'q') exit(0); // Выход по 'q'
            break; // Выход при нажатии любой другой клавиши
        }
    }
}

void n5() { // gfx_fill_circle()
    // Демонстрация n5: gfx_fill_circle() - движущийся заполненный круг.
    draw_base_frame("n5: gfx_fill_circle()", "Moving filled circle."); // Базовый фрейм
    int x = canvas_width / 4; // Начальная X-позиция круга
    int y = canvas_height / 4; // Начальная Y-позиция круга
    int dx = 3, dy = 2; // Скорость движения по X и Y
    int radius = 30; // Радиус круга

    while (1) { // Анимационный цикл
        draw_base_frame("n5: gfx_fill_circle()", "Moving filled circle."); // Redraw frame - Перерисовка фрейма на каждом кадре, чтобы стереть предыдущий круг (неэффективно, лучше использовать очистку части экрана)
        x += dx; // Изменение X-позиции
        y += dy; // Изменение Y-позиции
        if (x + radius > canvas_width || x - radius < 0) dx = -dx; // Отскок от левой и правой границ
        if (y + radius > canvas_height || y - radius < 0) dy = -dy; // Отскок от верхней и нижней границ
        gfx_color(x * 255 / canvas_width, y * 255 / canvas_height, 128); // Цвет круга, зависящий от X и Y
        gfx_fill_circle(transform_x(x), transform_y(y), radius * 2, radius * 2); // Рисование заполненного круга (диаметр = 2*радиус)
        gfx_flush(); // Обновление экрана
        usleep(20000); // Пауза
        if (gfx_event_waiting()) { // Проверка событий ввода
            gfx_wait(); // Ожидание нажатия клавиши
            break; // Выход из цикла
        }
    }
}

void n6() {
    // Демонстрация n6: gfx_fill_rectangle() - одиночные прямоугольники, движущиеся случайно.
    draw_base_frame("n6: gfx_fill_rectangle()", "Single rectangles moving randomly."); // Базовый фрейм

    const int grid_cols = 5, grid_rows = 5; // Размер сетки прямоугольников
    const int num_rects = grid_cols * grid_rows; // Общее количество прямоугольников
    const int rect_width = (canvas_width - 2) / grid_cols; // Ширина прямоугольника, равномерно распределенная по ширине канвы
    const int rect_height = (canvas_height - 2) / grid_rows; // Высота прямоугольника, равномерно распределенная по высоте канвы
    int rect_positions[num_rects][2]; // Массив позиций прямоугольников [x, y]
    int colors[num_rects][3]; // Массив цветов прямоугольников [r, g, b]
    int rect_indices[num_rects]; // Массив индексов прямоугольников (для случайного порядка отрисовки)
    int rect_directions[num_rects][2]; // Массив направлений движения прямоугольников [dx, dy]

    for (int i = 0; i < num_rects; ++i) { // Инициализация параметров для каждого прямоугольника
        rect_positions[i][0] = (i % grid_cols) * rect_width + 1; // Начальная X-позиция в сетке
        rect_positions[i][1] = (i / grid_cols) * rect_height + 1; // Начальная Y-позиция в сетке
        for (int j = 0; j < 3; ++j) colors[i][j] = rand() % 256; // Случайный цвет
        rect_indices[i] = i; // Изначально индексы по порядку
        rect_directions[i][0] = 0; // Изначально нет направления движения по X
        rect_directions[i][1] = 0; // Изначально нет направления движения по Y
    }

    // Initial shuffle (optional, but keeps dynamic start)
    for (int i = 0; i < num_rects; ++i) { // Случайное перемешивание индексов для динамического старта
        int rand_index = rand() % num_rects; // Случайный индекс
        int temp_index = rect_indices[i]; // Временное хранение индекса
        rect_indices[i] = rect_indices[rand_index]; // Обмен индексами
        rect_indices[rand_index] = temp_index; // Завершение обмена
    }

    float animation_time = 0.0f; // **Переименовано: animation_time** - Время анимации (переименовано для ясности)
    srand(time(NULL)); // Seed random number generator for more varied directions - Инициализация генератора случайных чисел для разнообразных направлений

    while (1) { // Анимационный цикл
        gfx_color_alpha(0, 0, 0, 50); // Полупрозрачный черный
        gfx_fill_rectangle(canvas_x + 1, canvas_y + 1, canvas_width - 2, canvas_height - 2); // Заполнение канвы для затухания

        // Move each rectangle randomly
        for (int i = 0; i < num_rects; ++i) { // Цикл по прямоугольникам
            int rect_index = rect_indices[i]; // Получение индекса прямоугольника (для случайного порядка)

            // Randomly change direction sometimes
            if (rand() % 50 == 0) { // Change direction roughly every 50 frames - Случайное изменение направления примерно каждые 50 кадров
                int direction = rand() % 4; // 0: right, 1: left, 2: down, 3: up - Случайное направление (право, лево, вниз, вверх)
                switch (direction) { // Установка направления в зависимости от случайного выбора
                    case 0: rect_directions[rect_index][0] = 1; rect_directions[rect_index][1] = 0; break;  // Right - Вправо
                    case 1: rect_directions[rect_index][0] = -1; rect_directions[rect_index][1] = 0; break; // Left - Влево
                    case 2: rect_directions[rect_index][0] = 0; rect_directions[rect_index][1] = 1; break;  // Down - Вниз
                    case 3: rect_directions[rect_index][0] = 0; rect_directions[rect_index][1] = -1; break; // Up - Вверх
                }
            }

            rect_positions[rect_index][0] += rect_directions[rect_index][0] * 2; // Move X, adjust speed here - Изменение X-позиции, скорость регулируется множителем
            rect_positions[rect_index][1] += rect_directions[rect_index][1] * 2; // Move Y, adjust speed here - Изменение Y-позиции

            // Boundary collision detection and direction reversal
            int x = rect_positions[rect_index][0]; // Текущая X-позиция
            int y = rect_positions[rect_index][1]; // Текущая Y-позиция

            if (x + rect_width > canvas_width -1 ) { // Столкновение с правой границей
                rect_positions[rect_index][0] = canvas_width - 1 - rect_width; // Установка позиции у границы
                rect_directions[rect_index][0] = -1; // Reverse X direction - Изменение направления по X на противоположное
            }
            if (x < 1) { // Столкновение с левой границей
                rect_positions[rect_index][0] = 1; // Установка позиции у границы
                rect_directions[rect_index][0] = 1;  // Reverse X direction - Изменение направления по X
            }
            if (y + rect_height > canvas_height - 1) { // Столкновение с нижней границей
                rect_positions[rect_index][1] = canvas_height - 1 - rect_height; // Установка позиции у границы
                rect_directions[rect_index][1] = -1; // Reverse Y direction - Изменение направления по Y
            }
            if (y < 1) { // Столкновение с верхней границей
                rect_positions[rect_index][1] = 1; // Установка позиции у границы
                rect_directions[rect_index][1] = 1;  // Reverse Y direction - Изменение направления по Y
            }
        }

        // Draw rectangles
        for (int i = 0; i < num_rects; ++i) { // Цикл по прямоугольникам для отрисовки
            int rect_index_to_draw = rect_indices[i]; // Индекс прямоугольника для отрисовки (случайный порядок)
            int x = rect_positions[rect_index_to_draw][0]; // X-позиция для отрисовки
            int y = rect_positions[rect_index_to_draw][1]; // Y-позиция для отрисовки

            gfx_double_buffer_fill_rectangle(transform_x(x), transform_y(y), rect_width, rect_height,
                                            colors[rect_index_to_draw][0], colors[rect_index_to_draw][1], colors[rect_index_to_draw][2], 255); // Рисование заполненного прямоугольника с двойной буферизацией и установленным цветом
        }

        gfx_color(255, 255, 255); // Белый цвет
        gfx_rectangle(canvas_x, canvas_y, canvas_width, canvas_height); // Рамка канвы
        gfx_double_buffer_swap(); // Обмен буферов
        usleep(30000); // Пауза
        animation_time += 0.05f; // Увеличение времени анимации

        if (gfx_event_waiting()) { // Проверка событий ввода
            if (gfx_wait() == 'q') exit(0); // Выход по 'q'
            break; // Выход при нажатии любой другой клавиши
        }
    }
}

void n7() { // GetPix()
    // Демонстрация n7: GetPix() - получение цвета пикселя.
    draw_base_frame("n7: GetPix()", "Click to see pixel color."); // Базовый фрейм
    int palette_width = canvas_width * 0.8; // Ширина цветовой палитры (80% от ширины канвы)
    int palette_height = canvas_height * 0.8; // Высота цветовой палитры (80% от высоты канвы)
    int start_x = (canvas_width - palette_width) / 2; // Начальная X-позиция палитры, центрирование по горизонтали
    int start_y = (canvas_height - palette_height) / 2; // Начальная Y-позиция палитры, центрирование по вертикали

    for (int y = 0; y < palette_height; y++) { // Цикл по строкам палитры
        for (int x = 0; x < palette_width; x++) { // Цикл по столбцам палитры
            int r = x * 255 / palette_width; // Красная компонента цвета, градиент по X
            int g = y * 255 / palette_height; // Зеленая компонента цвета, градиент по Y
            int b = 255 - x * 255 / palette_width; // Синяя компонента цвета, обратный градиент по X
            gfx_color(r, g, b); // Установка цвета
            gfx_point(transform_x(start_x + x), transform_y(start_y + y)); // Рисование точки в палитре
        }
    }
    gfx_flush(); // Обновление экрана (отрисовка палитры)

    while (1) { // Бесконечный цикл ожидания кликов мыши
        char event = gfx_wait(); // Ожидание события ввода (клик мыши или нажатие клавиши)
        if (event > 3) break; // Выход из цикла, если событие не клик мыши (код клика <= 3)
        int x_click = gfx_xpos() - canvas_x; // X-координата клика относительно канвы
        int y_click = gfx_ypos() - canvas_y; // Y-координата клика относительно канвы
        if (x_click >= 0 && x_click < canvas_width && y_click >= 0 && y_click < canvas_height) { // Проверка, что клик внутри канвы
            int color = GetPix(gfx_xpos(), gfx_ypos()); // Получение цвета пикселя в точке клика с помощью функции GetPix из gfx.h
            int r = (color >> 16) & 0xFF; // Извлечение красной компоненты цвета из целого числа
            int g = (color >> 8) & 0xFF;  // Извлечение зеленой компоненты цвета
            int b = color & 0xFF;      // Извлечение синей компоненты цвета
            char color_str[50]; // Буфер для строки с информацией о цвете
            sprintf(color_str, "R=%d G=%d B=%d at (%d, %d)", r, g, b, x_click, y_click); // Форматирование строки с информацией о цвете и координатах клика
            draw_base_frame("n7: GetPix()", "Click to see pixel color."); // Redraw frame - Перерисовка базового фрейма
            draw_text_with_background(50, 140, color_str); // Вывод информации о цвете пикселя
            for (int y = 0; y < palette_height; y++) { // Перерисовка палитры (необходимо, так как draw_base_frame очищает экран)
                for (int x = 0; x < palette_width; x++) {
                    int r = x * 255 / palette_width;
                    int g = y * 255 / palette_height;
                    int b = 255 - x * 255 / palette_width;
                    gfx_color(r, g, b);
                    gfx_point(transform_x(start_x + x), transform_y(start_y + y));
                }
            }
            gfx_flush(); // Обновление экрана
        }
    }
}

//
void n8() { // Style
    // Демонстрация n8: Style - пока пустая демонстрация стиля (функциональность не реализована).
}

// Array of demo functions
void (*demos[])() = {n0, n1, n2, n3, n4, n5, n6, n7, n8};
// Массив указателей на демонстрационные функции.
// `demos` - имя массива.
// `()` - указывает, что это массив функций.
// `void (*)()` - тип элементов массива: указатель на функцию, не принимающую аргументов и возвращающую void.
// `{n0, n1, n2, n3, n4, n5, n6, n7, n8}` - инициализация массива адресами функций n0, n1, ..., n8.
const int num_demos = 9; // Количество демонстрационных функций в массиве `demos`

// Main function
int main() {
    // Главная функция программы.
    gfx_open(WINDOW_WIDTH, WINDOW_HEIGHT, "GFX Library Demo"); // Открытие графического окна с заданными размерами и заголовком, функция gfx_open определена в gfx.c
    init_canvas(); // Инициализация параметров канвы

    if (gfx_textwidth("Test") == 0 && gfx_get_error() != "") { // Проверка работы функции gfx_textwidth и обработки ошибок
        fprintf(stderr, "Error: gfx_textwidth issues: %s\n", gfx_get_error()); // Вывод сообщения об ошибке в stderr, если gfx_textwidth вернула 0 и есть сообщение об ошибке
        return 1; // Возврат кода ошибки 1
    }

    int current_demo = 0; // Индекс текущей демонстрации, начинается с первой (n0)
    while (1) { // Бесконечный цикл демонстраций
        demos[current_demo](); // Вызов текущей демонстрационной функции из массива `demos` по индексу `current_demo`
        current_demo = (current_demo + 1) % num_demos; // Переход к следующей демонстрации, циклический переход через `% num_demos`
        if (current_demo == 0) n0(); // Always return to n0 after cycle - Всегда возвращаться к демонстрации n0 после завершения цикла всех демонстраций
    }

    return 0; // Успешное завершение программы
}
