#ifndef __ANIMATIONS_H__
#define __ANIMATIONS_H__

#include <stdint.h>
#include <cairo/cairo.h>
#include "ws2811.h"

#ifdef __cplusplus
extern "C" {
#endif


//Stuff here

#define HEX_HEIGHT_RATIO 0.8128988125
// full canvas dimensions
#define LUT_W 11
#define LUT_H 12
#define LUT_LEN (LUT_W * LUT_H)
#define __ -1// full canvas including missing pixels (marked as __)
// LUT currently nased of prototype (2:1 ratio, skipping every other pixel)
extern const int LUT[LUT_LEN];

typedef struct {
    cairo_surface_t **frames;
    int frame_count;
    int current_frame;
    int direction;
} AnimationContext;

extern AnimationContext anim_ctx;
uint32_t convert_argb_to_neopixel(uint32_t argb);
void send_frame_to_neopixels(cairo_surface_t *surface, ws2811_t *ledstring);
void draw_ellipse_frame(AnimationContext *ctx, double scale_factor);
void draw_hexagon_frame(AnimationContext *ctx, double scale_factor);

#ifdef __cplusplus
}
#endif
#endif /* __ANIMATIONS_H__ */
