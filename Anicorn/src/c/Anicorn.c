#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_status_layer;

// --- COMUNICACIÓN APP MESSAGE ---

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Se ejecuta cuando el JS nos envía datos
  Tuple *data_tuple = dict_find(iterator, MESSAGE_KEY_DATA_KEY);
  
  if(data_tuple) {
    text_layer_set_text(s_status_layer, data_tuple->value->cstring);
  }
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  text_layer_set_text(s_status_layer, "Error de envío");
}

// --- LÓGICA DE BOTONES ---

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(s_status_layer, "Cargando...");

  // Enviar mensaje al JS para que haga el fetch
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  
  // Añadimos una llave dummy para avisar al JS que empiece
  dict_write_uint8(iter, MESSAGE_KEY_FETCH_COMMAND, 1);
  
  app_message_outbox_send();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

// --- UI Y CICLO DE VIDA ---

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_status_layer = text_layer_create(GRect(5, 50, bounds.size.w - 10, 80));
  text_layer_set_background_color(s_status_layer, GColorClear);
  text_layer_set_text(s_status_layer, "Pulsa SELECT para consultar API");
  text_layer_set_text_alignment(s_status_layer, GTextAlignmentCenter);
  text_layer_set_font(s_status_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_status_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_status_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_click_config_provider(s_main_window, click_config_provider);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);

  // REGISTRO DE APP MESSAGE (Importante)
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  
  // Abrir canales de comunicación (Tamaño de buffer entrada/salida)
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}