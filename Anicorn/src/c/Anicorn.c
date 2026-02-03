#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_status_layer;
static DictationSession *s_dictation_session;
static char s_last_transcription[512]; 

// 1. Callback cuando el dictado termina (Voz -> Texto)
static void dictation_session_callback(DictationSession *session, DictationSessionStatus status, 
                                        char *transcription, void *context) {
  if(status == DictationSessionStatusSuccess) {
    // Guardamos el texto
    snprintf(s_last_transcription, sizeof(s_last_transcription), "%s", transcription);
    text_layer_set_text(s_status_layer, s_last_transcription);

    // Enviar el texto al móvil (JS)
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    
    // CORRECCIÓN: Usamos dict_write_cstring
    dict_write_cstring(iter, MESSAGE_KEY_TRANSCRIPTION, s_last_transcription);
    
    app_message_outbox_send();
    vibes_short_pulse();
  } else {
    static char s_err_msg[32];
    snprintf(s_err_msg, sizeof(s_err_msg), "Error: %d", (int)status);
    text_layer_set_text(s_status_layer, s_err_msg);
  }
}

// 2. Manejador del botón SELECT
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  dictation_session_start(s_dictation_session);
}

// 3. Proveedor de configuración de clicks (CORRECCIÓN: Función separada)
static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

// 4. UI Window Load
static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_status_layer = text_layer_create(GRect(5, 40, bounds.size.w - 10, 100));
  text_layer_set_background_color(s_status_layer, GColorClear);
  text_layer_set_text_color(s_status_layer, GColorWhite);
  text_layer_set_text(s_status_layer, "Presiona SELECT para hablar");
  text_layer_set_text_alignment(s_status_layer, GTextAlignmentCenter);
  text_layer_set_font(s_status_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_status_layer));
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_status_layer);
}

// 5. Inicialización
static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorCobaltBlue);

  // Configurar Handlers de la ventana
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Configurar botones
  window_set_click_config_provider(s_main_window, click_config_provider);

  window_stack_push(s_main_window, true);

  // Inicializar Dictado
  s_dictation_session = dictation_session_create(sizeof(s_last_transcription), dictation_session_callback, NULL);
  dictation_session_enable_confirmation(s_dictation_session, false);

  // Abrir AppMessage
  app_message_open(512, 512); 
}

static void deinit() {
  dictation_session_destroy(s_dictation_session);
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}