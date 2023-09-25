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

void add_frame_to_animation_context(AnimationContext *ctx, cairo_surface_t *surface) {
    // Increment the frame count
    ++(ctx->frame_count);

    // Resize the array of frames
    ctx->frames = realloc(ctx->frames, ctx->frame_count * sizeof(cairo_surface_t*));
    if (ctx->frames == NULL) {
        // Handle memory allocation failure here
        exit(1);
    }

    // Save the new surface pointer into the frames array
    ctx->frames[ctx->frame_count - 1] = surface;
}

void draw_rotating_pie_chart_frame(AnimationContext *ctx, double rotation_angle) {
    const int width  = LUT_W;
    const int height = LUT_H;
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(surface);

    // Clear the background with black
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);

    // Move to the center of the image
    cairo_translate(cr, width / 2.0, height / 2.0);
    cairo_scale(cr, width , height / HEX_HEIGHT_RATIO);

    double angle_start = 0; // start angle in radians
    double angle_end = 0; // end angle in radians
    double slice_angle = 2 * PI / 6; // assuming 6 slices
    double radius = fmin(width, height) * 1.0;  // Adjust the size of the pie chart

    for (int i = 0; i < 6; i++) { // assuming 6 slices
        // Rotate the canvas by the rotation angle
        cairo_rotate(cr, rotation_angle);

        // Set the color for this slice
        cairo_set_source_rgb(cr, (i== 0 || i == 2), (i == 1 || i == 3 || i == 5), (i == 2 || i == 4));

        angle_end += slice_angle;

        // Draw the slice
        cairo_move_to(cr, 0, 0);
        cairo_arc(cr, 0, 0, radius, angle_start, angle_end);
        cairo_line_to(cr, 0, 0);
        cairo_fill(cr);

        // Rotate the canvas back by the negative of the rotation angle
        cairo_rotate(cr, -rotation_angle);

        angle_start = angle_end;
    }

    // Draw a small black circle at the center
    cairo_set_source_rgb(cr, 0, 0, 0);  // Black color
    cairo_arc(cr, 0, 0, 1 / width, 0, 2 * PI);  // Circle with 1-pixel radius
    cairo_fill(cr);

    // Save the surface to the dynamic array of frames
    add_frame_to_animation_context(ctx, surface);

    // Clean up
    cairo_destroy(cr);
}

void draw_side_wave_frame(AnimationContext *ctx, double wave_length, double up_or_down) {
    // Create a new Cairo surface and Cairo context (cr)
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, LUT_W, LUT_H);
    cairo_t *cr = cairo_create(surface);

    // Clear the background
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    cairo_paint(cr);

    // Translate and rotate the canvas
    cairo_translate(cr, LUT_W / 2.0, LUT_H / 2.0);  // Center the drawing
    cairo_rotate(cr, 30 * (PI / 180));  // Rotate 30 degrees

    // Initialize the wave variables
    double amplitude = 1;  // Amplitude of the wave
    double frequency = 0.5 * PI / wave_length;  // Frequency based on wave_length
    double phase = up_or_down;  // Phase shift based on up_or_down

    // Array of colors (RGBA) for the bands
    double colors[][4] = {
        {1, 0, 0, 1},
        {0, 1, 0, 1},
        {0, 0, 1, 1},
        {1, 1, 0, 1},
        {1, 0, 1, 1},
    };

    // Draw parallel bands with wave
    int num_bands = 5;
    for (int band = 0; band < num_bands; ++band) {
        // Set the drawing color for this band
        cairo_set_source_rgba(cr, colors[band][0], colors[band][1], colors[band][2], colors[band][3]);

        // Create the path for the wave in this band
        cairo_move_to(cr, -LUT_W / 2.0, band * 2);
        for (double x = -LUT_W / 2.0; x < LUT_W / 2.0; x += 1) {
            double y = band * 2 + amplitude * sin(frequency * x + phase);
            cairo_line_to(cr, x, y);
        }
        cairo_stroke(cr);  // Draw the wave
    }

    // Save the frame
    add_frame_to_animation_context(ctx, surface);

    // Cleanup
    cairo_destroy(cr);
}

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
    cairo_arc(cr, 0, 0, 1, 0, 2 * PI);
    cairo_fill(cr);

    // Save the surface to the dynamic array of frames
    add_frame_to_animation_context(ctx, surface);

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
               uint8_t r = (neopixel_color >> 16) & 0xFF;
               uint8_t g = (neopixel_color >> 8) & 0xFF;
               uint8_t b = neopixel_color & 0xFF;

            // Scale down to fit into ANSI 6x6x6 cube
               uint8_t scaled_r = r / 51;
               uint8_t scaled_g = g / 51;
               uint8_t scaled_b = b / 51;

            // Convert to ANSI index
               int ansi_index = 16 + (36 * scaled_r) + (6 * scaled_g) + scaled_b;

                // Assuming hexadecimal output and 8 characters wide field
                printf("\033[38;5;%dm%08X \033[0m", ansi_index, neopixel_color);
            } else {
                // Print 8 spaces to align
                printf("        ");
            }
        }
        printf("\n");  // Newline after each row
        printf("\n");  // Newline after each row
    }
}

