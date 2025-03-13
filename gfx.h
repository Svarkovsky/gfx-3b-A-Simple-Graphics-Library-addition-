/*
    A simple graphics library for CSE 20211 by Douglas Thain
    For complete documentation, see:
    http://www.nd.edu/~dthain/courses/cse20211/fall2011/gfx
*/

/*
	Copyright (c) 2019 Ivan Svarkovsky Version 3a
	06/19/2019 - Expanded and supplemented
*/

/*
    Copyright (c) 2024 Ivan Svarkovsky Version 3b
    06/04/2024 - Added a new section for alpha channel support functions (non-destructive addition)
	06/04/2024 - Added a new section for double buffering support functions (non-destructive addition)
	06/05/2024 - Rewrote double buffering for efficiency and to fix flickering/artifacts.
	06/06/2024 - Re-added XSHM support with backward compatibility.
	06/06/2024 - Refactored XSHM to separate functions for better backward compatibility and conditional compilation.
    06/06/2024 - Optimized gfx_double_buffer_fill_circle and gfx_double_buffer_fill_polygon for faster rendering.
    06/06/2024 - Optimized gfx_double_buffer_fill_ellipse using Midpoint Ellipse Algorithm.
    06/06/2024 - Replaced bubble sort in gfx_double_buffer_fill_polygon with qsort for intersection sorting.
*/


#ifndef _GFX_H_
#define _GFX_H_

/* ====================================================================== */
/*                  BASIC GRAPHICS FUNCTIONS DECLARATIONS                */
/* ====================================================================== */

/**
 * @brief Open a graphics window with specified width, height, and title.
 *
 * @param width  The width of the graphics window in pixels.
 * @param height The height of the graphics window in pixels.
 * @param title  The title to be displayed in the window's title bar.
 */
void gfx_open(int width, int height, const char *title);

/**
 * @brief Draw a single point at coordinates (x, y).
 *
 * @param x The x-coordinate of the point.
 * @param y The y-coordinate of the point.
 */
void gfx_point(int x, int y);

/**
 * @brief Draw a line from (x1, y1) to (x2, y2).
 *
 * @param x1 The x-coordinate of the starting point of the line.
 * @param y1 The y-coordinate of the starting point of the line.
 * @param x2 The x-coordinate of the ending point of the line.
 * @param y2 The y-coordinate of the ending point of the line.
 */
void gfx_line(int x1, int y1, int x2, int y2);

/**
 * @brief Draw a text string at coordinates (x, y).
 *
 * @param x The x-coordinate where the string will start.
 * @param y The y-coordinate where the string will start (baseline).
 * @param s The string to be drawn.
 */
void gfx_string(int x, int y, const char *s);

/**
 * @brief Get the width of a text string in pixels using the current font.
 *
 * @param cc The null-terminated string to measure.
 * @return The width of the string in pixels. Returns 0 if gfx_display is not initialized.
 */
int gfx_textwidth(const char *cc);

/**
 * @brief Get the last error message. (Currently a placeholder)
 *
 * @return An empty string.  This function is a placeholder and does not yet implement error reporting.
 */
const char* gfx_get_error();


/**
 * @brief Draw a circle with center (x, y), width, and height (bounding box).
 *
 * @param x     The x-coordinate of the center of the circle.
 * @param y     The y-coordinate of the center of the circle.
 * @param width The width of the bounding box of the circle (horizontal diameter).
 * @param height The height of the bounding box of the circle (vertical diameter).
 */
void gfx_circle(int x, int y, int width, int height);

/**
 * @brief Draw a filled circle with center (x, y), width, and height (bounding box).
 *
 * @param x     The x-coordinate of the center of the circle.
 * @param y     The y-coordinate of the center of the circle.
 * @param width The width of the bounding box of the circle (horizontal diameter).
 * @param height The height of the bounding box of the circle (vertical diameter).
 */
void gfx_fill_circle(int x, int y, int width, int height);

/**
 * @brief Draw a rectangle with top-left corner (x, y), width, and height.
 *
 * @param x     The x-coordinate of the top-left corner of the rectangle.
 * @param y     The y-coordinate of the top-left corner of the rectangle.
 * @param width The width of the rectangle.
 * @param height The height of the rectangle.
 */
void gfx_rectangle(int x, int y, int width, int height);

/**
 * @brief Draw a filled rectangle with top-left corner (x, y), width, and height.
 *
 * @param x     The x-coordinate of the top-left corner of the filled rectangle.
 * @param y     The y-coordinate of the top-left corner of the filled rectangle.
 * @param width The width of the filled rectangle.
 * @param height The height of the filled rectangle.
 */
void gfx_fill_rectangle(int x, int y, int width, int height);

/**
 * @brief Set the current drawing color using RGB values (0-255).
 *
 * @param r The red component of the color (0-255).
 * @param g The green component of the color (0-255).
 * @param b The blue component of the color (0-255).
 */
void gfx_color(int r, int g, int b);

/**
 * @brief Clear the graphics window to the current background color.
 */
