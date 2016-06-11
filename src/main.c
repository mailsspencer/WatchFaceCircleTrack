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
  
  strftime(szDay, sizeof(szDay), "%a", tick_time);
  strftime(szDate, sizeof(szDate), "%d/%m/%y", tick_time);
  
  snprintf(s_buffer_date, 22, "\r\n\r\n%s\r\n%s", szDay, szDate);
  
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

  GRect bounds = layer_get_bounds(layer);  
  
    // Date
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_buffer_date, fonts_get_system_font(FONT_KEY_GOTHIC_24), bounds,
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  // Minute
  AngleMin = (360.0f * ((float)StatusMin / 60.0f));
  GRect frame = grect_inset(bounds, GEdgeInsets(-41));
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 47,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360.0f));
  frame = grect_inset(bounds, GEdgeInsets(-40));
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 45,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360.0f));

  graphics_context_set_fill_color(ctx, GColorBlack);
  if (AngleMin == 0)
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 45,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(2.0f));
  else
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 45,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(AngleMin));
  
  // Hour
  AngleHour = (360.0f * ((float)StatusHour / 12.0f));
  frame = grect_inset(bounds, GEdgeInsets(9));
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 22,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360.0f));
  frame = grect_inset(bounds, GEdgeInsets(10));
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 20,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360.0f));
  
  graphics_context_set_fill_color(ctx, GColorBlack);
  if (AngleHour == 0)
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 20,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(2.0f));
  else
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 20,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(AngleHour));
}


static void window_load(Window *window) 
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create canvas layer
  s_canvas_layer = layer_create(bounds);
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);
  
  update_time();
  
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
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