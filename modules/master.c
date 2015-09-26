#include <pebble.h>

// The layer we use to apply all our visual modifications
static Layer* content_layer;

%module_code

#if defined(HOOK_UPDATE_PROC) || defined(HOOK_MODIFY_FRAMEBUFFER)

typedef struct ShadowWindowUserData {
  void* their_user_data;
  WindowHandler their_window_appear_handler;
} ShadowWindowUserData;


void master_update_proc(Layer* layer, GContext* ctx) {
  // For on-screen additions
  HOOK_UPDATE_PROC_CALLS(layer, ctx)
  // For colourization, flipping, etc.
#ifdef HOOK_MODIFY_FRAMEBUFFER
  GRect layer_bounds = layer_get_frame(window_get_root_layer(window_stack_get_top_window()));

  GBitmap* framebuffer_bmp = graphics_capture_frame_buffer(ctx);
  GRect framebuffer_bounds = gbitmap_get_bounds(framebuffer_bmp);

  #ifdef PBL_COLOR
  // This is a fixup for Aplite apps with a statusbar running on Basalt
  // It seems there are some shenanigans going on - the height of the window is reduced - but it's not shifted down
  // (the entire framebuffer is writable - just the top section gets pasted over with the status bar at some later point)
  // So something, somewhere is telling the graphics calls to shift things down - maybe I'm just missing something
  // In the meantime - this.
  layer_bounds.origin.y += framebuffer_bounds.size.h - layer_bounds.size.h;
  #endif

  // We want a GRect that falls entirely in the framebuffer - the layer frame extends outside of it during compositor animations
  GRect drawable_bounds = layer_bounds;
  if (drawable_bounds.origin.x < 0) {
    drawable_bounds.size.w += drawable_bounds.origin.x;
    drawable_bounds.origin.x = 0;
  } else if (drawable_bounds.origin.x > (framebuffer_bounds.origin.x + framebuffer_bounds.size.w)) {
    drawable_bounds = GRectZero;
  }
  if (drawable_bounds.origin.y < 0) {
    drawable_bounds.size.h += drawable_bounds.origin.y;
    drawable_bounds.origin.y = 0;
  }  else if (drawable_bounds.origin.y > (framebuffer_bounds.origin.y + framebuffer_bounds.size.h)) {
    drawable_bounds = GRectZero;
  }
  if (drawable_bounds.origin.x + drawable_bounds.size.w > framebuffer_bounds.size.w) {
    drawable_bounds.size.w = framebuffer_bounds.size.w - drawable_bounds.origin.x;
  }
  if (drawable_bounds.origin.y + drawable_bounds.size.h > framebuffer_bounds.size.h) {
    drawable_bounds.size.h = framebuffer_bounds.size.h - drawable_bounds.origin.y;
  }

  uint8_t* framebuffer = gbitmap_get_data((GBitmap*)ctx);
  HOOK_MODIFY_FRAMEBUFFER_CALLS(layer_bounds, drawable_bounds, framebuffer_bmp, framebuffer);

  graphics_release_frame_buffer(ctx, framebuffer_bmp);
#endif
}

// We go through all sorts of acrobatics to keep our content_layer visible at all times
// We need to use the window_handlers facility - but of course, they probably need to, too
// So, we proxy all the events to their handlers
// And, do do that, we need a place to store their handlers on a per-Window basis
// So we need to co-opt the user_data stuff, too

void* window_get_user_data__patch(Window *window) {
  ShadowWindowUserData* shadow_data = window_get_user_data(window);
  return shadow_data->their_user_data;
}

void window_set_user_data__patch(Window *window, void* user_data) {
  ShadowWindowUserData* shadow_data = window_get_user_data(window);
  shadow_data->their_user_data = user_data;
}

static void window_appear(Window *window) {
  ShadowWindowUserData* shadow_data = window_get_user_data(window);
  if (shadow_data->their_window_appear_handler) shadow_data->their_window_appear_handler(window);
  const Layer* const root_layer = window_get_root_layer(window);

  // Whenever a window appears, we paste our stuff over top of it
  const GRect parent_bounds = layer_get_bounds(root_layer);
  if (!content_layer) {
    content_layer = layer_create(parent_bounds);
    layer_set_update_proc(content_layer, master_update_proc);
  }
  // Make sure we're always matching the size of the window (status bars, etc)
  // This is a no-op the first time they push a window
  layer_set_bounds(content_layer, parent_bounds);

  layer_add_child(root_layer, content_layer);
}

void window_set_window_handlers__patch(Window *window, WindowHandlers handlers) {
  ShadowWindowUserData* shadow_data = window_get_user_data(window);
  // Stop them from overwriting our appear handler
  shadow_data->their_window_appear_handler = handlers.appear;
  handlers.appear = window_appear;
  // ...but pass the rest through
  window_set_window_handlers(window, handlers);
}

// We install the appear handler immediately, and set up the ShadowWindowUserData object that keeps this crazy train on the tracks
Window* window_create__patch(void) {
  Window* window = window_create();
  ShadowWindowUserData* shadow_data = malloc(sizeof(ShadowWindowUserData));
  memset(shadow_data, 0, sizeof(ShadowWindowUserData));
  window_set_user_data(window, shadow_data);
  window_set_window_handlers(window, (WindowHandlers) {
    .appear = window_appear
  });
  return window;
}

void window_destroy__patch(Window* window) {
  free(window_get_user_data(window));
  window_destroy(window);
}

// Keep our stuff on top
void layer_add_child__patch(Layer* parent, Layer* child) {
  layer_add_child(parent, child);
  if (parent == window_get_root_layer(window_stack_get_top_window()) && content_layer) {
    layer_add_child(parent, content_layer);
  }
}

#endif

#ifdef HOOK_BT_CONNECTION_SERVICE
static BluetoothConnectionHandler their_bt_connection_handler;

static void bluetooth_callback(bool connected) {
  HOOK_BT_CONNECTION_SERVICE_CALLS(connected)
  if (their_bt_connection_handler) their_bt_connection_handler(connected);
}

void bluetooth_connection_service_subscribe__patch(BluetoothConnectionHandler handler) {
  their_bt_connection_handler = handler;
}

void bluetooth_connection_service_unsubscribe__patch(void) {
  their_bt_connection_handler = NULL;
}
#endif


#ifdef HOOK_BATT_STATE_SERVICE
static BatteryStateHandler their_batt_state_handler;

static void battery_callback(BatteryChargeState state) {
  HOOK_BATT_STATE_SERVICE_CALLS(state)
  if (their_batt_state_handler) their_batt_state_handler(state);
}

void battery_state_service_subscribe__patch(BatteryStateHandler handler) {
  their_batt_state_handler = handler;
}

void battery_state_service_unsubscribe__patch(void) {
  their_batt_state_handler = NULL;
}
#endif

// Common startup
#if defined(HOOK_BT_CONNECTION_SERVICE) || defined(HOOK_BATT_STATE_SERVICE) || defined(HOOK_STARTUP)
void app_event_loop__patch(void) {
  #ifdef HOOK_BT_CONNECTION_SERVICE
  bluetooth_connection_service_subscribe(bluetooth_callback);
  #endif
  #ifdef HOOK_BATT_STATE_SERVICE
  battery_state_service_subscribe(battery_callback);
  #endif
  HOOK_STARTUP_CALLS()
  app_event_loop();
}
#endif
