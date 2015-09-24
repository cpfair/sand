#include <pebble.h>
#include "common.h"

#ifdef enabled
// This is slow, but seriously, who will actually use it for anything serious?
static inline void %hook(update_proc) (Layer* layer, GContext* ctx) {
  graphics_context_set_stroke_color(ctx, bezel_colour);
  const GRect layer_bounds = layer_get_bounds(layer);
  for (int y = 0; y < layer_bounds.size.h; y++) {
    for (int x = 0; x < layer_bounds.size.w; x++) {
      const int dx = x - 143/2;
      const int dy = y - 168/2;
      const int d = dx * dx + dy * dy;
      if (d >= 5184) {
        graphics_draw_pixel(ctx, (GPoint){.x=x, .y=y});
      }
    }
  }
}
#endif