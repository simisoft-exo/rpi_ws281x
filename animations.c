#include "cairo.h"
#include "animations.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

const int LUT[LUT_LEN] = {
  __,__,__, 0,__, 1,__, 2,__,__,__,
  __,__, 6,__, 5,__, 4,__, 3,__,__,
  __,__, 7,__, 8,__, 9,__,10,__,__,
  __,15,__,14,__,13,__,12,__,11,__,
  __,16,__,17,__,18,__,19,__,20,__,
  26,__,25,__,24,__,23,__,22,__,21,
  27,__,28,__,29,__,30,__,31,__,32,
  __,37,__,36,__,35,__,34,__,33,__,
  __,38,__,39,__,40,__,41,__,42,__,
  __,__,46,__,45,__,44,__,43,__,__,
  __,__,47,__,48,__,49,__,50,__,__,
  __,__,__,53,__,52,__,51,__,__,__
 };

void draw_ellipse_frame(AnimationContext *ctx, double scale_factor) {
    const int width = LUT_W;
    const int height = LUT_H;
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(surface);

    // Clear the background with black
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    // Set the ellipse color
    cairo_set_source_rgb(cr, 1, 0, 0);

    // Move to the center of the image
    cairo_translate(cr, width / 2.0, height / 2.0);

    // Adjust scale based on the scale factor
    cairo_scale(cr, scale_factor * width , scale_factor * height / HEX_HEIGHT_RATIO);

    // Draw the ellipse
    cairo_arc(cr, 0, 0, 1, 0, 2 * 3.14159);
    cairo_fill(cr);

    // Save the surface to the dynamic array of frames
    ++(ctx->frame_count);  // Increment the frame count
    ctx->frames = realloc(ctx->frames, ctx->frame_count * sizeof(cairo_surface_t*));  // Resize the array
    ctx->frames[ctx->frame_count - 1] = surface;  // Save the surface pointer

    // Clean up
    cairo_destroy(cr);
}

uint32_t convert_argb_to_neopixel(uint32_t argb) {
    uint8_t a = 1;
    /* uint8_t a = (argb >> 24) & 0xFF; */
    uint8_t r = (argb >> 16) & 0xFF;
    uint8_t g = (argb >> 8) & 0xFF;
    uint8_t b = argb & 0xFF;

    return (a << 24) | (r << 16) | (g << 8) | b;
}

int lut_index(int x, int y, int width) {
    // Assuming the LUT is filled row by row
    return LUT[y * width + x];
}

void print_frame_as_table(int width, int height, cairo_surface_t *surface) {
    unsigned char *data = cairo_image_surface_get_data(surface);
 // Move the cursor to the top-left corner of the terminal
    printf("\033[H");
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int offset = (y * width + x) * 4;
            uint32_t argb = *(uint32_t *)(data + offset);
            uint32_t neopixel_color = convert_argb_to_neopixel(argb);
            int index = lut_index(x, y, width);

            if (index != __) {
                // Assuming hexadecimal output and 8 characters wide field
                printf("%08X ", neopixel_color);
            } else {
                // Print 8 spaces to align
                printf("        ");
            }
        }
        printf("\n");  // Newline after each row
    }
}

void send_frame_to_neopixels(cairo_surface_t *surface, ws2811_t *ledstring) {
    unsigned char *data = cairo_image_surface_get_data(surface);
    int width = cairo_image_surface_get_width(surface);
    int height = cairo_image_surface_get_height(surface);

    print_frame_as_table(width,height,surface);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int offset = (y * width + x) * 4;  // 4 bytes per pixel for ARGB
            uint32_t argb = *(uint32_t *)(data + offset);
            uint32_t neopixel_color = convert_argb_to_neopixel(argb);

            // Assuming you have a function to convert 2D coordinates to a 1D index
            int index = lut_index(x, y, width);

            // Send only when there is a pixel
            if (index != __)
            {
                ledstring->channel[0].leds[index] = neopixel_color;
            }
        }
    }

    ws2811_render(ledstring);
}
