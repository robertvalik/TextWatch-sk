#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "num2words-sk.h"
#include "resource_ids.auto.h"
#include "settings.c"

#if HOUR_VIBRATION

static const uint32_t const vibes_00[] = { 200, 100, 200 };
static const uint32_t const vibes_01[] = { 100 };
static const uint32_t const vibes_02[] = { 100, 200, 100 };
static const uint32_t const vibes_03[] = { 100, 200, 100, 200, 100 };
static const uint32_t const vibes_04[] = { 100, 200, 100, 200, 100, 200, 100 };
static const uint32_t const vibes_05[] = { 300 };
static const uint32_t const vibes_06[] = { 300, 200, 100 };
static const uint32_t const vibes_07[] = { 300, 200, 100, 200, 100 };
static const uint32_t const vibes_08[] = { 300, 200, 100, 200, 100, 200, 100 };
static const uint32_t const vibes_09[] = { 300, 200, 100, 200, 100, 200, 100, 200, 100 };
static const uint32_t const vibes_10[] = { 300, 200, 300 };
static const uint32_t const vibes_11[] = { 300, 200, 300, 200, 100 };
static const uint32_t const vibes_12[] = { 300, 200, 300, 200, 100, 200, 100 };
static const uint32_t const vibes_13[] = { 300, 200, 300, 200, 100, 200, 100, 200, 100 };
static const uint32_t const vibes_14[] = { 300, 200, 300, 200, 100, 200, 100, 200, 100, 200, 100 };
static const uint32_t const vibes_15[] = { 300, 200, 300, 200, 300 };
static const uint32_t const vibes_16[] = { 300, 200, 300, 200, 300, 200, 100 };
static const uint32_t const vibes_17[] = { 300, 200, 300, 200, 300, 200, 100, 200, 100 };
static const uint32_t const vibes_18[] = { 300, 200, 300, 200, 300, 200, 100, 200, 100, 200, 100 };
static const uint32_t const vibes_19[] = { 300, 200, 300, 200, 300, 200, 100, 200, 100, 200, 100, 200, 100 };
static const uint32_t const vibes_20[] = { 300, 200, 300, 200, 300, 200, 300 };
static const uint32_t const vibes_21[] = { 300, 200, 300, 200, 300, 200, 300, 200, 100 };
static const uint32_t const vibes_22[] = { 300, 200, 300, 200, 300, 200, 300, 200, 100, 200, 100 };
static const uint32_t const vibes_23[] = { 300, 200, 300, 200, 300, 200, 300, 200, 100, 200, 100, 200, 100 };

VibePattern foo[24] = {
    { .durations = vibes_00, .num_segments = ARRAY_LENGTH(vibes_00) },
    { .durations = vibes_01, .num_segments = ARRAY_LENGTH(vibes_01) },
    { .durations = vibes_02, .num_segments = ARRAY_LENGTH(vibes_02) },
    { .durations = vibes_03, .num_segments = ARRAY_LENGTH(vibes_03) },
    { .durations = vibes_04, .num_segments = ARRAY_LENGTH(vibes_04) },
    { .durations = vibes_05, .num_segments = ARRAY_LENGTH(vibes_05) },
    { .durations = vibes_06, .num_segments = ARRAY_LENGTH(vibes_06) },
    { .durations = vibes_07, .num_segments = ARRAY_LENGTH(vibes_07) },
    { .durations = vibes_08, .num_segments = ARRAY_LENGTH(vibes_08) },
    { .durations = vibes_09, .num_segments = ARRAY_LENGTH(vibes_09) },
    { .durations = vibes_10, .num_segments = ARRAY_LENGTH(vibes_10) },
    { .durations = vibes_11, .num_segments = ARRAY_LENGTH(vibes_11) },
    { .durations = vibes_12, .num_segments = ARRAY_LENGTH(vibes_12) },
    { .durations = vibes_13, .num_segments = ARRAY_LENGTH(vibes_13) },
    { .durations = vibes_14, .num_segments = ARRAY_LENGTH(vibes_14) },
    { .durations = vibes_15, .num_segments = ARRAY_LENGTH(vibes_15) },
    { .durations = vibes_16, .num_segments = ARRAY_LENGTH(vibes_16) },
    { .durations = vibes_17, .num_segments = ARRAY_LENGTH(vibes_17) },
    { .durations = vibes_18, .num_segments = ARRAY_LENGTH(vibes_18) },
    { .durations = vibes_19, .num_segments = ARRAY_LENGTH(vibes_19) },
    { .durations = vibes_20, .num_segments = ARRAY_LENGTH(vibes_20) },
    { .durations = vibes_21, .num_segments = ARRAY_LENGTH(vibes_21) },
    { .durations = vibes_22, .num_segments = ARRAY_LENGTH(vibes_22) },
    { .durations = vibes_23, .num_segments = ARRAY_LENGTH(vibes_23) },
};

