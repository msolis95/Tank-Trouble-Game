#ifndef __COLOR_H__
#define __COLOR_H__

/**
 * A color to display on the screen.
 * The color is represented by its red, green, and blue components.
 * Each component must be between 0 (black) and 1 (white).
 */
typedef struct {
    float r;
    float g;
    float b;
} rgb_color_t;

typedef struct {
    float h;
    float s;
    float v;
} hsv_color_t;

/**
 * Converts hsv into rgb
 * @param h 0~360
 * @param s 0~1
 * @param v 0~1
 * @return rgb in 0~1
 */
rgb_color_t hsv_to_rgb(hsv_color_t hsv);

hsv_color_t rgb_to_hsv(rgb_color_t rgb);

#endif // #ifndef __COLOR_H__
