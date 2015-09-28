#include <pebble.h>

#if defined(PBL_COLOR) && defined(enabled)

// The graphics context's state gets reset between layers, so we can't just set it once
void graphics_context_set_fill_color__patch(GContext * ctx, GColor color) {
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_fill_color(ctx, color);
}

void graphics_context_set_stroke_color__patch(GContext * ctx, GColor color) {
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color(ctx, color);
}

void graphics_context_set_stroke_color_2bit__patch(GContext * ctx, GColor color) {
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_stroke_color_2bit(ctx, color);
}

void graphics_context_set_fill_color_2bit__patch(GContext * ctx, GColor color) {
  graphics_context_set_antialiased(ctx, true);
  graphics_context_set_fill_color_2bit(ctx, color);
}

#endif