void send_frame_to_neopixels(cairo_surface_t *surface, ws2811_t *ledstring) {
    unsigned char *data = cairo_image_surface_get_data(surface);
    int width  = cairo_image_surface_get_width(surface);
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

void make_rotating_frames(AnimationContext *ctx, int num_frames) {
    double max_angle = 1.5 * PI; // Maximum rotation angle
    double delta_angle = max_angle / (num_frames / 2); // Angle change per frame

    // Rotate from 0 to max_angle
    for (int i = 0; i < num_frames / 2; i++) {
        double rotation_angle = i * delta_angle;
        draw_rotating_pie_chart_frame(ctx, rotation_angle);
    }

    // Rotate back from max_angle to 0
    for (int i = 0; i < num_frames / 2; i++) {
        double rotation_angle = max_angle - i * delta_angle;
        draw_rotating_pie_chart_frame(ctx, rotation_angle);
    }
}

void make_growing_ellipse(AnimationContext *ctx, int num_frames) {
    // Grow ellipse from 0.1 to 1.0 scale
    for (int i = 0; i < num_frames / 2; i++) {
        double scale_factor = 0.1 + (0.9 * i) / (num_frames / 2);
        draw_ellipse_frame(ctx, scale_factor);
    }

    // Shrink ellipse from 1.0 back to 0.1 scale
    for (int i = 0; i < num_frames / 2; i++) {
        double scale_factor = 1.0 - (0.9 * i) / (num_frames / 2);
        draw_ellipse_frame(ctx, scale_factor);
    }
}

void make_side_waves(AnimationContext *ctx, int num_frames) {
    // Increase wave_length from 1 to 100 over num_frames / 2
    // Change direction at halfway point
    double wave_length_min = -50.0;
    double wave_length_max = 50.0;
    double direction = 1.0;

    for (int i = 0; i < num_frames; ++i) {
        double wave_length = wave_length_min + ((wave_length_max - wave_length_min) * i) / (num_frames - 1);
        draw_side_wave_frame(ctx, wave_length, direction);
    }
}

void smooth_interpolate_to_new_frames(
    AnimationContext *current_ctx,
    AnimationContext *new_ctx,
    AnimationContext *transition_ctx,
    int fps) {

 // Deallocate any dynamically allocated resources in transition_ctx

    // Get the current frame from the current context
    cairo_surface_t *current_surface = current_ctx->frames[current_ctx->current_frame];

    // Get the current frame from the new context (current frame might not be first if animation changed)
    cairo_surface_t *target_surface = new_ctx->frames[new_ctx->current_frame];

    unsigned char *current_data = cairo_image_surface_get_data(current_surface);
    unsigned char *target_data = cairo_image_surface_get_data(target_surface);

    // Loop through each frame to perform the interpolation
    for (int i = 1; i <= fps; i++) {
        // Calculate the weight for interpolation
        double weight = (double)i / fps;

        // Create a new surface for the interpolated frame
        cairo_surface_t *interpolated_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, LUT_W, LUT_H);
        cairo_t *cr = cairo_create(interpolated_surface);

        cairo_surface_flush(interpolated_surface);
        unsigned char *interpolated_data = cairo_image_surface_get_data(interpolated_surface);

        for (int y = 0; y < LUT_H; y++) {
            for (int x = 0; x < LUT_W; x++) {
                int offset = (y * LUT_W + x) * 4; // 4 bytes per pixel for ARGB

                unsigned char *current_pixel = current_data + offset;
                unsigned char *target_pixel = target_data + offset;

                unsigned char current_a = current_pixel[3];
                unsigned char current_r = current_pixel[2];
                unsigned char current_g = current_pixel[1];
                unsigned char current_b = current_pixel[0];

                unsigned char target_a = target_pixel[3];
                unsigned char target_r = target_pixel[2];
                unsigned char target_g = target_pixel[1];
                unsigned char target_b = target_pixel[0];

                // Interpolate each channel individually
                unsigned char interpolated_a = (unsigned char)(weight * target_a + (1 - weight) * current_a);
                unsigned char interpolated_r = (unsigned char)(weight * target_r + (1 - weight) * current_r);
                unsigned char interpolated_g = (unsigned char)(weight * target_g + (1 - weight) * current_g);
                unsigned char interpolated_b = (unsigned char)(weight * target_b + (1 - weight) * current_b);

                // Combine interpolated channels back into a single 32-bit ARGB color
                uint32_t interpolated_color = (interpolated_a << 24) | (interpolated_r << 16) | (interpolated_g << 8) | interpolated_b;

                // Set the interpolated color to the new cairo surface
                *(uint32_t *)(interpolated_data + offset) = interpolated_color;
            }
        }


        // Mark the surface as dirty as we have modified it at the pixel level
        cairo_surface_mark_dirty(interpolated_surface);

        // Save the interpolated frame
        add_frame_to_animation_context(transition_ctx, interpolated_surface);
        // Cleanup
        cairo_destroy(cr);
    }
}