void gfx_clear();

/**
 * @brief Set the background color for clearing the window using RGB values (0-255).
 *
 * @param r The red component of the background color (0-255).
 * @param g The green component of the background color (0-255).
 * @param b The blue component of the background color (0-255).
 */
void gfx_clear_color(int r, int g, int b);

/**
 * @brief Check if an event (keypress or mouse button press) is waiting in the event queue.
 *
 * @return 1 if an event is waiting, 0 otherwise.
 */
int gfx_event_waiting();

/**
 * @brief Wait for the next event (keypress or mouse button press) and return the character of the key pressed or the button number.
 *
 * @return The character of the key pressed, or the button number for a mouse button press.
 *         Returns a KeySym for key presses and button number for button presses.
 */
char gfx_wait();

/**
 * @brief Get the X coordinate of the last event (keypress or mouse button press).
 *
 * @return The X coordinate of the last event.
 */
int gfx_xpos();

/**
 * @brief Get the Y coordinate of the last event (keypress or mouse button press).
 *
 * @return The Y coordinate of the last event.
 */
int gfx_ypos();

/**
 * @brief Get the width of the graphics window.
 *
 * @return The width of the graphics window in pixels.
 */
int gfx_xsize();

/**
 * @brief Get the height of the graphics window.
 *
 * @return The height of the graphics window in pixels.
 */
int gfx_ysize();

/**
 * @brief Flush all pending drawing operations to the window to make them visible.
 *        This ensures that all drawing commands issued so far are actually displayed.
 */
void gfx_flush();

/**
 * @brief Get the RGB color value of a pixel at (x, y) in the window.
 *
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @return An integer representing the RGB color of the pixel.
 *         The color is packed as 0xRRGGBB.
 */
int GetPix(int x, int y);

/**
 * @brief Read a key press event and return the KeySym. Waits for a key press if no event is pending.
 *
 * @return The KeySym of the pressed key.
 */
int gfx_xreadkeys();

/**
 * @brief Read a key press event only if an event is already pending, otherwise returns -1. Non-blocking read.
 *
 * @return The KeySym of the pressed key if an event is pending and it's a key press, otherwise -1.
 */
int gfx_m_xreadkeys();

/**
 * @brief Move the graphics window to the left by a specified distance in steps with a delay between steps.
 *
 * @param x        Current x-coordinate of the window.
 * @param y        Current y-coordinate of the window.
 * @param distance Total distance to move the window to the left in pixels.
 * @param delay    Delay in microseconds between each step (e.g., 16666 for approximately 60 FPS).
 * @param step     Number of pixels to move the window in each step.
 * @return The final x-coordinate of the window after moving.
 */
int gfx_move_win_l(int x, int y, int distance, int delay, int step);

/**
 * @brief Move the graphics window down by a specified distance in steps with a delay between steps.
 *
 * @param x        Current x-coordinate of the window.
 * @param y        Current y-coordinate of the window.
 * @param distance Total distance to move the window down in pixels.
 * @param delay    Delay in microseconds between each step (e.g., 16666 for approximately 60 FPS).
 * @param step     Number of pixels to move the window in each step.
 * @return The final y-coordinate of the window after moving.
 */
int gfx_move_win_d(int x, int y, int distance, int delay, int step);

/**
 * @brief Move the graphics window to the right by a specified distance in steps with a delay between steps.
 *
 * @param x        Current x-coordinate of the window.
 * @param y        Current y-coordinate of the window.
 * @param distance Total distance to move the window to the right in pixels.
 * @param delay    Delay in microseconds between each step (e.g., 16666 for approximately 60 FPS).
 * @param step     Number of pixels to move the window in each step.
 * @return The final x-coordinate of the window after moving.
 */
int gfx_move_win_r(int x, int y, int distance, int delay, int step);

/**
 * @brief Move the graphics window up by a specified distance in steps with a delay between steps.
 *
 * @param x        Current x-coordinate of the window.
 * @param y        Current y-coordinate of the window.
 * @param distance Total distance to move the window up in pixels.
 * @param delay    Delay in microseconds between each step (e.g., 16666 for approximately 60 FPS).
 * @param step     Number of pixels to move the window in each step.
 * @return The final y-coordinate of the window after moving.
 */
int gfx_move_win_u(int x, int y, int distance, int delay, int step);


/* ====================================================================== */
/*                  ALPHA CHANNEL SUPPORT FUNCTIONS DECLARATIONS         */
/* ====================================================================== */

/**
 * @brief Set the current drawing color with alpha transparency using RGBA values (0-255 for R, G, B, A).
 *
 * @param r The red component of the color (0-255).
 * @param g The green component of the color (0-255).
 * @param b The blue component of the color (0-255).
 * @param a The alpha component of the color (0-255), where 255 is opaque and 0 is fully transparent.
 */
void gfx_color_alpha(int r, int g, int b, int a);

