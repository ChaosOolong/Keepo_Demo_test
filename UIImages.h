#ifndef UIIMAGES_H
#define UIIMAGES_H

#include <stdint.h>

// Main screen buttons exported from RGB565 .bin files.
// add / amend / outbound are confirmed as 66x45 RGB565 images.
static constexpr int IMG_MAIN_BTN_W = 66;
static constexpr int IMG_MAIN_BTN_H = 45;
static constexpr int IMG_MAIN_BTN_PIXEL_COUNT = IMG_MAIN_BTN_W * IMG_MAIN_BTN_H;

extern const uint16_t img_add_button[IMG_MAIN_BTN_PIXEL_COUNT];
extern const uint16_t img_amend_button[IMG_MAIN_BTN_PIXEL_COUNT];
extern const uint16_t img_outbound_button[IMG_MAIN_BTN_PIXEL_COUNT];

static constexpr int IMG_LONG_BTN_W = 107;
static constexpr int IMG_LONG_BTN_H = 32;
// Additional buttons exported from RGB565 .bin files.
// Current files contain 3424 RGB565 pixels each.
static constexpr int IMG_CONFIRM_BUTTON_PIXEL_COUNT = 3424;
static constexpr int IMG_RETURN_BUTTON_PIXEL_COUNT  = 3424;

extern const uint16_t img_confirm_button[IMG_CONFIRM_BUTTON_PIXEL_COUNT];
extern const uint16_t img_return_button[IMG_RETURN_BUTTON_PIXEL_COUNT];

// camera_button.bin was also converted, but its width/height still need confirmation.
static constexpr int IMG_CAMERA_BUTTON_PIXEL_COUNT = 5590;
extern const uint16_t img_camera_button_raw[IMG_CAMERA_BUTTON_PIXEL_COUNT];

#endif
