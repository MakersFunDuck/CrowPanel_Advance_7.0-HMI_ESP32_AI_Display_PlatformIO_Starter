#include <lvgl.h>
#include <lgfx.h>
#include <touch.h>

#define TFT_BL 2

LGFX lcd;

LGFX::LGFX(void)
{
  {
    auto cfg = _bus_instance.config();
    cfg.panel = &_panel_instance;
    cfg.pin_d0 = GPIO_NUM_21;  // B0
    cfg.pin_d1 = GPIO_NUM_47;  // B1
    cfg.pin_d2 = GPIO_NUM_48;  // B2
    cfg.pin_d3 = GPIO_NUM_45;  // B3
    cfg.pin_d4 = GPIO_NUM_38;  // B4
    cfg.pin_d5 = GPIO_NUM_9;   // G0
    cfg.pin_d6 = GPIO_NUM_10;  // G1
    cfg.pin_d7 = GPIO_NUM_11;  // G2
    cfg.pin_d8 = GPIO_NUM_12;  // G3
    cfg.pin_d9 = GPIO_NUM_13;  // G4
    cfg.pin_d10 = GPIO_NUM_14; // G5
    cfg.pin_d11 = GPIO_NUM_7;  // R0
    cfg.pin_d12 = GPIO_NUM_17; // R1
    cfg.pin_d13 = GPIO_NUM_18; // R2
    cfg.pin_d14 = GPIO_NUM_3;  // R3
    cfg.pin_d15 = GPIO_NUM_46; // R4

    cfg.pin_henable = GPIO_NUM_42;
    cfg.pin_vsync = GPIO_NUM_41;
    cfg.pin_hsync = GPIO_NUM_40;
    cfg.pin_pclk = GPIO_NUM_39;
    cfg.freq_write = 15000000;

    cfg.hsync_polarity = 0;
    cfg.hsync_front_porch = 8;
    cfg.hsync_pulse_width = 4;
    cfg.hsync_back_porch = 8;
    cfg.vsync_polarity = 0;
    cfg.vsync_front_porch = 8;
    cfg.vsync_pulse_width = 4;
    cfg.vsync_back_porch = 8;
    cfg.pclk_idle_high = 1;
    _bus_instance.config(cfg);
  }
  {
    auto cfg = _panel_instance.config();
    cfg.memory_width = 800;
    cfg.memory_height = 480;
    cfg.panel_width = 800;
    cfg.panel_height = 480;
    cfg.offset_x = 0;
    cfg.offset_y = 0;
    _panel_instance.config(cfg);
  }
  _panel_instance.setBus(&_bus_instance);
  setPanel(&_panel_instance);
}

// Display flushing
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

  lcd.pushImageDMA(area->x1, area->y1, w, h, (lgfx::rgb565_t *)&color_p->full);
  lv_disp_flush_ready(disp);
}

void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
  if (touch_has_signal())
  {
    if (touch_touched())
    {
      // Update the coordinates for LVGL
      data->state = LV_INDEV_STATE_PR;
      data->point.x = touch_last_x;
      data->point.y = touch_last_y;

      // Print touched coordinates
      Serial.print("Touched at X: ");
      Serial.print(data->point.x);
      Serial.print(", Y: ");
      Serial.print(data->point.y);
      Serial.print("\n");
    }
    else if (touch_released())
    {
      data->state = LV_INDEV_STATE_REL;
    }
  }
  else
  {
    data->state = LV_INDEV_STATE_REL;
  }
  delay(10);
}

void LGFX::setup()
{
  // Initialise the display for LGFX
  this->begin();
  this->fillScreen(TFT_BLACK);
  this->setTextSize(2);
  screenWidth = this->width();
  screenHeight = this->height();

  // Initialise the display for LVGL
  lv_init();
  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight / 15);
  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register(&disp_drv);

  // Initialise the touchscreen device drivers
  touch_init();
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register(&indev_drv);

  ledcSetup(1, 300, 8);
  ledcAttachPin(TFT_BL, 1);
  ledcWrite(1, 255); // Screen brightness can be modified by adjusting this parameter (0-255)

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW);
  delay(500);
  digitalWrite(TFT_BL, HIGH);
}