/**
 * @brief Draw a filled polygon on the back buffer with alpha blending.
 *
 * @param x_points  Array of x-coordinates of the polygon vertices.
 * @param y_points  Array of y-coordinates of the polygon vertices.
 * @param num_points Number of vertices in the polygon.
 * @param r         Red color component (0-255).
 * @param g         Green color component (0-255).
 * @param b         Blue color component (0-255).
 * @param a         Alpha component (0-255).
 */
void gfx_double_buffer_fill_polygon(int *x_points, int *y_points, int num_points, int r, int g, int b, int a);

/**
 * @brief Draw a filled ellipse on the back buffer with alpha blending.
 *
 * @param x_center  X-coordinate of the center of the ellipse.
 * @param y_center  Y-coordinate of the center of the ellipse.
 * @param radius_x  Horizontal radius of the ellipse.
 * @param radius_y  Vertical radius of the ellipse.
 * @param r         Red color component (0-255).
 * @param g         Green color component (0-255).
 * @param b         Blue color component (0-255).
 * @param a         Alpha component (0-255).
 */
void gfx_double_buffer_fill_ellipse(int x_center, int y_center, int radius_x, int radius_y, int r, int g, int b, int a);

/**
 * @brief Draw a filled circle on the back buffer with alpha blending - OPTIMIZED version with alpha.
 *
 * @param x_center X-coordinate of the center of the circle.
 * @param y_center Y-coordinate of the center of the circle.
 * @param radius   Radius of the circle.
 * @param r        Red color component (0-255).
 * @param g        Green color component (0-255).
 * @param b        Blue color component (0-255).
 * @param a        Alpha component (0-255).
 */
void gfx_double_buffer_fill_circle_alpha(int x_center, int y_center, int radius, int r, int g, int b, int a);


/* ====================================================================== */
/*                  XSHM SUPPORT FUNCTIONS DECLARATIONS                  */
/* ====================================================================== */
#ifdef USE_XSHM
/**
 * @brief Initialize XSHM for double buffering. Internal function.
 *        Attempts to initialize X Shared Memory Extension for faster image transfers.
 * @return 1 on success (XSHM initialized), 0 on failure (fallback to non-SHM mode).
 */
static int gfx_double_buffer_init_xshm();
/**
 * @brief Swap back buffer to the window using XSHM. Internal function.
 *        Uses X Shared Memory Extension for faster buffer swapping.
 */
static void gfx_double_buffer_swap_xshm();
/**
 * @brief Cleanup XSHM resources. Internal function.
 *        Releases shared memory and related resources.
 */
static void gfx_double_buffer_cleanup_xshm();
#endif


/* ====================================================================== */
/*                  DOUBLE BUFFERING SUPPORT FUNCTIONS DECLARATIONS      */
/* ====================================================================== */

/**
 * @brief Initialize double buffering.
 *        If compiled with USE_XSHM and XSHM is available, it will use XSHM for faster performance.
 */
void gfx_double_buffer_init();

/**
 * @brief Swap the back buffer to the front buffer (visible window).
 *        If using XSHM, it will use XSHM for swapping.
 */
void gfx_double_buffer_swap();

/**
 * @brief Clear the back buffer to the specified RGB color.
 *
 * @param r Red color component (0-255).
 * @param g Green color component (0-255).
 * @param b Blue color component (0-255).
 */
void gfx_double_buffer_clear(int r, int g, int b);

/**
 * @brief Draw a point on the back buffer with alpha blending.
 *
 * @param x X-coordinate of the point.
 * @param y Y-coordinate of the point.
 * @param r Red color component (0-255).
 * @param g Green color component (0-255).
 * @param b Blue color component (0-255).
 * @param a Alpha component (0-255).
 */
void gfx_double_buffer_point(int x, int y, int r, int g, int b, int a);

/**
 * @brief Draw a filled rectangle on the back buffer with alpha blending.
 *
 * @param x X-coordinate of the top-left corner.
 * @param y Y-coordinate of the top-left corner.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @param r Red color component (0-255).
 * @param g Green color component (0-255).
 * @param b Blue color component (0-255).
 * @param a Alpha component (0-255).
 */
void gfx_double_buffer_fill_rectangle(int x, int y, int w, int h, int r, int g, int b, int a);

/**
 * @brief Draw a filled circle on the back buffer with alpha blending. Calls optimized version.
 *
 * @param x_center X-coordinate of the center of the circle.
 * @param y_center Y-coordinate of the center of the circle.
 * @param radius   Radius of the circle.
 * @param r        Red color component (0-255).
 * @param g        Green color component (0-255).
 * @param b        Blue color component (0-255).
 * @param a        Alpha component (0-255).
 */
void gfx_double_buffer_fill_circle(int x_center, int y_center, int radius, int r, int g, int b, int a);

/**
 * @brief Set the title of the graphics window.
 *
 * @param title The title string to set for the window.
 */
void gfx_set_title(const char *title);

/**
 * @brief Cleanup double buffering resources.
 *        If using XSHM, it will also cleanup XSHM resources. Should be called at the end of the program.
 */
void gfx_double_buffer_cleanup();

#endif /* _GFX_H_ */