#endif

#define MY_UUID { 0x24, 0xA2, 0xD8, 0x3A, 0x89, 0x90, 0x4B, 0xEA, 0xA2, 0x9F, 0x1A, 0x92, 0x26, 0xC3, 0xDC, 0x1E }
PBL_APP_INFO(MY_UUID,
             "TextWatch-sk",
             "Robert Valik valik@visitors.sk",
             1, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
#if DEBUG
             APP_INFO_STANDARD_APP
#else
             APP_INFO_WATCH_FACE
#endif
);

Window window;

typedef struct {
	TextLayer currentLayer;
	TextLayer nextLayer;	
	PropertyAnimation currentAnimation;
	PropertyAnimation nextAnimation;
} Line;

Line line1;
Line line2;

PblTm t;

static char line1Str[2][BUFFER_SIZE];
static char line2Str[2][BUFFER_SIZE];

static bool textInitialized = false;

// Animation handler
void animationStoppedHandler(struct Animation *animation, bool finished, void *context)
{
	TextLayer *current = (TextLayer *)context;
	GRect rect = layer_get_frame(&current->layer);
	rect.origin.x = 144;
	layer_set_frame(&current->layer, rect);
}

// Animate line
void makeAnimationsForLayers(Line *line, TextLayer *current, TextLayer *next)
{
	GRect rect = layer_get_frame(&next->layer);
	rect.origin.x -= 144;
	
	property_animation_init_layer_frame(&line->nextAnimation, &next->layer, NULL, &rect);
	animation_set_duration(&line->nextAnimation.animation, ANIMATION_DURATION);
	animation_set_curve(&line->nextAnimation.animation, AnimationCurveEaseOut);
	animation_schedule(&line->nextAnimation.animation);
	
	GRect rect2 = layer_get_frame(&current->layer);
	rect2.origin.x -= 144;
	
	property_animation_init_layer_frame(&line->currentAnimation, &current->layer, NULL, &rect2);
	animation_set_duration(&line->currentAnimation.animation, ANIMATION_DURATION);
	animation_set_curve(&line->currentAnimation.animation, AnimationCurveEaseOut);
	
	animation_set_handlers(&line->currentAnimation.animation, (AnimationHandlers) {
		.stopped = (AnimationStoppedHandler) animationStoppedHandler
	}, current);
	
	animation_schedule(&line->currentAnimation.animation);
}

// Update line
void updateLineTo(Line *line, char lineStr[2][BUFFER_SIZE], char *value)
{
	TextLayer *next, *current;
	
	GRect rect = layer_get_frame(&line->currentLayer.layer);
	current = (rect.origin.x == 0) ? &line->currentLayer : &line->nextLayer;
	next = (current == &line->currentLayer) ? &line->nextLayer : &line->currentLayer;
	
	// Update correct text only
	if (current == &line->currentLayer) {
		memset(lineStr[1], 0, BUFFER_SIZE);
		memcpy(lineStr[1], value, strlen(value));
		text_layer_set_text(next, lineStr[1]);
	} else {
		memset(lineStr[0], 0, BUFFER_SIZE);
		memcpy(lineStr[0], value, strlen(value));
		text_layer_set_text(next, lineStr[0]);
	}
	
	makeAnimationsForLayers(line, current, next);
}

// Check to see if the current line needs to be updated
bool needToUpdateLine(Line *line, char lineStr[2][BUFFER_SIZE], char *nextValue)
{
	char *currentStr;
	GRect rect = layer_get_frame(&line->currentLayer.layer);
	currentStr = (rect.origin.x == 0) ? lineStr[0] : lineStr[1];

	if (memcmp(currentStr, nextValue, strlen(nextValue)) != 0 ||
		(strlen(nextValue) == 0 && strlen(currentStr) != 0)) {
		return true;
	}
	return false;
}