void make_color_spectrum(AnimationContext *ctx, int num_frames) {
    for (int i = 0; i < num_frames; i++) {
        // Calculate the progress through the spectrum
        double progress = (double)i / num_frames;

        // Calculate r, g, b values based on progress.
        // This is a simplified example; you can use more complex color calculations.
        int r = (int)(sin(progress * 2 * M_PI + 0) * 127.5 + 127.5);
        int g = (int)(sin(progress * 2 * M_PI + 2) * 127.5 + 127.5);
        int b = (int)(sin(progress * 2 * M_PI + 4) * 127.5 + 127.5);

        // Draw the frame
        draw_full_color_frame(ctx, r, g, b);
    }
}

void draw_full_color_frame(AnimationContext *ctx, int r, int g, int b) {
    // Create a new Cairo surface for the frame
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, LUT_W, LUT_H);
    cairo_t *cr = cairo_create(surface);

    // Set the color
    cairo_set_source_rgb(cr, r / 255.0, g / 255.0, b / 255.0);  // Cairo expects color components to be in [0, 1]

    // Draw a rectangle to fill the entire surface
    cairo_rectangle(cr, 0, 0, LUT_W, LUT_H);
    cairo_fill(cr);

    // Add this frame to the animation context
    add_frame_to_animation_context(ctx, surface);

    // Clean up
    cairo_destroy(cr);
}

void draw_random_color_frame(AnimationContext *ctx) {
    // Create a new surface
    cairo_surface_t *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, LUT_W, LUT_H);
    cairo_surface_flush(surface);
    unsigned char *data = cairo_image_surface_get_data(surface);

    // Loop over each pixel
    for (int y = 0; y < LUT_H; y++) {
        for (int x = 0; x < LUT_W; x++) {
            // Generate random colors in the range [0, 255]
            int r = rand() % 256;
            int g = rand() % 256;
            int b = rand() % 256;

            // Apply the first rule: If all are greater than 0.5, set the smallest to 0
            if (r > 127 && g > 127 && b > 127) {
                if (r <= g && r <= b) r = 0;
                else if (g <= r && g <= b) g = 0;
                else b = 0;
            }

            // Apply the second rule: if all are close to each other, modify accordingly
            if (abs(r - g) < 50 && abs(r - b) < 50 && abs(g - b) < 50) {
                if (r <= g && r <= b) r = 0;
                else if (g <= r && g <= b) g = 0;
                else b = 0;

                if (r >= g && r >= b) r = 255;
                else if (g >= r && g >= b) g = 255;
                else b = 255;
            }

            // Compute the pixel offset
            int offset = (y * LUT_W + x) * 4; // 4 bytes per pixel for ARGB

            data[offset] = b;       // Blue channel
            data[offset + 1] = g;   // Green channel
            data[offset + 2] = r;   // Red channel
            data[offset + 3] = 255; // Alpha channel (fully opaque)
        }
    }

    // Mark the surface as dirty after manually changing pixels
    cairo_surface_mark_dirty(surface);

    // Add this frame to the context
    add_frame_to_animation_context(ctx, surface);

}

