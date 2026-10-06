#ifndef _EYEDRAWER_h
#define _EYEDRAWER_h

#include <Arduino.h>
#include "Common.h"
#include "EyeConfig.h"
#include <LovyanGFX.hpp>

extern LGFX_Sprite canvas;

enum CornerType { T_R, T_L, B_L, B_R };

class EyeDrawer {
  static uint16_t C(uint16_t color) { return color ? TFT_WHITE : TFT_BLACK; }

public:
  static void Draw(int16_t centerX, int16_t centerY, EyeConfig *config) {
    int32_t delta_y_top = config->Height * config->Slope_Top / 2.0;
    int32_t delta_y_bottom = config->Height * config->Slope_Bottom / 2.0;
    auto totalHeight = config->Height + delta_y_top - delta_y_bottom;

    if (config->Radius_Bottom > 0 && config->Radius_Top > 0 &&
        totalHeight - 1 < config->Radius_Bottom + config->Radius_Top) {
      int32_t corrected_radius_top =
          (float)config->Radius_Top * (totalHeight - 1) /
          (config->Radius_Bottom + config->Radius_Top);
      int32_t corrected_radius_bottom =
          (float)config->Radius_Bottom * (totalHeight - 1) /
          (config->Radius_Bottom + config->Radius_Top);
      config->Radius_Top = corrected_radius_top;
      config->Radius_Bottom = corrected_radius_bottom;
    }

    int32_t TLc_y = centerY + config->OffsetY - config->Height/2
                    + config->Radius_Top - delta_y_top;
    int32_t TLc_x = centerX + config->OffsetX - config->Width/2
                    + config->Radius_Top;
    int32_t TRc_y = centerY + config->OffsetY - config->Height/2
                    + config->Radius_Top + delta_y_top;
    int32_t TRc_x = centerX + config->OffsetX + config->Width/2
                    - config->Radius_Top;
    int32_t BLc_y = centerY + config->OffsetY + config->Height/2
                    - config->Radius_Bottom - delta_y_bottom;
    int32_t BLc_x = centerX + config->OffsetX - config->Width/2
                    + config->Radius_Bottom;
    int32_t BRc_y = centerY + config->OffsetY + config->Height/2
                    - config->Radius_Bottom + delta_y_bottom;
    int32_t BRc_x = centerX + config->OffsetX + config->Width/2
                    - config->Radius_Bottom;

    int32_t min_c_x = min(TLc_x, BLc_x);
    int32_t max_c_x = max(TRc_x, BRc_x);
    int32_t min_c_y = min(TLc_y, TRc_y);
    int32_t max_c_y = max(BLc_y, BRc_y);

    FillRectangle(min_c_x, min_c_y, max_c_x, max_c_y, 1);
    FillRectangle(TRc_x, TRc_y, BRc_x + config->Radius_Bottom, BRc_y, 1);
    FillRectangle(TLc_x - config->Radius_Top, TLc_y, BLc_x, BLc_y, 1);
    FillRectangle(TLc_x, TLc_y - config->Radius_Top, TRc_x, TRc_y, 1);
    FillRectangle(BLc_x, BLc_y, BRc_x, BRc_y + config->Radius_Bottom, 1);

    if (config->Slope_Top > 0) {
      FillRectangularTriangle(TLc_x, TLc_y-config->Radius_Top,
                              TRc_x, TRc_y-config->Radius_Top, 0);
      FillRectangularTriangle(TRc_x, TRc_y-config->Radius_Top,
                              TLc_x, TLc_y-config->Radius_Top, 1);
    } else if (config->Slope_Top < 0) {
      FillRectangularTriangle(TRc_x, TRc_y-config->Radius_Top,
                              TLc_x, TLc_y-config->Radius_Top, 0);
      FillRectangularTriangle(TLc_x, TLc_y-config->Radius_Top,
                              TRc_x, TRc_y-config->Radius_Top, 1);
    }

    if (config->Slope_Bottom > 0) {
      FillRectangularTriangle(BRc_x+config->Radius_Bottom,
                              BRc_y+config->Radius_Bottom,
                              BLc_x-config->Radius_Bottom,
                              BLc_y+config->Radius_Bottom, 0);
      FillRectangularTriangle(BLc_x-config->Radius_Bottom,
                              BLc_y+config->Radius_Bottom,
                              BRc_x+config->Radius_Bottom,
                              BRc_y+config->Radius_Bottom, 1);
    } else if (config->Slope_Bottom < 0) {
      FillRectangularTriangle(BLc_x-config->Radius_Bottom,
                              BLc_y+config->Radius_Bottom,
                              BRc_x+config->Radius_Bottom,
                              BRc_y+config->Radius_Bottom, 0);
      FillRectangularTriangle(BRc_x+config->Radius_Bottom,
                              BRc_y+config->Radius_Bottom,
                              BLc_x-config->Radius_Bottom,
                              BLc_y+config->Radius_Bottom, 1);
    }

    if (config->Radius_Top > 0) {
      FillEllipseCorner(T_L, TLc_x, TLc_y,
                        config->Radius_Top, config->Radius_Top, 1);
      FillEllipseCorner(T_R, TRc_x, TRc_y,
                        config->Radius_Top, config->Radius_Top, 1);
    }

    if (config->Radius_Bottom > 0) {
      FillEllipseCorner(B_L, BLc_x, BLc_y,
                        config->Radius_Bottom, config->Radius_Bottom, 1);
      FillEllipseCorner(B_R, BRc_x, BRc_y,
                        config->Radius_Bottom, config->Radius_Bottom, 1);
    }
  }

