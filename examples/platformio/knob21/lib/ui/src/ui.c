// This file was adapted from the SquareLine export for the digital human selector.
#include "ui.h"
#include "ui_helpers.h"
#include "esp_log.h"

static const char *TAG = "ui";

uint8_t HF_ui_screen_id = 1;
uint8_t HF_open = 0;

lv_obj_t *ui_Screen1;
lv_obj_t *ui_background;
lv_obj_t *ui_Image1;
lv_obj_t *ui_Button1;
lv_obj_t *ui_Button2;

lv_obj_t *ui_time;
lv_obj_t *ui_Roller1;
lv_obj_t *ui_Button4;
lv_obj_t *ui_Button5;
lv_obj_t *ui_Image3;
lv_obj_t *ui_Image4;

lv_obj_t *ui_working;
lv_obj_t *ui_Spinner1;
lv_obj_t *ui_Bar1;
lv_obj_t *ui_Button6;
lv_obj_t *ui_Label1;
lv_obj_t *ui_Label2;
lv_obj_t *ui_Label3;

lv_obj_t *ui____initial_actions0;

typedef enum {
    KNOB_LEFT = 0,
    KNOB_RIGHT,
    KNOB_H_LIM,
    KNOB_L_LIM,
    KNOB_ZERO,
    KNOB_EVENT_MAX,
    KNOB_NONE,
} knob_event_t;

typedef enum {
    BUTTON_PRESS_DOWN = 0,
    BUTTON_PRESS_UP,
    BUTTON_PRESS_REPEAT,
    BUTTON_PRESS_REPEAT_DONE,
    BUTTON_SINGLE_CLICK,
    BUTTON_DOUBLE_CLICK,
    BUTTON_MULTIPLE_CLICK,
    BUTTON_LONG_PRESS_START,
    BUTTON_LONG_PRESS_HOLD,
    BUTTON_LONG_PRESS_UP,
    BUTTON_PRESS_END,
    BUTTON_EVENT_MAX,
    BUTTON_NONE_PRESS,
} button_event_t;

#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match the generated avatar assets"
#endif
#if LV_COLOR_16_SWAP != 0
    #error "LV_COLOR_16_SWAP should be 0 for RGB LCD panels"
#endif

static const lv_img_dsc_t *avatar_images[] = {
    &ui_img_avatar_curie,
    &ui_img_avatar_einstein,
    &ui_img_avatar_confucius,
    &ui_img_avatar_newton,
    &ui_img_avatar_sushi,
};

static int avatar_index = 0;

static void ui_show_avatar(int index)
{
    int count = sizeof(avatar_images) / sizeof(avatar_images[0]);
    avatar_index = (index + count) % count;

    if (ui_background != NULL) {
        lv_img_set_src(ui_background, avatar_images[avatar_index]);
    }
}

static void ui_next_avatar(void)
{
    ui_show_avatar(avatar_index + 1);
}

static void ui_prev_avatar(void)
{
    ui_show_avatar(avatar_index - 1);
}

void LVGL_knob_event(void *event)
{
    ESP_LOGI(TAG, "Knob event: %d", (int)event);
    HF_ui_screen_id = 1;
    HF_open = 0;

    if ((int)event == KNOB_LEFT) {
        ui_prev_avatar();
    } else if ((int)event == KNOB_RIGHT) {
        ui_next_avatar();
    }
}

void LVGL_button_event(void *event)
{
    ESP_LOGI(TAG, "Button event: %d", (int)event);

    if ((int)event == BUTTON_SINGLE_CLICK) {
        ui_next_avatar();
    } else if ((int)event == BUTTON_LONG_PRESS_START) {
        ui_prev_avatar();
    }
}

void ui_event_background(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_next_avatar();
    }
}

void ui_event_Button1(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_next_avatar();
    }
}

void ui_event_Button2(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_prev_avatar();
    }
}

void ui_event_time(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_show_avatar(avatar_index);
    }
}

void ui_event_Button4(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_prev_avatar();
    }
}

void ui_event_Button5(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_next_avatar();
    }
}

void ui_event_Button6(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        ui_show_avatar(avatar_index);
    }
}

void ui_init(void)
{
    lv_disp_t *dispp = lv_disp_get_default();
    lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_CYAN),
                                              true, LV_FONT_DEFAULT);
    lv_disp_set_theme(dispp, theme);

    lv_scr_load(lv_obj_create(NULL));
    ui_Screen1_screen_init();
    ui____initial_actions0 = lv_obj_create(NULL);
    lv_disp_load_scr(ui_Screen1);
}
