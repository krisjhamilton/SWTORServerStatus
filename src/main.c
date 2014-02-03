#include <pebble.h>
 
Window* window;
TextLayer *title_layer, *state_layer, *name_layer, *description_layer, *time_layer;

char state_buffer[64], name_buffer[64], description_buffer[64], time_buffer[32];

enum {
	KEY_STATE = 0,
	KEY_NAME = 1,
	KEY_DESCRIPTION = 2,
};

void process_tuple(Tuple *t)
{
	//Get key
	int key = t->key;

	//Get integer value, if present
	int value = t->value->int32;

	//Get string value, if present
	char string_value[32];
	strcpy(string_value, t->value->cstring);

	//Decide what to do
	switch(key) {
		case KEY_STATE:
			//Location received
			snprintf(state_buffer, sizeof("State: couldbereallylongname"), "State: %s", string_value);
			text_layer_set_text(state_layer, (char*) &state_buffer);
			break;
		case KEY_NAME:
			//Location received
			snprintf(name_buffer, sizeof("Name: couldbereallylongname"), "Server(s): %s", string_value);
			text_layer_set_text(name_layer, (char*) &name_buffer);
			break;
		case KEY_DESCRIPTION:
			//Temperature received
			snprintf(description_buffer, sizeof("Description: couldbereallylongname"), "Count: %s", string_value);
			text_layer_set_text(description_layer, (char*) &description_buffer);
			break;
	}

	//Set time this update came in
	time_t temp = time(NULL);	
	struct tm *tm = localtime(&temp);
	strftime(time_buffer, sizeof("Last updated: XX:XX"), "Last updated: %H:%M", tm);
	text_layer_set_text(time_layer, (char*) &time_buffer);
}

static void in_received_handler(DictionaryIterator *iter, void *context) 
{
	(void) context;
	
	//Get data
	Tuple *t = dict_read_first(iter);
	if(t)
	{
		process_tuple(t);
	}
	
	//Get next
	while(t != NULL)
	{
		t = dict_read_next(iter);
		if(t)
		{
			process_tuple(t);
		}
	}
}

static TextLayer* init_text_layer(GRect location, GColor colour, GColor background, const char *res_id, GTextAlignment alignment)
{
	TextLayer *layer = text_layer_create(location);
	text_layer_set_text_color(layer, colour);
	text_layer_set_background_color(layer, background);
	text_layer_set_font(layer, fonts_get_system_font(res_id));
	text_layer_set_text_alignment(layer, alignment);

	return layer;
}
 
void window_load(Window *window)
{
	title_layer = init_text_layer(GRect(5, 0, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentLeft);
	text_layer_set_text(title_layer, "SWTOR Server Status");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(title_layer));

	state_layer = init_text_layer(GRect(5, 35, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentLeft);
	text_layer_set_text(state_layer, "State: N/A");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(state_layer));

	name_layer = init_text_layer(GRect(5,60, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentLeft);
	text_layer_set_text(name_layer, "Server(s): N/A");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(name_layer));

	description_layer = init_text_layer(GRect(5, 85, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentLeft);
	text_layer_set_text(description_layer, "Count: N/A");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(description_layer));
	
	time_layer = init_text_layer(GRect(5, 110, 144, 30), GColorBlack, GColorClear, "RESOURCE_ID_GOTHIC_18", GTextAlignmentLeft);
	text_layer_set_text(time_layer, "Last updated: N/A");
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
}
 
void window_unload(Window *window)
{	
	text_layer_destroy(title_layer);
	text_layer_destroy(state_layer);
	text_layer_destroy(name_layer);
	text_layer_destroy(description_layer);
	text_layer_destroy(time_layer);
}

void send_int(uint8_t key, uint8_t cmd)
{
	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
 	
 	Tuplet value = TupletInteger(key, cmd);
 	dict_write_tuplet(iter, &value);
 	
 	app_message_outbox_send();
}

void tick_callback(struct tm *tick_time, TimeUnits units_changed)
{
	//Every five minutes
	if(tick_time->tm_min % 5 == 0)
	{
		//Send an arbitrary message, the response will be handled by in_received_handler()
		send_int(5, 5);
	}
}
 
void init()
{
	window = window_create();
	WindowHandlers handlers = {
		.load = window_load,
		.unload = window_unload
	};
	window_set_window_handlers(window, handlers);

	//Register AppMessage events
	app_message_register_inbox_received(in_received_handler);					 
	app_message_open(512, 512);		//Large input and output buffer sizes
	
	//Register to receive minutely updates
	tick_timer_service_subscribe(MINUTE_UNIT, tick_callback);

	window_stack_push(window, true);
}
 
void deinit()
{
	tick_timer_service_unsubscribe();
	
	window_destroy(window);
}
 
int main(void)
{
	init();
	app_event_loop();
	deinit();
}