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
static int StatusHour = 0;
static int StatusMin = 0;
static char szDay[10];
static char szDate[20];
static char s_buffer_date[30];

int anglev, x, y, a;
int radiusv;



static void update_time() 
{
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
 // static char s_buffer2[8];
 // static char s_buffer3[20];
 // strftime(s_buffer2, sizeof(s_buffer2), "%H:%M", tick_time);
  
  strftime(szDay, sizeof(szDay), "%a", tick_time);
  strftime(szDate, sizeof(szDate), "%d/%m/%y", tick_time);
  
  snprintf(s_buffer_date, 22, "\r\n\r\n%s\r\n%s", szDay, szDate);
  
//  text_layer_set_text(text_top, s_buffer3);
//  text_layer_set_text(text_layer, s_buffer2);
  
  StatusHour = tick_time->tm_hour;
  if (StatusHour >= 12)
    StatusHour -= 12;
    
  StatusMin = tick_time->tm_min; 
  
  layer_mark_dirty(s_canvas_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) 
{
  update_time();
}


static void canvas_update_proc(Layer *layer, GContext *ctx) 
{
  float AngleHour = 0;
  float AngleMin = 0;

  /*
  int i;
  radiusv = 150;

  for( i = 1 ; i < StatusHour + 1 ; i++)
  {
    anglev = TRIG_MAX_ANGLE * i / MAX_MINS;

    y = (-cos_lookup(anglev) * radiusv / TRIG_MAX_RATIO);
    x = (sin_lookup(anglev) * radiusv / TRIG_MAX_RATIO) ;

    GPoint start = GPoint(DISP_X_MID, DISP_Y_MID);
    GPoint end = GPoint(DISP_X_MID + x, DISP_Y_MID + y);
    graphics_draw_line(ctx, start, end); 
  }
  */
  GRect bounds = layer_get_bounds(layer);
  
    // Date
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_buffer_date, fonts_get_system_font(FONT_KEY_GOTHIC_24), bounds,
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  
  // Minute
  AngleMin = (360.0f * ((float)StatusMin / 60.0f));
  GRect frame = grect_inset(bounds, GEdgeInsets(-40));
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 45,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(AngleMin));
  
  // Hour
  AngleHour = (360.0f * ((float)StatusHour / 12.0f));
      GRect frame2 = grect_inset(bounds, GEdgeInsets(10));
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_radial(ctx, frame2, GOvalScaleModeFitCircle, 20,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(AngleHour));
}


static void window_load(Window *window) 
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  /*
  text_top = text_layer_create((GRect) { .origin = { 5, 10 }, .size = { bounds.size.w-10, 40 } });
  text_layer_set_text_alignment(text_top, GTextAlignmentCenter);
  text_layer_set_font(text_top, fonts_get_system_font(FONT_KEY_GOTHIC_28));
  
  text_layer = text_layer_create((GRect) { .origin = { 10, 90 }, .size = { bounds.size.w-20, 50 } });
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  */
  // Create canvas layer
  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  update_time();
  
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
//  layer_add_child(window_layer, text_layer_get_layer(text_top));
//  layer_add_child(window_layer, text_layer_get_layer(text_layer));
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