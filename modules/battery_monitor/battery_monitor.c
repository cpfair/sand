#include <pebble.h>
#include "common.h"
#include "icons.h"

#define IS_LOW(state) (state.charge_percent <= 20)

#ifdef visual_indicator
static inline void %hook(batt_state_service) (BatteryChargeState state) {
  if (content_layer) layer_mark_dirty(content_layer);
}

static inline void %hook(update_proc) (Layer* layer, GContext* ctx) {
  BatteryChargeState state = battery_state_service_peek();

#ifndef visual_indicator_always_visible
  if (!IS_LOW(state)) return;
  #define colour colour_fg_low
#else
  const GColor colour = IS_LOW(state) ? colour_fg_low : colour_fg;
#endif

  graphics_context_set_fill_color(ctx, colour);
  graphics_context_set_stroke_color(ctx, colour);


  DRAW_BATTERY_SMALL(ctx, pos_x, pos_y);
  graphics_fill_rect(ctx, GRect(pos_x + 2, pos_y + 2, state.charge_percent * 7 / 100, 3), 0, 0);
}
#endif
