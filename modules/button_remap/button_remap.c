#include <pebble.h>

// The code generator doesn't define falsy config values
#ifndef remap_back
#define remap_back 0
#endif

// Needed so the conditional wrapping the rest of the code evaluates properly
#define BUTTON_ID_BACK 0
#define BUTTON_ID_UP 1
#define BUTTON_ID_SELECT 2
#define BUTTON_ID_DOWN 3

#if remap_down != BUTTON_ID_DOWN || remap_up != BUTTON_ID_UP || remap_back != BUTTON_ID_BACK || remap_select != BUTTON_ID_SELECT
static ButtonId remap[] = {
  remap_back,
  remap_up,
  remap_select,
  remap_down
};

// rockgarden discards these if the app doesn't use them
void window_single_click_subscribe__patch(ButtonId button_id, ClickHandler handler) {
  button_id = remap[button_id];
  window_single_click_subscribe(button_id, handler);
}

void window_single_repeating_click_subscribe__patch(ButtonId button_id, uint16_t repeat_interval_ms, ClickHandler handler) {
  button_id = remap[button_id];
  window_single_repeating_click_subscribe(button_id, repeat_interval_ms, handler);
}

void window_multi_click_subscribe__patch(ButtonId button_id, uint8_t min_clicks, uint8_t max_clicks, uint16_t timeout, bool last_click_only, ClickHandler handler) {
  button_id = remap[button_id];
  window_multi_click_subscribe(button_id, min_clicks, max_clicks, timeout, last_click_only, handler);
}

void window_long_click_subscribe__patch(ButtonId button_id, uint16_t delay_ms, ClickHandler down_handler, ClickHandler up_handler) {
  button_id = remap[button_id];
  window_long_click_subscribe(button_id, delay_ms, down_handler, up_handler);
}

void window_raw_click_subscribe__patch(ButtonId button_id, ClickHandler down_handler, ClickHandler up_handler, void * context) {
  button_id = remap[button_id];
  window_raw_click_subscribe(button_id, down_handler, up_handler, context);
}

// These normally call the above individual functions internally in the FW
// So we need to paste over them entirely
// and reinvent the wheel everywhere
static ClickConfigProvider scroll_layer_ccp;
static ClickConfigProvider menu_layer_ccp;

static uint8_t scroll_layer_context_offset;
#define GET_CONTEXT(scroll_layer) (scroll_layer_context_offset ? (void*)((uint8_t*)scroll_layer + scroll_layer_context_offset) : scroll_layer)
// Auto-discover the context - if they use it at all
void scroll_layer_set_context__patch(ScrollLayer *scroll_layer, void* context) {
  if (!scroll_layer_context_offset) {
    scroll_layer_set_context(scroll_layer, 0x01233210);
    for (scroll_layer_context_offset = 255; scroll_layer_context_offset >= 0; scroll_layer_context_offset--) {
      if (GET_CONTEXT(scroll_layer) == 0x01233210) {
        break;
      }
    }
  }
  scroll_layer_set_context(scroll_layer, context);
}

void scroll_layer_click_config_provider(ScrollLayer *scroll_layer) {
  // In case there's anything interesting happening in the original handler I don't know about
  scroll_layer_ccp(scroll_layer);
  // Overwrite up/down
  window_single_repeating_click_subscribe(remap[BUTTON_ID_UP], 100, scroll_layer_scroll_up_click_handler);
  window_single_repeating_click_subscribe(remap[BUTTON_ID_DOWN], 100, scroll_layer_scroll_down_click_handler);
  // Update context for the select-click handler
  // (the handler itself, if any, exists in app code and is thus handled by the earlier *_subscribe patches)
  window_set_click_context(remap[BUTTON_ID_SELECT], GET_CONTEXT(scroll_layer));
}

static uint16_t menu_layer_callbacks_word_offset = 0;
static uint16_t menu_layer_context_word_offset = 0;
#define MENULAYER_CALLBACKS(menu_layer) (*(MenuLayerCallbacks*)(((uint32_t*)menu_layer) + menu_layer_callbacks_word_offset))
#define MENULAYER_CALLBACK_CONTEXT(menu_layer) (*((void**)(((uint32_t*)menu_layer) + menu_layer_context_word_offset)))

static uint16_t no_row_count_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
  return 0;
}

static void no_draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
}

static inline void discover_menu_layer_callbacks_offset(MenuLayer *menu_layer) {
  if (menu_layer_callbacks_word_offset) return;
  MenuLayerCallbacks callbacks = {
    .get_num_rows = no_row_count_callback,
    .draw_row = no_draw_row
  };
  memset(&callbacks, 0, sizeof(callbacks));
  callbacks.get_num_rows = no_row_count_callback;
  callbacks.draw_row = no_draw_row;
  const void* callback_context = 0x32100123;
  menu_layer_set_callbacks(menu_layer, callback_context, callbacks);
  // There's no menu_layer_get_callbacks, we need to reverse-engineer it
  for (menu_layer_callbacks_word_offset = 0; menu_layer_callbacks_word_offset < 1024; menu_layer_callbacks_word_offset += 1) {
    if (MENULAYER_CALLBACKS(menu_layer).get_num_rows == callbacks.get_num_rows) break; // It's the first member in the struct
  }

  // Similarly for the callback context - it might be at a constant offset from the callbacks, but better safe than sorry
  for (menu_layer_context_word_offset = 0; menu_layer_context_word_offset < 1024; menu_layer_context_word_offset += 1) {
    if (MENULAYER_CALLBACK_CONTEXT(menu_layer) == callback_context) break;
  }
}

