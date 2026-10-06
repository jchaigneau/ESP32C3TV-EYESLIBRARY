#include "Face.h"
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_ST7735S _panel;
  lgfx::Bus_SPI _bus;

public:
  LGFX()
  {
    auto bus_cfg = _bus.config();
    bus_cfg.spi_host = SPI2_HOST;
    bus_cfg.spi_mode = 0;
    bus_cfg.freq_write = 40000000;
    bus_cfg.freq_read = 16000000;
    bus_cfg.spi_3wire = false;
    bus_cfg.use_lock = true;
    bus_cfg.dma_channel = SPI_DMA_CH_AUTO;
    bus_cfg.pin_sclk = 3;
    bus_cfg.pin_mosi = 4;
    bus_cfg.pin_miso = -1;
    bus_cfg.pin_dc = 0;
    _bus.config(bus_cfg);
    _panel.setBus(&_bus);

    auto panel_cfg = _panel.config();
    panel_cfg.pin_cs = 2;
    panel_cfg.pin_rst = 5;
    panel_cfg.pin_busy = -1;
    panel_cfg.panel_width = 128;
    panel_cfg.panel_height = 128;
    panel_cfg.offset_x = 0;
    panel_cfg.offset_y = 0;
    panel_cfg.offset_rotation = 0;
    panel_cfg.dummy_read_pixel = 8;
    panel_cfg.dummy_read_bits = 1;
    panel_cfg.readable = false;
    panel_cfg.invert = false;
    panel_cfg.rgb_order = false;
    panel_cfg.dlen_16bit = false;
    panel_cfg.bus_shared = false;
    _panel.config(panel_cfg);

    setPanel(&_panel);
  }
};

LGFX tft;
LGFX_Sprite canvas(&tft);

Face::Face(uint16_t screenWidth, uint16_t screenHeight, uint16_t eyeSize)
  : LeftEye(*this), RightEye(*this), Blink(*this),
    Look(*this), Behavior(*this), Expression(*this)
{
  Width = screenWidth;
  Height = screenHeight;
  EyeSize = eyeSize;

  CenterX = Width / 2;
  CenterY = Height / 2;

  LeftEye.IsMirrored = true;

  // Initialize the display BEFORE creating/using the sprite.
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  // 128x128x16-bit = 32768 bytes.
  canvas.setColorDepth(16);
  if (!canvas.createSprite(Width, Height)) {
    // If allocation fails, keep the physical display black rather than
    // dereferencing an invalid sprite.
    tft.fillScreen(TFT_RED);
    return;
  }
  canvas.fillSprite(TFT_BLACK);

  Behavior.Clear();
  Behavior.Timer.Start();
}

void Face::LookFront()  { Look.LookAt(0.0, 0.0); }
void Face::LookRight()  { Look.LookAt(-1.0, 0.0); }
void Face::LookLeft()   { Look.LookAt(1.0, 0.0); }
void Face::LookTop()    { Look.LookAt(0.0, 1.0); }
void Face::LookBottom() { Look.LookAt(0.0, -1.0); }

void Face::Wait(unsigned long milliseconds)
{
  unsigned long start = millis();
  while (millis() - start < milliseconds) {
    Draw();
  }
}

void Face::DoBlink()
{
  Blink.Blink();
}

void Face::Update()
{
  if (RandomBehavior) Behavior.Update();
  if (RandomLook) Look.Update();
  if (RandomBlink) Blink.Update();
  Draw();
}

void Face::Draw()
{
  canvas.fillSprite(TFT_BLACK);

  LeftEye.CenterX = CenterX - EyeSize / 2 - EyeInterDistance;
  LeftEye.CenterY = CenterY;
  LeftEye.Draw();

  RightEye.CenterX = CenterX + EyeSize / 2 + EyeInterDistance;
  RightEye.CenterY = CenterY;
  RightEye.Draw();

  canvas.pushSprite(0, 0);
}
