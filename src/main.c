#include <pebble.h>
	
static Window *window;
static TextLayer *text_layer;
static TextLayer *sync_layer;

static AppSync sync;
static uint8_t sync_buffer[64];

enum MessageKey {
	MESSAGE_KEY = 0x0,
};

static void send_message() {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	
	Tuplet value = TupletInteger(1, 0);
	dict_write_tuplet(iter, &value);
	
	app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	text_layer_set_text(text_layer, "Select");
	send_message();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Tuple Changed: %s", new_tuple->value->cstring);
	switch (key) {
		case MESSAGE_KEY:
			text_layer_set_text(sync_layer, new_tuple->value->cstring);
			break;
	}
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	
	text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
	text_layer_set_text(text_layer, "Press a button!");
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(text_layer));
	
	sync_layer = text_layer_create((GRect) { .origin = { 0, 92 }, .size = { bounds.size.w, 20} });
	text_layer_set_text_alignment(sync_layer, GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(sync_layer));
	
	
	Tuplet initial_values[] = {
		TupletCString(MESSAGE_KEY, "Waiting for JS..."),
	};
	
	app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
				  sync_tuple_changed_callback, sync_error_callback, NULL);
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

void init(void) {
	window = window_create();
	window_set_click_config_provider(window, click_config_provider);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	
	const int inbound_size = 64;
	const int outbound_size = 64;
	app_message_open(inbound_size, outbound_size);
	
	const bool animated = true;
	window_stack_push(window, animated);
}

void deinit(void) {
	window_destroy(window);
}

int main(void) {
	init();
	
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);
	
	app_event_loop();
	deinit();
}
