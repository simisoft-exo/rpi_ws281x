#ifndef __ANIMATIONS_H__
#define __ANIMATIONS_H__

#include <stdint.h>
#include <cairo/cairo.h>
#include "ws2811.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PI 3.14159265358979323846
#define HEX_HEIGHT_RATIO 0.8128988125
// full canvas dimensions
#define LUT_W 11
#define LUT_H 12
#define LUT_LEN (LUT_W * LUT_H)
#define __ -1// full canvas including missing pixels (marked as __)
// LUT currently nased of prototype (2:1 ratio, skipping every other pixel)
extern const int LUT[LUT_LEN];

enum AnimationType {
    GROWING_ELLIPSE,
    ROTATING_FRAMES,
    SURFACE_SPECTRUM,
    RANDOM,
    NONE  // Initially, no animation is set
};

typedef struct {
    cairo_surface_t **frames;
    int frame_count;
    int max_frames;
    int current_frame;
    int direction;
} AnimationContext;

extern AnimationContext anim_ctx;

// Utility functions
void send_frame_to_neopixels(cairo_surface_t *surface, ws2811_t *ledstring);
void smooth_interpolate_to_new_frames(AnimationContext *current_ctx, AnimationContext *new_ctx, AnimationContext *transition_ctx, int fps);
void clear_animation(AnimationContext *ctx);
void insert_frame_to_animation_context_at(AnimationContext *ctx, cairo_surface_t *frame, int index);

void smooth_interpolate_between_frames(
    cairo_surface_t *first_frame,
    cairo_surface_t *second_frame,
    AnimationContext *ctx,
    int fps);

// Animation frame functions
void draw_ellipse_frame(AnimationContext *ctx, double scale_factor);
void draw_rotating_pie_chart_frame(AnimationContext *ctx, double rotation_angle);

void draw_full_color_frame(AnimationContext *ctx, int r, int g, int b);
void draw_random_color_frame(AnimationContext *ctx);


//Animation sequence functions
void make_rotating_frames(AnimationContext *ctx, int num_frames);
void make_growing_ellipse(AnimationContext *ctx, int num_frames);
void make_color_spectrum(AnimationContext *ctx, int num_frames);
void make_random_color_sequence(AnimationContext *ctx, int num_frames, int fps);

#ifdef __cplusplus
}
#endif
#endif /* __ANIMATIONS_H__ */
