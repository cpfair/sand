#include <pebble.h>
#ifndef _hook_0
#define _hook_0(...)
#endif
#ifndef _hook_1
#define _hook_1(...)
#endif
#define HOOK_BT_CONNECTION_SERVICE_CALLS(...) _hook_0(__VA_ARGS__)_hook_1(__VA_ARGS__)
#ifndef _hook_2
#define _hook_2(...)
#endif
#define HOOK_UPDATE_PROC_CALLS(...) _hook_2(__VA_ARGS__)

// These are always included, but get optimized out if not used.
static Layer* content_layer;

#define pos_y 10
#define pos_x 10
#define vibe_indicator True
#define colour_fg GColorWhite
#include <pebble.h>
#include "icons.h"

#ifdef vibe_indicator
#define _hook_0(...) hook_0(__VA_ARGS__);
#define HOOK_BT_CONNECTION_SERVICE
inline void hook_0 (bool connected) {
  if (!connected) vibes_double_pulse();
}
#endif

#ifdef visual_indicator
#define _hook_1(...) hook_1(__VA_ARGS__);
#define HOOK_BT_CONNECTION_SERVICE
inline void hook_1 (bool connected) {
  layer_mark_dirty(content_layer);
}

#define _hook_2(...) hook_2(__VA_ARGS__);
#define HOOK_UPDATE_PROC
inline void hook_2 (Layer* layer, GContext* ctx) {
  DRAW_BT_CONNECTED_SMALL(ctx, pos_x, pos_y);
}
#endif
#undef pos_y
#undef visual_indicator
#undef pos_x
#undef vibe_indicator
#undef colour_fg

#ifdef HOOK_UPDATE_PROC

void master_update_proc(Layer* layer, GContext* ctx) {
  HOOK_UPDATE_PROC_CALLS(layer, ctx)
}

void window_stack_push__patch(Window *window, bool animated) {
  if (!content_layer) {
    content_layer = layer_create(GRect(0, 0, 144, 168));
    layer_set_update_proc(content_layer, master_update_proc);
  }
  layer_add_child(window_get_root_layer(window), content_layer);
  window_stack_push(window, animated);
}

void layer_add_child__patch(Layer* parent, Layer* child) {
  layer_add_child(parent, child);
  if (parent == window_get_root_layer(window_stack_get_top_window())) {
    // Keep our stuff on top
    layer_add_child(parent, content_layer);
  }
}

#endif

#ifdef HOOK_BT_CONNECTION_SERVICE
static BluetoothConnectionHandler their_bt_status_handler;

static void bluetooth_callback(bool connected) {
  HOOK_BT_CONNECTION_SERVICE_CALLS(connected)
  if (their_bt_status_handler) their_bt_status_handler(connected);
}

void bluetooth_connection_service_subscribe__patch(BluetoothConnectionHandler handler) {
  their_bt_status_handler = handler;
}

void bluetooth_connection_service_unsubscribe__patch(void) {
  their_bt_status_handler = NULL;
}
#endif


// Common startup
void app_event_loop__patch(void) {
  #ifdef HOOK_BT_CONNECTION_SERVICE
  bluetooth_connection_service_subscribe(bluetooth_callback);
  #endif
  app_event_loop();
}

