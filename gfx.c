/*
    A simple graphics library for CSE 20211 by Douglas Thain
    For complete documentation, see:
    http://www.nd Nog.edu/~dthain/courses/cse20211/fall2011/gfx
    Version 3, 11/07/2012 - Now much faster at changing colors rapidly.
    Version 2, 9/23/2011 - Fixes a bug that could result in jerky animation.
*/

/*
    Copyright (c) 2019 Ivan Svarkovsky	Version 3a
    06/19/2019 - Expanded and supplemented
*/

/*
    Copyright (c) 2024 Ivan Svarkovsky	Version 3b
    06/04/2024 - Added a new section for alpha channel support functions (non-destructive addition)
    06/04/2024 - Added a new section for double buffering support functions (non-destructive addition)
    06/05/2024 - Rewrote double buffering for efficiency and to fix flickering/artifacts, using XSHM.
    06/06/2024 - Re-added XSHM support with backward compatibility.
    06/06/2024 - Refactored XSHM to separate functions for better backward compatibility and conditional compilation.
    06/06/2024 - Optimized gfx_double_buffer_fill_circle and gfx_double_buffer_fill_polygon for faster rendering.
    06/06/2024 - Optimized gfx_double_buffer_fill_ellipse using Midpoint Ellipse Algorithm.
    06/06/2024 - Replaced bubble sort in gfx_double_buffer_fill_polygon with qsort for intersection sorting.
*/

#include <stdio.h>
#include <stdlib.h> // Required for qsort
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>
#include <string.h>
#include "gfx.h"
#include <math.h>

int usleep(unsigned int __useconds); // Явне оголошення функції usleep()

#ifdef USE_XSHM // Conditional compilation for XSHM
#include <sys/shm.h>    // Required for XSHM
#include <X11/extensions/XShm.h> // Required for XSHM
#endif

/* ====================================================================== */
/*                  GLOBAL VARIABLES SECTION                              */
/* ====================================================================== */

static Display *gfx_display = 0;
static Window gfx_window;
static GC gfx_gc;
static Colormap gfx_colormap;
static int gfx_fast_color_mode = 0;
static int window_width = 0;  // Store window width
static int window_height = 0; // Store window height
static int saved_xpos = 0;    // Saved X position for events
static int saved_ypos = 0;    // Saved Y position for events

/* Global variables for double buffering */
static XImage *back_buffer = NULL;
static unsigned char *back_buffer_data = NULL; // RGBA buffer (4 bytes per pixel)
static int double_buffer_enabled = 0;
static Visual *gfx_visual = NULL;
static int gfx_depth = 0;

#ifdef USE_XSHM // Conditional compilation for XSHM
/* XSHM specific variables */
static XShmSegmentInfo shminfo;
static int use_shm = 0; // Flag to indicate if XSHM is used
#else
static int use_shm = 0; // Flag to indicate if XSHM is used (but always false if not compiled with XSHM)
#endif

void (*current_demo_function)(void) = NULL; // 


/* ====================================================================== */
/*                  INTERNAL HELPER FUNCTIONS                            */
/* ====================================================================== */

/* Function for alpha blending pixels */
static void blend_pixel(unsigned char *dest, int r_src, int g_src, int b_src, int a_src)
{
    float alpha = a_src / 255.0f;
    int r_dest = dest[0];
    int g_dest = dest[1];
    int b_dest = dest[2];

    r_dest = (int)(r_src * alpha + r_dest * (1.0f - alpha));
    g_dest = (int)(g_src * alpha + g_dest * (1.0f - alpha));
    b_dest = (int)(b_src * alpha + b_dest * (1.0f - alpha));

    dest[0] = r_dest > 255 ? 255 : r_dest;
    dest[1] = g_dest > 255 ? 255 : g_dest;
    dest[2] = b_dest > 255 ? 255 : b_dest;
    dest[3] = 255; // Back buffer is fully opaque after blending
}

/* Helper function to find maximum of two integers */
static inline int max_int(int a, int b) {
    return (a > b) ? a : b;
}