  static void FillEllipseCorner(CornerType corner, int16_t x0, int16_t y0,
                                int32_t rx, int32_t ry, uint16_t color) {
    if (rx < 2 || ry < 2) return;

    int32_t x, y;
    int32_t rx2 = rx * rx;
    int32_t ry2 = ry * ry;
    int32_t fx2 = 4 * rx2;
    int32_t fy2 = 4 * ry2;
    int32_t s;

    for (x = 0, y = ry, s = 2 * ry2 + rx2 * (1 - 2 * ry);
         ry2 * x <= rx2 * y; x++) {
      int32_t len = x;
      int32_t sx = x0;
      int32_t sy = y0 - y;

      switch (corner) {
        case T_R: canvas.drawFastHLine(sx, sy, len, C(color)); break;
        case T_L: canvas.drawFastHLine(sx - len, sy, len, C(color)); break;
        case B_R: canvas.drawFastHLine(sx, y0 + y - 1, len, C(color)); break;
        case B_L: canvas.drawFastHLine(sx - len, y0 + y - 1, len, C(color)); break;
      }

      if (s >= 0) {
        s += fx2 * (1 - y);
        y--;
      }
      s += ry2 * ((4 * x) + 6);
    }

    for (x = rx, y = 0, s = 2 * rx2 + ry2 * (1 - 2 * rx);
         rx2 * y <= ry2 * x; y++) {
      int32_t len = x;
      int32_t sy = (corner == B_R || corner == B_L) ? y0 + y : y0 - y;

      switch (corner) {
        case T_R:
        case B_R:
          canvas.drawFastHLine(x0, sy, len, C(color));
          break;
        case T_L:
        case B_L:
          canvas.drawFastHLine(x0 - len, sy, len, C(color));
          break;
      }

      if (s >= 0) {
        s += fy2 * (1 - x);
        x--;
      }
      s += rx2 * ((4 * y) + 6);
    }
  }

  static void FillRectangle(int32_t x0, int32_t y0,
                            int32_t x1, int32_t y1, int32_t color) {
    int32_t l = min(x0, x1);
    int32_t r = max(x0, x1);
    int32_t t = min(y0, y1);
    int32_t b = max(y0, y1);
    if (r >= l && b >= t)
      canvas.fillRect(l, t, r - l + 1, b - t + 1, C(color));
  }

  static void FillRectangularTriangle(int32_t x0, int32_t y0,
                                      int32_t x1, int32_t y1,
                                      int32_t color) {
    canvas.fillTriangle(x0, y0, x1, y1, x1, y0, C(color));
  }

  static void FillTriangle(int32_t x0, int32_t y0,
                           int32_t x1, int32_t y1,
                           int32_t x2, int32_t y2, int32_t color) {
    canvas.fillTriangle(x0, y0, x1, y1, x2, y2, C(color));
  }
};

#endif