// Update screen based on new time
void display_time(PblTm *t)
{
	// The current time text will be stored in the following 3 strings
	char textLine1[BUFFER_SIZE];
	char textLine2[BUFFER_SIZE];
	
	time_to_2words(t->tm_hour, t->tm_min, textLine1, BUFFER_SIZE, textLine2, BUFFER_SIZE);
	
	if (needToUpdateLine(&line1, line1Str, textLine1)) {
		updateLineTo(&line1, line1Str, textLine1);	
	}
	if (needToUpdateLine(&line2, line2Str, textLine2)) {
		updateLineTo(&line2, line2Str, textLine2);	
	}
}

// Update screen without animation first time we start the watchface
void display_initial_time(PblTm *t)
{
	time_to_2words(t->tm_hour, t->tm_min, line1Str[0], BUFFER_SIZE, line2Str[0], BUFFER_SIZE);
	
	text_layer_set_text(&line1.currentLayer, line1Str[0]);
	text_layer_set_text(&line2.currentLayer, line2Str[0]);
}


// Configure the first line of text
void configureBoldLayer(TextLayer *textlayer)
{
	text_layer_set_font(textlayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_BOLD_35)));
	text_layer_set_text_color(textlayer, COLOR_TEXT);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_text_alignment(textlayer, GTextAlignmentLeft);
}

// Configure for the 2nd and 3rd lines
void configureLightLayer(TextLayer *textlayer)
{
	text_layer_set_font(textlayer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SANSATION_LIGHT_34)));
	text_layer_set_text_color(textlayer, COLOR_TEXT);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_text_alignment(textlayer, GTextAlignmentLeft);
}


/** 
 * Debug methods. For quickly debugging enable debug macro on top to transform the watchface into
 * a standard app and you will be able to change the time with the up and down buttons
 */ 
#if DEBUG

void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	(void)recognizer;
	(void)window;
	
	t.tm_min += 1;
	if (t.tm_min >= 60) {
		t.tm_min = 0;
		t.tm_hour += 1;
		
		if (t.tm_hour >= 24) {
			t.tm_hour = 0;
		}
	}
	display_time(&t);
}


void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	(void)recognizer;
	(void)window;
	
	t.tm_min -= 1;
	if (t.tm_min < 0) {
		t.tm_min = 59;
		t.tm_hour -= 1;
	}
	display_time(&t);
}

void click_config_provider(ClickConfig **config, Window *window) {
  (void)window;

  config[BUTTON_ID_UP]->click.handler = (ClickHandler) up_single_click_handler;
  config[BUTTON_ID_UP]->click.repeat_interval_ms = 100;

  config[BUTTON_ID_DOWN]->click.handler = (ClickHandler) down_single_click_handler;
  config[BUTTON_ID_DOWN]->click.repeat_interval_ms = 100;
}

#endif

void handle_init(AppContextRef ctx) {
  	(void)ctx;

	window_init(&window, "Clock_Window");
	window_stack_push(&window, true);
	window_set_background_color(&window, COLOR_BG);

	// Init resources
	resource_init_current_app(&APP_RESOURCES);
	
	// 1st line layers
	text_layer_init(&line1.currentLayer, GRect(0, 12, 144, 80));
	text_layer_init(&line1.nextLayer, GRect(144, 12, 144, 80));
	configureBoldLayer(&line1.currentLayer);
	configureBoldLayer(&line1.nextLayer);

	// 2nd layers
	text_layer_init(&line2.currentLayer, GRect(0, 81, 144, 168-81));
	text_layer_init(&line2.nextLayer, GRect(144, 81, 144, 168-81));
	configureLightLayer(&line2.currentLayer);
	configureLightLayer(&line2.nextLayer);

	// Configure time on init
	get_time(&t);
	display_initial_time(&t);
	
	// Load layers
  	layer_add_child(&window.layer, &line1.currentLayer.layer);
	layer_add_child(&window.layer, &line1.nextLayer.layer);
	layer_add_child(&window.layer, &line2.currentLayer.layer);
	layer_add_child(&window.layer, &line2.nextLayer.layer);
	
#if DEBUG
	// Button functionality
	window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
#endif
}

// Time handler called every minute by the system
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;

#if HOUR_VIBRATION
  if ((t->tick_time->tm_min == 0) && (t->tick_time->tm_sec < 2) ) vibes_enqueue_custom_pattern(foo[t->tick_time->tm_hour]);
#endif
#if HOUR_SIMPLE_VIBRATION
  if ((t->tick_time->tm_min == 0) && (t->tick_time->tm_sec < 2) ) vibes_short_pulse();
#endif
  display_time(t->tick_time);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
	.tick_info = {
		      .tick_handler = &handle_minute_tick,
		      .tick_units = MINUTE_UNIT
		    }
  };
  app_event_loop(params, &handlers);
}