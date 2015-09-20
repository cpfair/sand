#include <pebble.h>
#include "common.h"
#include "icons.h"

#ifdef vibe_indicator
static inline void %hook(bt_connection_service) (bool connected) {
  if (!connected) vibes_double_pulse();
}
#endif

#ifdef visual_indicator
static inline void %hook(bt_connection_service) (bool connected) {
  if (content_layer) layer_mark_dirty(content_layer);
}

static inline void %hook(update_proc) (Layer* layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, colour_fg);
  graphics_context_set_stroke_color(ctx, colour_fg);
  if (!bluetooth_connection_service_peek()) {
    DRAW_BT_DISCONNECTED_SMALL(ctx, pos_x, pos_y);
  }
#ifdef visual_indicator_always_visible
  else {
    DRAW_BT_CONNECTED_SMALL(ctx,
                            pos_x + ICON_BT_DISCONNECTED_SMALL_WIDTH / 2 - ICON_BT_CONNECTED_SMALL_WIDTH / 2,
                            pos_y + ICON_BT_DISCONNECTED_SMALL_HEIGHT / 2 - ICON_BT_CONNECTED_SMALL_HEIGHT / 2);
  }
#endif
}
#endif