// At first this was in _set_callbacks - but there are at least 3 "deprecated" versions of that call to handle
// This is cleaner (only 1!)
MenuLayer* menu_layer_create__patch(GRect frame) {
  MenuLayer* menu_layer = menu_layer_create(frame);
  discover_menu_layer_callbacks_offset(menu_layer);
  return menu_layer;
}

#ifdef PBL_PLATFORM_BASALT
extern MenuLayer* menu_layer_legacy2_create(GRect frame); // It's not in pebble.h, shockingly
MenuLayer* menu_layer_legacy2_create__patch(GRect frame) {
  MenuLayer* menu_layer = menu_layer_legacy2_create(frame);
  discover_menu_layer_callbacks_offset(menu_layer);
  return menu_layer;
}
#endif

SimpleMenuLayer* simple_menu_layer_create__patch(GRect frame, Window * window, const SimpleMenuSection * sections, int32_t num_sections, void * callback_context) {
  SimpleMenuLayer* simple_menu_layer = simple_menu_layer_create(frame, window,  sections, num_sections, callback_context);
  discover_menu_layer_callbacks_offset(simple_menu_layer_get_menu_layer(simple_menu_layer));
  return simple_menu_layer;
}

static void menu_select_click_handler(ClickRecognizerRef recognizer, MenuLayer* menu_layer) {
  MenuLayerCallbacks callbacks = MENULAYER_CALLBACKS(menu_layer);
  MenuIndex selected_index = menu_layer_get_selected_index(menu_layer);
  if (callbacks.select_click) {
    callbacks.select_click(menu_layer, &selected_index, MENULAYER_CALLBACK_CONTEXT(menu_layer));
  }
}

static void menu_select_long_click_handler(ClickRecognizerRef recognizer, MenuLayer* menu_layer) {
  MenuLayerCallbacks callbacks = MENULAYER_CALLBACKS(menu_layer);
  MenuIndex selected_index = menu_layer_get_selected_index(menu_layer);
  if (callbacks.select_long_click) {
    callbacks.select_long_click(menu_layer, &selected_index, MENULAYER_CALLBACK_CONTEXT(menu_layer));
  }
}

static void menu_scroll_up_click_handler(ClickRecognizerRef recognizer, MenuLayer *menu_layer) {
  menu_layer_set_selected_next(menu_layer, true, MenuRowAlignCenter, true);
}

static void menu_scroll_down_click_handler(ClickRecognizerRef recognizer, MenuLayer *menu_layer) {
  menu_layer_set_selected_next(menu_layer, false, MenuRowAlignCenter, true);
}

static void menu_layer_click_config_provider(MenuLayer *menu_layer) {
  menu_layer_ccp(menu_layer);
  MenuLayerCallbacks callbacks = *((MenuLayerCallbacks*)(((uint32_t*)menu_layer) + menu_layer_callbacks_word_offset));
  // Trash all the existing long-click handlers
  for (int i = 0; i < NUM_BUTTONS; ++i) {
    window_long_click_subscribe(i, 0, NULL, NULL);
  }
  // These timing values seem right-ish
  window_single_click_subscribe(remap[BUTTON_ID_SELECT], menu_select_click_handler);
  // We don't want to register a long-click handler at all unless the user specified one, otherwise it makes regular clicks lag
  if (callbacks.select_long_click) {
    window_long_click_subscribe(remap[BUTTON_ID_SELECT], 0, menu_select_long_click_handler, NULL);
  }

  window_single_repeating_click_subscribe(remap[BUTTON_ID_UP], 100, menu_scroll_up_click_handler);
  window_single_repeating_click_subscribe(remap[BUTTON_ID_DOWN], 100, menu_scroll_down_click_handler);
}

void scroll_layer_set_click_config_onto_window__patch(ScrollLayer * scroll_layer, Window * window) {
  if (!scroll_layer_ccp) {
    scroll_layer_set_click_config_onto_window(scroll_layer, window);
    scroll_layer_ccp = window_get_click_config_provider(window);
  }
  window_set_click_config_provider_with_context(window, (ClickConfigProvider) scroll_layer_click_config_provider, scroll_layer);
}

void menu_layer_set_click_config_onto_window__patch(MenuLayer * menu_layer, Window * window) {
  if (!menu_layer_ccp) {
    menu_layer_set_click_config_onto_window(menu_layer, window);
    menu_layer_ccp = window_get_click_config_provider(window);
  }
  window_set_click_config_provider_with_context(window, (ClickConfigProvider) menu_layer_click_config_provider, menu_layer);
}

#endif
