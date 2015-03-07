#include <pebble.h>
#include "bwd.h"

static Window *window;
static Layer *background_layer;
BitmapWithData checkers;

static TextLayer *name_layer;
static TextLayer *format_layer;
static BitmapLayer *bitmap_layer;
BitmapWithData bitmap;

int background_index = 0;
int bitmap_index = 0;

#define NUM_BACKGROUNDS 6

typedef struct {
  int resource_id;
  const char *name;
} BitmapDef;

#define NUM_BITMAPS 8
BitmapDef bitmap_defs[NUM_BITMAPS] = {
  { RESOURCE_ID_BM64, "bm64" },
  { RESOURCE_ID_BM16, "bm16" },
  { RESOURCE_ID_BM16_ALPHA, "bm16_alpha" },
  { RESOURCE_ID_BM4, "bm4" },
  { RESOURCE_ID_BM4_GRAYSCALE, "bm4_grayscale" },
  { RESOURCE_ID_BM4_ALPHA, "bm4_alpha" },
  { RESOURCE_ID_BM2, "bm2" },
  { RESOURCE_ID_BM2_GRAYSCALE, "bm2_grayscale" },
};

static void inc_background(int inc_value) {
  background_index = (background_index + NUM_BACKGROUNDS + inc_value) % NUM_BACKGROUNDS;

  layer_mark_dirty(background_layer);
}

static void inc_bitmap(int inc_value) {
  bitmap_index = (bitmap_index + NUM_BITMAPS + inc_value) % NUM_BITMAPS;

  bwd_destroy(&bitmap);
  bitmap = rle_bwd_create(bitmap_defs[bitmap_index].resource_id);
  bitmap_layer_set_bitmap(bitmap_layer, bitmap.bitmap);
  text_layer_set_text(name_layer, bitmap_defs[bitmap_index].name);

  const char *format_str = "Unknown";
#ifdef PBL_PLATFORM_APLITE
  format_str = "1Bit";  // Only thing supported by Aplite
#else  //  PBL_PLATFORM_APLITE
  switch (gbitmap_get_format(bitmap.bitmap)) {
  case GBitmapFormat1Bit:
    format_str = "1Bit";
    break;
  case GBitmapFormat8Bit:
    format_str = "8Bit";
    break;
  case GBitmapFormat1BitPalette:
    format_str = "1BitPalette";
    break;
  case GBitmapFormat2BitPalette:
    format_str = "2BitPalette";
    break;
  case GBitmapFormat4BitPalette:
    format_str = "4BitPalette";
    break;
  }
#endif  //  PBL_PLATFORM_APLITE
  text_layer_set_text(format_layer, format_str);
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  inc_background(1);
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  inc_bitmap(-1);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  inc_bitmap(1);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}


void background_layer_update_callback(Layer *me, GContext *ctx) {
  GRect destination = layer_get_frame(me);

  switch (background_index) {
  case 0:
    graphics_draw_bitmap_in_rect(ctx, checkers.bitmap, destination);
    return;

#ifndef PBL_PLATFORM_APLITE
  case 1:
    graphics_context_set_fill_color(ctx, GColorFromRGB(0x00, 0x00, 0x55));
    break;

  case 2:
    graphics_context_set_fill_color(ctx, GColorFromRGB(0x00, 0x00, 0xcc));
    break;

  case 3:
    graphics_context_set_fill_color(ctx, GColorFromRGB(0x00, 0x55, 0xcc));
    break;

  case 4:
    graphics_context_set_fill_color(ctx, GColorFromRGB(0xff, 0xff, 0xff));
    break;

  case 5:
    graphics_context_set_fill_color(ctx, GColorFromRGB(0x00, 0x00, 0x00));
    break;
#endif  // PBL_PLATFORM_APLITE
  }

  graphics_fill_rect(ctx, destination, 0, GCornerNone);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  background_layer = layer_create(bounds);
  layer_set_update_proc(background_layer, &background_layer_update_callback);
  layer_add_child(window_layer, background_layer);
  checkers = rle_bwd_create(RESOURCE_ID_CHECKERS);

  bitmap_layer = bitmap_layer_create((GRect) { .origin = { 40, 22 }, .size = { 64, 64 } });
  bitmap_layer_set_compositing_mode(bitmap_layer, GCompOpSet);
  layer_add_child(background_layer, bitmap_layer_get_layer(bitmap_layer));

  name_layer = text_layer_create((GRect) { .origin = { 0, 108 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(name_layer, GTextAlignmentCenter);
  layer_add_child(background_layer, text_layer_get_layer(name_layer));

  format_layer = text_layer_create((GRect) { .origin = { 0, 128 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(format_layer, GTextAlignmentCenter);
  layer_add_child(background_layer, text_layer_get_layer(format_layer));

  inc_bitmap(0);
}

static void window_unload(Window *window) {
  bitmap_layer_destroy(bitmap_layer);
  text_layer_destroy(name_layer);
  text_layer_destroy(format_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
