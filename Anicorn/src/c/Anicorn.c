#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_status_layer;
static TextLayer *s_header_layer;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  Tuple *data_tuple = dict_find(iterator, MESSAGE_KEY_DATA_KEY);
  if(data_tuple) {
    // Cambiamos el color de fondo cuando recibimos datos
    window_set_background_color(s_main_window, GColorLiberty);
    text_layer_set_text(s_status_layer, data_tuple->value->cstring);
    vibes_double_pulse(); // Feedback táctil de éxito
  }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_status_layer, "Consultando nube...");
  window_set_background_color(s_main_window, GColorDarkGray);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint8(iter, MESSAGE_KEY_FETCH_COMMAND, 1);
  app_message_outbox_send();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Cabecera elegante
  s_header_layer = text_layer_create(GRect(0, 10, bounds.size.w, 30));
  text_layer_set_background_color(s_header_layer, GColorClear);
  text_layer_set_text_color(s_header_layer, GColorWhite);
  text_layer_set_text(s_header_layer, "ANICORN API");
  text_layer_set_text_alignment(s_header_layer, GTextAlignmentCenter);
  text_layer_set_font(s_header_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_header_layer));

  // Cuerpo del mensaje
  s_status_layer = text_layer_create(GRect(10, 50, bounds.size.w - 20, 100));
  text_layer_set_background_color(s_status_layer, GColorClear);
  text_layer_set_text_color(s_status_layer, GColorWhite);
  text_layer_set_text(s_status_layer, "Pulsa el botón central para empezar");
  text_layer_set_text_alignment(s_status_layer, GTextAlignmentCenter);
  text_layer_set_font(s_status_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_status_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_header_layer);
  text_layer_destroy(s_status_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack); // Fondo inicial

  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);

  app_message_register_inbox_received(inbox_received_callback);
  app_message_open(256, 64); // Aumentamos buffer de entrada para JSONs largos
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}