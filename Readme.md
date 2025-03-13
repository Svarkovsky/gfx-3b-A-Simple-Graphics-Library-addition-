# gfx_library

A simple C graphics library designed for educational purposes and creative projects on X11 systems. This library provides a straightforward interface for basic graphics operations, making it easy to draw shapes, text, and perform pixel manipulations in a window. Version 3b expands on the original library with features like alpha blending and double buffering for more advanced visual effects.

## Features

- **Basic Drawing Primitives:**
    - Points (`gfx_point`)
    - Lines (`gfx_line`)
    - Circles and Filled Circles (`gfx_circle`, `gfx_fill_circle`)
    - Rectangles and Filled Rectangles (`gfx_rectangle`, `gfx_fill_rectangle`)
    - Ellipses and Filled Ellipses (`gfx_double_buffer_fill_ellipse`)
    - Polygons and Filled Polygons (`gfx_double_buffer_fill_polygon`)
- **Text Rendering:**
    - Draw text strings (`gfx_string`)
    - Get text width in pixels (`gfx_textwidth`)
- **Color Control:**
    - Set drawing color using RGB (`gfx_color`)
    - Set drawing color with alpha transparency RGBA (`gfx_color_alpha`)
    - Set clear color (`gfx_clear_color`)
- **Window Management:**
    - Open a graphics window (`gfx_open`)
    - Clear the window (`gfx_clear`)
    - Set window title (`gfx_set_title`)
    - Move window (`gfx_move_win_l`, `gfx_move_win_d`, `gfx_move_win_r`, `gfx_move_win_u`)
    - Get window dimensions (`gfx_xsize`, `gfx_ysize`)
- **Event Handling:**
    - Check for event waiting (`gfx_event_waiting`)
    - Wait for event (key press or mouse button) (`gfx_wait`)
    - Get event coordinates (`gfx_xpos`, `gfx_ypos`)
    - Read key presses (`gfx_xreadkeys`, `gfx_m_xreadkeys`)
- **Pixel Manipulation:**
    - Get pixel color (`GetPix`)
- **Double Buffering:**
    - Initialize double buffering (`gfx_double_buffer_init`)
    - Swap buffers for smooth animation (`gfx_double_buffer_swap`)
    - Clear back buffer (`gfx_double_buffer_clear`)
    - Draw primitives to back buffer with alpha support (`gfx_double_buffer_point`, `gfx_double_buffer_fill_rectangle`, `gfx_double_buffer_fill_circle`, `gfx_double_buffer_fill_ellipse`, `gfx_double_buffer_fill_polygon`)
    - Cleanup double buffering resources (`gfx_double_buffer_cleanup`)
- **Alpha Blending Support:** For semi-transparent graphics.
- **XSHM Support (Optional):** For potentially faster double buffering using X Shared Memory Extension (can be enabled during compilation).

## Authors

- **Douglas Thain:** Original author of the initial versions of the library for educational purposes (CSE 20211).
- **Ivan Svarkovsky:** Expanded and modernized the library, adding alpha channel and double buffering support, optimizing performance, and refactoring for better compatibility (Versions 3a and 3b).

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Version 3b - Usable with Potential Improvements

This is version 3b of the `gfx_library`. It is functional and can be used for creating graphics applications. However, it is important to note that this version may contain imperfections and areas for improvement.

**Contributions are Welcome!**

If you find bugs, have suggestions for improvements, or want to contribute to the library, pull requests are highly encouraged.  This version was developed with expanded functionality in mind, and real-world usage and feedback are essential for further refinement and bug fixing. Please feel free to contribute to make this library even better!