void smooth_interpolate_between_frames(
    cairo_surface_t *first_frame,
    cairo_surface_t *second_frame,
    AnimationContext *ctx,
    int fps) {

    // Get data for the first and second frames
    unsigned char *first_data = cairo_image_surface_get_data(first_frame);
    unsigned char *second_data = cairo_image_surface_get_data(second_frame);

    // Loop through each frame to perform the interpolation
    for (int i = 1; i <= fps; i++) {
        // Calculate the weight for interpolation
        double weight = (double)i / fps;

        // Create a new surface for the interpolated frame
        cairo_surface_t *interpolated_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, LUT_W, LUT_H);
        cairo_t *cr = cairo_create(interpolated_surface);

        cairo_surface_flush(interpolated_surface);
        unsigned char *interpolated_data = cairo_image_surface_get_data(interpolated_surface);

        for (int y = 0; y < LUT_H; y++) {
            for (int x = 0; x < LUT_W; x++) {
                int offset = (y * LUT_W + x) * 4; // 4 bytes per pixel for ARGB

                unsigned char *first_pixel = first_data + offset;
                unsigned char *second_pixel = second_data + offset;

                unsigned char first_a = first_pixel[3];
                unsigned char first_r = first_pixel[2];
                unsigned char first_g = first_pixel[1];
                unsigned char first_b = first_pixel[0];

                unsigned char second_a = second_pixel[3];
                unsigned char second_r = second_pixel[2];
                unsigned char second_g = second_pixel[1];
                unsigned char second_b = second_pixel[0];

                // Interpolate each channel individually
                unsigned char interpolated_a = (unsigned char)(weight * second_a + (1 - weight) * first_a);
                unsigned char interpolated_r = (unsigned char)(weight * second_r + (1 - weight) * first_r);
                unsigned char interpolated_g = (unsigned char)(weight * second_g + (1 - weight) * first_g);
                unsigned char interpolated_b = (unsigned char)(weight * second_b + (1 - weight) * first_b);

                // Combine interpolated channels back into a single 32-bit ARGB color
                uint32_t interpolated_color = (interpolated_a << 24) | (interpolated_r << 16) | (interpolated_g << 8) | interpolated_b;

                // Set the interpolated color to the new cairo surface
                *(uint32_t *)(interpolated_data + offset) = interpolated_color;
            }
        }

        // Mark the surface as dirty as we have modified it at the pixel level
        cairo_surface_mark_dirty(interpolated_surface);

        // Save the interpolated frame
        add_frame_to_animation_context(ctx, interpolated_surface);
        // Cleanup
        cairo_destroy(cr);
    }
}


void make_random_color_sequence(AnimationContext *ctx, int num_frames, int fps) {
    if (num_frames < 2) {
        // Not enough frames for interpolation
        return;
    }

    // Generate the first random frame
    draw_random_color_frame(ctx);

    for (int i = 1; i < num_frames; ++i) {
        // Generate the next random frame
        draw_random_color_frame(ctx);

        // Get the last two frames (the ones we just drew)
        cairo_surface_t *prev_surface = ctx->frames[ctx->frame_count - 2];
        cairo_surface_t *current_surface = ctx->frames[ctx->frame_count - 1];

        // Interpolate between these frames and add them to the context
        smooth_interpolate_between_frames(prev_surface, current_surface, ctx, fps);

        // At this point, the interpolated frames are added right after the two original frames in the context.
        // Therefore, the next iteration will correctly pick the newly added frame as the "previous" one for further interpolations.
    }
}

void clear_animation(AnimationContext *ctx) {
    for (int i = 0; i < ctx->frame_count; i++) {
        cairo_surface_destroy(ctx->frames[i]);
    }
    free(ctx->frames);
    ctx->frames = NULL;
    ctx->frame_count = 0;
    ctx->current_frame = 0;
    ctx->direction = 1;  // or whatever your initial direction is
}
