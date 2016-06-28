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
static int StatusCharge = 0;
static int PrevChargeTime = 0;
static char szDay[10];
static char szDate[20];
static char szText[20];
static char s_buffer_date[30];
static bool bBatteryState = 0;
static uint8_t u8BattCharg = false;

int anglev, x, y, a;
int radiusv; 

static void update_time() 
{
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  BatteryChargeState ChargeState = battery_state_service_peek();
  
  strftime(szDay, sizeof(szDay), "%a", tick_time);
  strftime(szDate, sizeof(szDate), "%d/%m/%y", tick_time);
  
  u8BattCharg = ChargeState.charge_percent;
  bBatteryState = ChargeState.is_charging;
  
  snprintf(s_buffer_date, 22, "\r\n\r\n%s\r\n%s", szDay, szDate);
  
  StatusHour = tick_time->tm_hour;
  if (StatusHour >= 12)
    StatusHour -= 12;
    
  StatusMin = tick_time->tm_min; 
  
  if (bBatteryState == true)
  {
    PrevChargeTime = temp;
  }

  StatusCharge = (temp -PrevChargeTime) / (3600 * 24);  
 
  layer_mark_dirty(s_canvas_layer);
}


static void  tick_handler(struct tm *tick_time, TimeUnits units_changed) 
{
  update_time();
}


static void batt_status_handler(BatteryChargeState charge_state)
{  
  update_time();
}



static void canvas_update_proc(Layer *layer, GContext *ctx) 
{
  float AngleHour = 0;
  float AngleMin = 0;
  float fDivAng = 0;

  GRect frame;
  GRect bounds = layer_get_bounds(layer);  
  graphics_context_set_text_color(ctx, GColorBlack);
  
    // Date
  frame = grect_inset(bounds, GEdgeInsets(-10));
  graphics_draw_text(ctx, s_buffer_date, fonts_get_system_font(FONT_KEY_GOTHIC_24), frame,
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  
  // Charge day
  frame = grect_inset(bounds, GEdgeInsets(50));
  if (PrevChargeTime == -1)
    snprintf(szText, 10, "\n\n\n\nn.a");  
  else
    snprintf(szText, 10, "\n\n\n\nCD %d",StatusCharge);
  graphics_draw_text(ctx, szText, fonts_get_system_font(FONT_KEY_GOTHIC_14), frame,
                     GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    
  // Minute
  AngleMin = (360.0f * ((float)StatusMin / 60.0f));
  frame = grect_inset(bounds, GEdgeInsets(-16));
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 22,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360.0f));
  frame = grect_inset(bounds, GEdgeInsets(-15));
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 20,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360.0f));

    // Add minute divisions
  frame = grect_inset(bounds, GEdgeInsets(-16));
  graphics_context_set_fill_color(ctx, GColorBlack);
  for ( fDivAng = 0 ; fDivAng < 360 ; fDivAng+=30 )
  {
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 8,
                                  DEG_TO_TRIGANGLE(fDivAng), DEG_TO_TRIGANGLE(fDivAng+1));
  }

  if (AngleMin == 0)
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 22,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(2.0f));
  else
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 22,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(AngleMin));
  
  // Hour
  // AngleHour = (360.0f * ((float)StatusHour / 12.0f));
  AngleHour = (360.0f * ((float)StatusHour / 12.0f)) + (AngleMin * 0.0833f);
  frame = grect_inset(bounds, GEdgeInsets(9));
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 22,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360.0f));
  frame = grect_inset(bounds, GEdgeInsets(10));
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 20,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(360.0f));
  
    // Add hour divisions
  frame = grect_inset(bounds, GEdgeInsets(9));
  graphics_context_set_fill_color(ctx, GColorBlack);
  for ( fDivAng = 0 ; fDivAng < 360 ; fDivAng+=30 )
  {
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 8,
                                  DEG_TO_TRIGANGLE(fDivAng), DEG_TO_TRIGANGLE(fDivAng+1));
  }
  
  if (AngleHour == 0)
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 22,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(2.0f));
  else
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, 22,
                                  DEG_TO_TRIGANGLE(0), DEG_TO_TRIGANGLE(AngleHour));
  
  // Draw a rectangle
  int corner_radius = 0;
  GRect rect_bounds = GRect(DISP_X_MID-20, DISP_Y_MID+10, 40, 10);
  graphics_draw_rect(ctx, rect_bounds);
  rect_bounds = GRect(DISP_X_MID-17, DISP_Y_MID+13, 34, 4);
  graphics_draw_rect(ctx, rect_bounds);
  rect_bounds = GRect(DISP_X_MID+19, DISP_Y_MID+13, 3, 5);
  graphics_draw_rect(ctx, rect_bounds);
  rect_bounds = GRect(DISP_X_MID-17, DISP_Y_MID+13, (int)(34.0f * ((float)u8BattCharg / 100.0f)), 4);
  graphics_fill_rect(ctx, rect_bounds, corner_radius, GCornersAll);
  
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
  PrevChargeTime = -1; // 1466792090;
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  
  battery_state_service_subscribe(batt_status_handler);
  
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