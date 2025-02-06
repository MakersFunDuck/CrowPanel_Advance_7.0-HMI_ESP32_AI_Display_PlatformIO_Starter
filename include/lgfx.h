#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>

#ifndef _LGFX_H
#define _LGFX_H

class LGFX : public lgfx::LGFX_Device {
private:
  lgfx::Bus_RGB _bus_instance;
  lgfx::Panel_RGB _panel_instance;

  uint32_t screenWidth;
  uint32_t screenHeight;
  lv_disp_draw_buf_t draw_buf;

  lv_color_t disp_draw_buf[800 * 480 / 15];
  lv_disp_drv_t disp_drv;

public:
  LGFX(void);

  void setup();
};

extern LGFX lcd;

#endif
