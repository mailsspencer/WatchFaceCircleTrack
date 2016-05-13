#include <pebble.h>
//  Display  144 x 168
#define DISP_X_WIDTH  144
#define DISP_Y_WIDTH  168

#define MAX_MINS      (23 * 60)

#define DISP_X_MID    (DISP_X_WIDTH/2)
#define DISP_Y_MID    (DISP_Y_WIDTH/2)

#define STATUS_BAR_PXL_WIDTH 95.0f

static Window *window;
static TextLayer *text_top;
static TextLayer *text_layer;
static Layer *s_canvas_layer;
static int StatusBar = 0;
int anglev, x, y, a;
int radiusv;

double pi(void)
{
  return 3.141592653589793;
}

float rad2deg(float angle)
{
  return angle * (180.0f / pi());  
}

float deg2rad(float angle)
{
  return angle * (pi() / 180.0f);  
}

static void update_time() 
{
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  static char s_buffer2[8];
  static char s_buffer3[20];
  strftime(s_buffer2, sizeof(s_buffer2), "%H:%M", tick_time);
  
  strftime(s_buffer3, sizeof(s_buffer3), "%a %d/%m/%y", tick_time);
  
  text_layer_set_text(text_top, s_buffer3);
  text_layer_set_text(text_layer, s_buffer2);
  
  StatusBar = tick_time->tm_min + (tick_time->tm_hour * 60);
  
  layer_mark_dirty(s_canvas_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) 
{
  update_time();
}


static void canvas_update_proc(Layer *layer, GContext *ctx) 
{
  int i;
  radiusv = 150;

  for( i = 1 ; i < StatusBar + 1 ; i++)
  {
    anglev = TRIG_MAX_ANGLE * i / MAX_MINS;

    y = (-cos_lookup(anglev) * radiusv / TRIG_MAX_RATIO);
    x = (sin_lookup(anglev) * radiusv / TRIG_MAX_RATIO) ;

    GPoint start = GPoint(DISP_X_MID, DISP_Y_MID);
    GPoint end = GPoint(DISP_X_MID + x, DISP_Y_MID + y);
    graphics_draw_line(ctx, start, end); 
  }
}


static void window_load(Window *window) 
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_top = text_layer_create((GRect) { .origin = { 5, 10 }, .size = { bounds.size.w-10, 40 } });
  text_layer_set_text_alignment(text_top, GTextAlignmentCenter);
  text_layer_set_font(text_top, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  
  text_layer = text_layer_create((GRect) { .origin = { 10, 90 }, .size = { bounds.size.w-20, 50 } });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  
  // Create canvas layer
  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  update_time();
  
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
  layer_add_child(window_layer, text_layer_get_layer(text_top));
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}


static void window_unload(Window *window) 
{
  text_layer_destroy(text_layer);
}


static void init(void) 
{
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
	.load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  window_stack_push(window, animated);
}

static void deinit(void) 
{
  window_destroy(window);
}

int main(void) 
{
  init();
  app_event_loop();
  deinit();
}