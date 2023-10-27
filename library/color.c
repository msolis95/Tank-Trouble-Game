#include "color.h"
#include "math.h"
#include <stdlib.h>

rgb_color_t hsv_to_rgb(hsv_color_t hsv) {
  float h = hsv.h; 
  float s = hsv.s; 
  float v = hsv.v;
  float max = v;
  float min = max - (s * max);
  float r;
  float g;
  float b;
  if (0 <= h && h <60) {
    r = max;
    g = (h / 60) * (max - min) + min;
    b = min;
  }
  if (60 <= h && h <120) {
    r = ((120 - h) / 60) * (max - min) + min;
    g = max;
    b = min;
    }
  if (120 <= h && h < 180) {
    r = min;
    g = max;
    b = ((h - 120) / 60) * (max - min) + min;
  }
  if (180 <= h && h < 240) {
    r = min;
    g = ((240 - h) / 60) * (max - min) + min;
    b = max;
  }
  if (240 <= h && h < 300) {
    r = ((h - 240) / 60) * (max - min) + min;
    g = min;
    b = max;
  }
  if (300 <= h && h < 360) {
    r = max;
    g = min;
    b = ((360 - h) / 60) * (max - min) + min;
  }
  return (rgb_color_t) {.r = r, .g = g, .b = b};
}

hsv_color_t rgb_to_hsv(rgb_color_t rgb) {
  double r = rgb.r;
  double g = rgb.g;
  double b = rgb.b;
  double max, min;
  double chroma;
  double h, s, v;
  max = fmax(r, fmax(g, b));
  min = fmin(r, fmin(g, b));
  chroma = max - min;
  if (chroma == 0)
      h = 0;
  else if (max == r)
      h = fmod((g - b) / chroma + 6.0, 6.0);
  else if (max == g)
      h = (b - r) / chroma + 2.0;
  else
      h = (r - g) / chroma + 4.0;
  h *= 60.0;
  s = chroma / max;
  v = max;
  return (hsv_color_t) {.h = h, .s = s, .v = v};
}