/* Helper function to find minimum of two integers */
static inline int min_int(int a, int b) {
    return (a < b) ? a : b;
}

/* Comparison function for qsort to sort integers in ascending order */
static int compare_intersections(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

/* ====================================================================== */
/*                  BASIC GRAPHICS FUNCTIONS SECTION                      */
/* ====================================================================== */

/* Open a new graphics window. */
void gfx_open(int width, int height, const char *title)
{
    gfx_display = XOpenDisplay(0);
    if (!gfx_display)
    {
        fprintf(stderr, "gfx_open: unable to open the graphics window.\n");
        exit(1);
    }
    gfx_visual = DefaultVisual(gfx_display, 0);
    gfx_depth = DefaultDepth(gfx_display, 0);
    if (gfx_visual && gfx_visual->class == TrueColor)
    {
        gfx_fast_color_mode = 1;
    }
    else
    {
        gfx_fast_color_mode = 0;
    }
    int blackColor = BlackPixel(gfx_display, DefaultScreen(gfx_display));
    int whiteColor = WhitePixel(gfx_display, DefaultScreen(gfx_display));
    gfx_window = XCreateSimpleWindow(gfx_display, DefaultRootWindow(gfx_display), 0, 0, width, height, 0, blackColor, blackColor);
    XSetWindowAttributes attr;
    attr.backing_store = Always;
    XChangeWindowAttributes(gfx_display, gfx_window, CWBackingStore, &attr);
    XStoreName(gfx_display, gfx_window, title);
    XSelectInput(gfx_display, gfx_window, StructureNotifyMask | KeyPressMask | ButtonPressMask | ExposureMask); // Added ExposureMask
    XMapWindow(gfx_display, gfx_window);
    gfx_gc = XCreateGC(gfx_display, gfx_window, 0, 0);
    gfx_colormap = DefaultColormap(gfx_display, 0);
    XSetForeground(gfx_display, gfx_gc, whiteColor);
    for (;;)
    {
        XEvent e;
        XNextEvent(gfx_display, &e);
        if (e.type == MapNotify)
        {
            break;
        }
    }
    window_width = width;
    window_height = height;
}

/* Draw a single point at (x,y) */
void gfx_point(int x, int y)
{
    XDrawPoint(gfx_display, gfx_window, gfx_gc, x, y);
}

/* Draw a line from (x1,y1) to (x2,y2) */
void gfx_line(int x1, int y1, int x2, int y2)
{
    XDrawLine(gfx_display, gfx_window, gfx_gc, x1, y1, x2, y2);
}

/* Draw a string */
void gfx_string(int x, int y, const char *cc)
{
    XDrawString(gfx_display, gfx_window, gfx_gc, x, y, cc, strlen(cc));
}

/* Draw one circle */
void gfx_circle(int x1, int y1, int width, int height)
{
    int angle1 = 0 * 64;
    int angle2 = 360 * 64;
    x1 = x1 - (width / 2);
    y1 = y1 - (height / 2);
    XDrawArc(gfx_display, gfx_window, gfx_gc, x1, y1, width, height, angle1, angle2);
}

/* Draw one fill circle */
void gfx_fill_circle(int x1, int y1, int width, int height)
{
    int angle1 = 0 * 64;
    int angle2 = 360 * 64;
    x1 = x1 - (width / 2);
    y1 = y1 - (height / 2);
    XFillArc(gfx_display, gfx_window, gfx_gc, x1, y1, width, height, angle1, angle2);
}

/* Draw one rectangle */
void gfx_rectangle(int x1, int y1, int width, int height)
{
    XDrawRectangle(gfx_display, gfx_window, gfx_gc, x1, y1, width, height);
}

/* Draw one fill rectangle */
void gfx_fill_rectangle(int x1, int y1, int width, int height)
{
    XFillRectangle(gfx_display, gfx_window, gfx_gc, x1, y1, width, height);
}

/**
 * @brief Get the width of a text string in pixels using the current font.
 *
 * @param cc The null-terminated string to measure.
 * @return The width of the string in pixels. Returns 0 if gfx_display is not initialized.
 */
int gfx_textwidth(const char *cc) {
    if (!gfx_display) {
        fprintf(stderr, "gfx_textwidth: gfx_display is not initialized.\n");
        return 0;
    }
    if (cc == NULL) {
        fprintf(stderr, "gfx_textwidth: Input string is NULL.\n");
        return 0;
    }

    XFontStruct *font_struct = XQueryFont(gfx_display, XGContextFromGC(gfx_gc));
    if (!font_struct) {
        fprintf(stderr, "gfx_textwidth: Failed to query font.\n");
        return 0;
    }

    int width = XTextWidth(font_struct, cc, strlen(cc));
    // XFreeFontInfo(gfx_display, font_struct, 1); // Free font info - УДАЛЕНО!

    return width;
}


/**
 * @brief Get the last error message. (Currently a placeholder)
 *
 * @return An empty string.  This function is a placeholder and does not yet implement error reporting.
 */
const char* gfx_get_error() {
    return ""; // Placeholder - no error reporting yet
}


/* Change the current drawing color. */
void gfx_color(int r, int g, int b)
{
    XColor color;
    if (gfx_fast_color_mode)
    {
        color.pixel = ((b & 0xff) | ((g & 0xff) << 8) | ((r & 0xff) << 16));
    }
    else
    {
        color.pixel = 0;
        color.red = r << 8;
        color.green = g << 8;
        color.blue = b << 8;
        XAllocColor(gfx_display, gfx_colormap, &color);
    }
    XSetForeground(gfx_display, gfx_gc, color.pixel);
}

/* Clear the graphics window to the background color. */
void gfx_clear()
{
    int blackColor = BlackPixel(gfx_display, DefaultScreen(gfx_display));
    XSetForeground(gfx_display, gfx_gc, blackColor);
    XClearWindow(gfx_display, gfx_window);
    XSetForeground(gfx_display, gfx_gc, BlackPixel(gfx_display, DefaultScreen(gfx_display)));
    gfx_color(0, 0, 0);
}

/* Change the current background color. */
void gfx_clear_color(int r, int g, int b)
{
    XColor color;
    color.pixel = 0;
    color.red = r << 8;
    color.green = g << 8;
    color.blue = b << 8;
    XAllocColor(gfx_display, gfx_colormap, &color);
    XSetWindowAttributes attr;
    attr.background_pixel = color.pixel;
    XChangeWindowAttributes(gfx_display, gfx_window, CWBackPixel, &attr);
}

/* Функция перерисовки, вызывающая текущую демо-функцию */
void gfx_redraw() {
    if (current_demo_function != NULL) {
        current_demo_function(); // Вызываем сохраненную демо-функцию для перерисовки
    }
}

/* Check to see if an event is waiting. */
int gfx_event_waiting()
{
    XEvent event;
    gfx_flush();
    while (1)
    {
        if (XCheckMaskEvent(gfx_display, -1, &event))
        {
            if (event.type == KeyPress)
            {
                XPutBackEvent(gfx_display, &event);
                return 1;
            }
            else if (event.type == ButtonPress)
            {
                XPutBackEvent(gfx_display, &event);
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
}

/* Wait for the user to press a key or mouse button. */
char gfx_wait()
{
    XEvent event;
    gfx_flush();
    while (1)
    {
        XNextEvent(gfx_display, &event);
        if (event.type == KeyPress)
        {
            saved_xpos = event.xkey.x;
            saved_ypos = event.xkey.y;
            return XLookupKeysym(&event.xkey, 0);
        }
        else if (event.type == ButtonPress)
        {
            saved_xpos = event.xbutton.x;
            saved_ypos = event.xbutton.y;
            return event.xbutton.button;
        }
        else if (event.type == Expose)
        {
            if (current_demo_function != NULL) {
                gfx_redraw(); // Вызываем функцию перерисовки при событии Expose
            }
            // После перерисовки, продолжаем ждать другие события (клавиши/кнопки)
        }
    }
}

/* Return the X and Y coordinates of the last event. */
int gfx_xpos()
{
    return saved_xpos;
}

int gfx_ypos()
{
    return saved_ypos;
}

/* Return the X and Y dimensions of the window. */
int gfx_xsize()
{
    return window_width;
}

int gfx_ysize()
{
    return window_height;
}

/* Flush all previous output to the window. */
void gfx_flush()
{
    XFlush(gfx_display);
}

/* XGetPixel() function returns the specified pixel from the named image. */
int GetPix(int x, int y)
{
    XColor color;
    XImage *image;
    image = XGetImage(gfx_display, gfx_window, x, y, 1, 1, AllPlanes, XYPixmap);
    color.pixel = XGetPixel(image, 0, 0);
    XDestroyImage(image);
    unsigned long red_mask = image->red_mask;
    unsigned long green_mask = image->green_mask;
    unsigned long blue_mask = image->blue_mask;
    unsigned char blue = (color.pixel & blue_mask);
    unsigned char green = (color.pixel & green_mask) >> 8;
    unsigned char red = (color.pixel & red_mask) >> 16;
    return ((red << 16) + (green << 8) + blue);
}

/* Read keys */
int gfx_xreadkeys()
{
    XEvent event;
    XNextEvent(gfx_display, &event);
    KeySym sym = XLookupKeysym(&event.xkey, 0);
    if (event.type == KeyPress)
    {
        printf("KeyCode: %2x KeySym: %4lx - (%s) \n", event.xkey.keycode, sym, XKeysymToString(sym));
        return sym;
    }
    else
    {
        return -1;
    }
}

/* With control of the number of events read key */
int gfx_m_xreadkeys()
{
    if (XPending(gfx_display) > 0)
    {
        XEvent event;
        XNextEvent(gfx_display, &event);
        KeySym sym = XLookupKeysym(&event.xkey, 0);
        if (event.type == KeyPress)
        {
            printf("KeyCode: %2x KeySym: %4lx - (%s) \n", event.xkey.keycode, sym, XKeysymToString(sym));
            return sym;
        }
    }
    else
    {
        return -1;
    }
}

/* Moving window to left */
int gfx_move_win_l(int x, int y, int distance, int delay, int step)
{
    for (int i = 0; i < distance; i++)
    {
        x -= step;
        XMoveWindow(gfx_display, gfx_window, x, y);
        XFlush(gfx_display);
        usleep(delay);
    }
    return x;
}

/* Moving window to down */
int gfx_move_win_d(int x, int y, int distance, int delay, int step)
{
    for (int i = 0; i < distance; i++)
    {
        y += step;
        XMoveWindow(gfx_display, gfx_window, x, y);
        XFlush(gfx_display);
        usleep(delay);
    }
    return y;
}

/* Moving window to right */
int gfx_move_win_r(int x, int y, int distance, int delay, int step)
{
    for (int i = 0; i < distance; i++)
    {
        x += step;
        XMoveWindow(gfx_display, gfx_window, x, y);
        XFlush(gfx_display);
        usleep(delay);
    }
    return x;
}

/* Moving window to up */
int gfx_move_win_u(int x, int y, int distance, int delay, int step)
{
    for (int i = 0; i < distance; i++)
    {
        y -= step;
        XMoveWindow(gfx_display, gfx_window, x, y);
        XFlush(gfx_display);
        usleep(delay);
    }
    return y;
}

/* ====================================================================== */
/*                  ALPHA CHANNEL SUPPORT SECTION                         */
/* ====================================================================== */

static int current_alpha_r = 255;
static int current_alpha_g = 255;
static int current_alpha_b = 255;
static int current_alpha_a = 255;

/* Change the current drawing color with alpha. */
void gfx_color_alpha(int r, int g, int b, int a)
{
    XColor color;
    current_alpha_r = r;
    current_alpha_g = g;
    current_alpha_b = b;
    current_alpha_a = a;

    if (gfx_fast_color_mode)
    {
        color.pixel = ((b & 0xff) | ((g & 0xff) << 8) | ((r & 0xff) << 16));
    }
    else
    {
        color.pixel = 0;
        color.red = r << 8;
        color.green = g << 8;
        color.blue = b << 8;
        XAllocColor(gfx_display, gfx_colormap, &color);
    }
    XSetForeground(gfx_display, gfx_gc, color.pixel);
}

/* Draw a filled polygon on the back buffer with alpha blending - OPTIMIZED SCANLINE FILL + QSORT */
void gfx_double_buffer_fill_polygon(int *x_points, int *y_points, int num_points, int r, int g, int b, int a)
{
    if (!double_buffer_enabled || !back_buffer_data) return;

    int min_y = y_points[0], max_y = y_points[0], min_x = x_points[0], max_x = x_points[0];
    for (int i = 1; i < num_points; i++) {
        if (y_points[i] < min_y) min_y = y_points[i];
        if (y_points[i] > max_y) max_y = y_points[i];
        if (x_points[i] < min_x) min_x = x_points[i];
        if (x_points[i] > max_x) max_x = x_points[i];
    }

    min_y = max_int(0, min_y);
    max_y = min_int(window_height - 1, max_y);
    min_x = max_int(0, min_x);
    max_x = min_int(window_width - 1, max_x);

    if (min_y > max_y || min_x > max_x) return;

    int intersections[num_points];

    for (int y = min_y; y <= max_y; y++) {
        int intersection_count = 0;

        for (int i = 0; i < num_points; i++) {
            int x1 = x_points[i], y1 = y_points[i];
            int x2 = x_points[(i + 1) % num_points], y2 = y_points[(i + 1) % num_points];

            if (((y1 <= y) && (y2 > y)) || ((y1 > y) && (y2 <= y))) {
                intersections[intersection_count++] = (int)(x1 + (double)(y - y1) / (y2 - y1) * (x2 - x1));
            }
        }

        qsort(intersections, intersection_count, sizeof(int), compare_intersections);

        for (int i = 0; i < intersection_count; i += 2) {
            int x_start = max_int(min_x, intersections[i]);
            int x_end = min_int(max_x, intersections[i + 1]);

            if (x_start < x_end) {
                for (int x = x_start; x < x_end; x++) {
                    int idx = (y * window_width + x) * 4;
                    blend_pixel(&back_buffer_data[idx], r, g, b, a);
                }
            }
        }
    }
}

/* Draw a filled circle on the back buffer with alpha blending - OPTIMIZED MIDPOINT CIRCLE ALGORITHM */
void gfx_double_buffer_fill_circle_alpha(int x_center, int y_center, int radius, int r, int g, int b, int a)
{
    if (!double_buffer_enabled || !back_buffer_data) {
        gfx_color_alpha(r, g, b, a);
        gfx_fill_circle(x_center, y_center, radius * 2, radius * 2);
        return;
    }

    int x = 0;
    int y = radius;
    int decisionOver2 = 1 - radius;

    while (y >= x) {
        for (int i = x_center - y; i <= x_center + y; i++) {
            if (i >= 0 && i < window_width && (y_center + x) >= 0 && (y_center + x) < window_height) {
                blend_pixel(&back_buffer_data[((y_center + x) * window_width + i) * 4], r, g, b, a);
            }
            if (i >= 0 && i < window_width && (y_center - x) >= 0 && (y_center - x) < window_height) {
                blend_pixel(&back_buffer_data[((y_center - x) * window_width + i) * 4], r, g, b, a);
            }
        }
        for (int i = x_center - x; i <= x_center + x; i++) {
            if (i >= 0 && i < window_width && (y_center + y) >= 0 && (y_center + y) < window_height) {
                blend_pixel(&back_buffer_data[((y_center + y) * window_width + i) * 4], r, g, b, a);
            }
            if (i >= 0 && i < window_width && (y_center - y) >= 0 && (y_center - y) < window_height) {
                blend_pixel(&back_buffer_data[((y_center - y) * window_width + i) * 4], r, g, b, a);
            }
        }
        x++;
        if (decisionOver2 <= 0) {
            decisionOver2 += 2 * x + 3;
        } else {
            y--;
            decisionOver2 += 2 * (x - y) + 5;
        }
    }
}

/* Draw a filled ellipse on the back buffer with alpha blending - OPTIMIZED MIDPOINT ELLIPSE ALGORITHM */
void gfx_double_buffer_fill_ellipse(int x_center, int y_center, int radius_x, int radius_y, int r, int g, int b, int a)
{
    if (!double_buffer_enabled || !back_buffer_data) {
        gfx_color_alpha(r, g, b, a);
        gfx_fill_circle(x_center, y_center, radius_x * 2, radius_y * 2);
        return;
    }

    int x = 0, y = radius_y;
    long dx = 0, dy = 2 * radius_x * radius_x * y;
    long d1 = (radius_y * radius_y) - (radius_x * radius_x * radius_y) + (0.25 * radius_x * radius_x);
    int rx_sq = radius_x * radius_x;
    int ry_sq = radius_y * radius_y;

    while (dx < dy) {
        for (int i = x_center - x; i <= x_center + x; i++) {
            if (i >= 0 && i < window_width) {
                if ((y_center + y) >= 0 && (y_center + y) < window_height) blend_pixel(&back_buffer_data[((y_center + y) * window_width + i) * 4], r, g, b, a);
                if ((y_center - y) >= 0 && (y_center - y) < window_height) blend_pixel(&back_buffer_data[((y_center - y) * window_width + i) * 4], r, g, b, a);
            }
        }

        x++;
        dx += 2 * ry_sq;

        if (d1 < 0) {
            d1 += ry_sq + dx;
        } else {
            y--;
            dy -= 2 * rx_sq * 2;
            d1 += ry_sq + dx - dy;
        }
    }

    long d2 = ((double)ry_sq * (x + 0.5) * (x + 0.5)) + ((double)rx_sq * (y - 1) * (y - 1)) - ((long)rx_sq * ry_sq);

    while (y >= 0) {
        for (int i = x_center - x; i <= x_center + x; i++) {
            if (i >= 0 && i < window_width) {
                if ((y_center + y) >= 0 && (y_center + y) < window_height) blend_pixel(&back_buffer_data[((y_center + y) * window_width + i) * 4], r, g, b, a);
                if ((y_center - y) >= 0 && (y_center - y) < window_height) blend_pixel(&back_buffer_data[((y_center - y) * window_width + i) * 4], r, g, b, a);
            }
        }
        y--;
        dy -= 2 * rx_sq;
        if (d2 > 0) {
            d2 -= dy + rx_sq;
        } else {
            x++;
            dx += 2 * ry_sq;
            d2 -= dy + rx_sq + dx;
        }
    }
}

/* ====================================================================== */
/*                  XSHM SUPPORT FUNCTIONS SECTION                       */
/* ====================================================================== */
#ifdef USE_XSHM

static int gfx_double_buffer_init_xshm()
{
    if (!XShmQueryExtension(gfx_display)) {
        fprintf(stderr, "XSHM Extension not available.\n");
        return 0;
    }

    back_buffer = XShmCreateImage(gfx_display, gfx_visual, gfx_depth, ZPixmap, NULL, &shminfo, window_width, window_height);
    if (!back_buffer) {
        fprintf(stderr, "XShmCreateImage failed.\n");
        return 0;
    }

    shminfo.shmid = shmget(IPC_PRIVATE, back_buffer->bytes_per_line * back_buffer->height, IPC_CREAT | 0777);
    if (shminfo.shmid < 0) {
        perror("shmget failed");
        XDestroyImage(back_buffer);
        back_buffer = NULL;
        return 0;
    }

    shminfo.shmaddr = back_buffer->data = shmat(shminfo.shmid, 0, 0);
    if (shminfo.shmaddr == (char *) -1) {
        perror("shmat failed");
        shmctl(shminfo.shmid, IPC_RMID, 0);
        XDestroyImage(back_buffer);
        back_buffer = NULL;
        return 0;
    }

    shminfo.readOnly = False;
    if (!XShmAttach(gfx_display, &shminfo)) {
        fprintf(stderr, "XShmAttach failed.\n");
        shmdt(shminfo.shmaddr);
        shmctl(shminfo.shmid, IPC_RMID, 0);
        XDestroyImage(back_buffer);
        back_buffer = NULL;
        return 0;
    }

    back_buffer_data = (unsigned char *)back_buffer->data;
    return 1;
}

static void gfx_double_buffer_swap_xshm()
{
    if (back_buffer) {
        XShmPutImage(gfx_display, gfx_window, gfx_gc, back_buffer, 0, 0, 0, 0, window_width, window_height, False);
    }
}

static void gfx_double_buffer_cleanup_xshm()
{
    if (back_buffer) {
        XShmDetach(gfx_display, &shminfo);
        shmdt(shminfo.shmaddr);
        shmctl(shminfo.shmid, IPC_RMID, 0);
    }
}

#endif

/* ====================================================================== */
/*                  DOUBLE BUFFERING SUPPORT SECTION                      */
/* ====================================================================== */

/* Initialize double buffering, using XSHM if enabled and available. */
void gfx_double_buffer_init()
{
    if (double_buffer_enabled) {
        return;
    }

    use_shm = 0;

#ifdef USE_XSHM
    if (gfx_double_buffer_init_xshm()) {
        use_shm = 1;
        fprintf(stderr, "Using XSHM for double buffering.\n");
    } else {
        fprintf(stderr, "Falling back to non-XSHM mode.\n");
    }
#else
    fprintf(stderr, "XSHM not compiled in, using non-XSHM mode.\n");
#endif

    if (!use_shm) {
        back_buffer_data = (unsigned char *)malloc(window_width * window_height * 4);
        if (!back_buffer_data) {
            fprintf(stderr, "Failed to allocate memory for back buffer data.\n");
            return;
        }

        back_buffer = XCreateImage(gfx_display, gfx_visual, gfx_depth, ZPixmap, 0, (char *)back_buffer_data, window_width, window_height, 32, 0);
        if (!back_buffer) {
            fprintf(stderr, "Failed to create back buffer XImage.\n");
            free(back_buffer_data);
            back_buffer_data = NULL;
            return;
        }
    }

    if (back_buffer && (back_buffer->bits_per_pixel != 32 && back_buffer->bits_per_pixel != 24)) {
        fprintf(stderr, "Warning: Back buffer is not 24 or 32 bits per pixel, performance may be degraded.\n");
    }

    double_buffer_enabled = 1;
    gfx_double_buffer_clear(0, 0, 0);
}

/* Swap the back buffer to the display, using XSHM if enabled. */
void gfx_double_buffer_swap()
{
    if (!double_buffer_enabled || !back_buffer_data || !back_buffer) return;

    if (use_shm) {
#ifdef USE_XSHM
        gfx_double_buffer_swap_xshm();
#endif
    } else {
        int bytes_per_pixel = back_buffer->bits_per_pixel / 8;
        if (bytes_per_pixel < 3) bytes_per_pixel = 3;

        unsigned char *image_data = (unsigned char *)back_buffer->data;

        for (int y = 0; y < window_height; y++) {
            for (int x = 0; x < window_width; x++) {
                int rgba_index = (y * window_width + x) * 4;
                int image_index = (y * back_buffer->bytes_per_line) + (x * bytes_per_pixel);

                if (bytes_per_pixel == 4) {
                    image_data[image_index + 0] = back_buffer_data[rgba_index + 2]; // B
                    image_data[image_index + 1] = back_buffer_data[rgba_index + 1]; // G
                    image_data[image_index + 2] = back_buffer_data[rgba_index + 0]; // R
                    image_data[image_index + 3] = back_buffer_data[rgba_index + 3]; // A or padding
                } else if (bytes_per_pixel == 3) {
                    image_data[image_index + 0] = back_buffer_data[rgba_index + 2]; // B
                    image_data[image_index + 1] = back_buffer_data[rgba_index + 1]; // G
                    image_data[image_index + 2] = back_buffer_data[rgba_index + 0]; // R
                }
            }
        }
        XPutImage(gfx_display, gfx_window, gfx_gc, back_buffer, 0, 0, 0, 0, window_width, window_height);
    }
    XFlush(gfx_display);
}

/* Clear the back buffer to the specified color with alpha support. */
void gfx_double_buffer_clear(int r, int g, int b)
{
    if (double_buffer_enabled && back_buffer_data) {
        for (int i = 0; i < window_width * window_height * 4; i += 4) {
            back_buffer_data[i] = r;
            back_buffer_data[i + 1] = g;
            back_buffer_data[i + 2] = b;
            back_buffer_data[i + 3] = 255;
        }
    } else {
        gfx_clear_color(r, g, b);
        gfx_clear();
    }
}

/* Draw a point on the back buffer with alpha blending. */
void gfx_double_buffer_point(int x, int y, int r, int g, int b, int a)
{
    if (double_buffer_enabled && back_buffer_data && x >= 0 && x < window_width && y >= 0 && y < window_height) {
        int idx = (y * window_width + x) * 4;
        if (a == 255) {
            back_buffer_data[idx] = r;
            back_buffer_data[idx + 1] = g;
            back_buffer_data[idx + 2] = b;
            back_buffer_data[idx + 3] = 255;
        } else {
            blend_pixel(&back_buffer_data[idx], r, g, b, a);
        }
    } else {
        gfx_color(r, g, b);
        gfx_point(x, y);
    }
}

/* Draw a filled rectangle on the back buffer with alpha blending */
void gfx_double_buffer_fill_rectangle(int x, int y, int w, int h, int r, int g, int b, int a)
{
    if (!double_buffer_enabled || !back_buffer_data) {
        gfx_color_alpha(r, g, b, a);
        gfx_fill_rectangle(x, y, w, h);
        return;
    }

    int x_start = x < 0 ? 0 : x;
    int y_start = y < 0 ? 0 : y;
    int x_end = (x + w) > window_width ? window_width : (x + w);
    int y_end = (y + h) > window_height ? window_height : (y + h);

    if (a == 255) {
        for (int py = y_start; py < y_end; py++) {
            int idx = py * window_width * 4 + x_start * 4;
            for (int px = x_start; px < x_end; px++, idx += 4) {
                back_buffer_data[idx] = r;
                back_buffer_data[idx + 1] = g;
                back_buffer_data[idx + 2] = b;
                back_buffer_data[idx + 3] = 255;
            }
        }
    } else if (a > 0) {
        for (int py = y_start; py < y_end; py++) {
            int idx = py * window_width * 4 + x_start * 4;
            for (int px = x_start; px < x_end; px++, idx += 4) {
                blend_pixel(&back_buffer_data[idx], r, g, b, a);
            }
        }
    }
}

/* Draw a filled circle on the back buffer with alpha blending - Calls optimized version */
void gfx_double_buffer_fill_circle(int x_center, int y_center, int radius, int r, int g, int b, int a)
{
    gfx_double_buffer_fill_circle_alpha(x_center, y_center, radius, r, g, b, a);
}

/* Set the window title */
void gfx_set_title(const char *title)
{
    XStoreName(gfx_display, gfx_window, title);
}

/* Cleanup double buffering resources, including XSHM if used. */
void gfx_double_buffer_cleanup()
{
    if (back_buffer) {
#ifdef USE_XSHM
        if (use_shm) {
            gfx_double_buffer_cleanup_xshm();
        } else {
            free(back_buffer->data); // Free data only if not XSHM
        }
#else
        free(back_buffer->data); // Free data in non-XSHM mode
#endif
        XDestroyImage(back_buffer); // Correct function to free XImage
        back_buffer = NULL;
    }
    if (back_buffer_data && !use_shm) {
        free(back_buffer_data); // Free only if not managed by XSHM
        back_buffer_data = NULL;
    } else if (use_shm) {
        back_buffer_data = NULL; // Reset pointer, memory managed by XSHM
    }
    double_buffer_enabled = 0;
    use_shm = 0;
}

/* ====================================================================== */
/*                  END OF FILE                                          */
/* ====================================================================== */
