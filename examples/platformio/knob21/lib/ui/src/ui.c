// This file was adapted from the SquareLine export for the digital human selector.
#include "ui.h"
#include "ui_helpers.h"
#include "esp_log.h"

static const char *TAG = "ui";

#define UI_RESPONSE_RETURN_LOCK_MS 10000

uint8_t HF_ui_screen_id = 1;
uint8_t HF_open = 0;

lv_obj_t *ui_Screen1;
lv_obj_t *ui_background;
lv_obj_t *ui_select_feedback;
lv_obj_t *ui_back_button;
lv_obj_t *ui_back_label;
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

typedef enum {
    UI_MODE_SELECTING = 0,
    UI_MODE_RESPONSE,
} ui_mode_t;

#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match the generated avatar assets"
#endif
#if LV_COLOR_16_SWAP != 0
    #error "LV_COLOR_16_SWAP should be 0 for RGB LCD panels"
#endif

static const lv_img_dsc_t *avatar_images[] = {
    &ui_img_avatar_newton,
    &ui_img_avatar_einstein,
    &ui_img_avatar_curie,
    &ui_img_avatar_sushi,
    &ui_img_avatar_confucius,
};

static const lv_img_dsc_t *response_images[] = {
    &ui_img_response_newton,
    &ui_img_response_einstein,
    &ui_img_response_curie,
    &ui_img_response_sushi,
    &ui_img_response_confucius,
};

static int avatar_index = 0;
static ui_mode_t ui_mode = UI_MODE_SELECTING;
static uint8_t response_return_locked = 0;
static uint32_t response_return_lock_started_ms = 0;

static void ui_set_select_feedback_opa(void *obj, int32_t opa);

static int ui_avatar_count(void)
{
    return sizeof(avatar_images) / sizeof(avatar_images[0]);
}

static int ui_wrap_avatar_index(int index)
{
    int count = ui_avatar_count();
    return (index + count) % count;
}

static void ui_set_back_button_visible(int visible)
{
    if (ui_back_button == NULL) {
        return;
    }

    if (visible) {
        lv_obj_clear_flag(ui_back_button, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(ui_back_button);
    } else {
        lv_obj_add_flag(ui_back_button, LV_OBJ_FLAG_HIDDEN);
    }
}

static void ui_show_avatar(int index)
{
    avatar_index = ui_wrap_avatar_index(index);
    ui_mode = UI_MODE_SELECTING;

    if (ui_background != NULL) {
        lv_img_set_src(ui_background, avatar_images[avatar_index]);
    }

    ui_set_back_button_visible(0);
}

static void ui_set_avatar_index(int index)
{
    avatar_index = ui_wrap_avatar_index(index);
}

static void ui_refresh_avatar_image(void)
{
    if (ui_background != NULL) {
        lv_img_set_src(ui_background, avatar_images[avatar_index]);
    }
}

static void ui_start_response_return_lock(void)
{
    response_return_locked = 1;
    response_return_lock_started_ms = lv_tick_get();
}

static int ui_is_response_return_locked(void)
{
    if (!response_return_locked) {
        return 0;
    }

    if (lv_tick_elaps(response_return_lock_started_ms) < UI_RESPONSE_RETURN_LOCK_MS) {
        return 1;
    }

    response_return_locked = 0;
    return 0;
}

static void ui_show_response(void)
{
    ui_mode = UI_MODE_RESPONSE;
    ui_start_response_return_lock();

    if (ui_select_feedback != NULL) {
        lv_anim_del(ui_select_feedback, ui_set_select_feedback_opa);
        lv_obj_add_flag(ui_select_feedback, LV_OBJ_FLAG_HIDDEN);
        ui_set_select_feedback_opa(ui_select_feedback, 255);
    }

    if (ui_background != NULL) {
        lv_img_set_src(ui_background, response_images[avatar_index]);
    }

    ui_set_back_button_visible(1);
}

static void ui_return_to_selection(void)
{
    ui_show_avatar(avatar_index);
}

void ui_select_avatar(int index)
{
    ui_show_avatar(index);
}

int ui_get_avatar_index(void)
{
    return avatar_index;
}

static void ui_next_avatar(void)
{
    ui_show_avatar(avatar_index + 1);
}

static void ui_prev_avatar(void)
{
    ui_show_avatar(avatar_index - 1);
}

static void ui_set_select_feedback_opa(void *obj, int32_t opa)
{
    lv_obj_t *target = (lv_obj_t *)obj;
    lv_obj_set_style_opa(target, (lv_opa_t)opa, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(target, (lv_opa_t)opa, LV_PART_INDICATOR | LV_STATE_DEFAULT);
}

static void ui_hide_select_feedback(lv_anim_t *anim)
{
    lv_obj_t *target = (lv_obj_t *)anim->var;
    lv_obj_add_flag(target, LV_OBJ_FLAG_HIDDEN);
    ui_set_select_feedback_opa(target, 255);
}

static void ui_show_select_feedback(void)
{
    if (ui_select_feedback == NULL) {
        return;
    }

    lv_anim_del(ui_select_feedback, ui_set_select_feedback_opa);
    ui_set_select_feedback_opa(ui_select_feedback, 255);
    lv_obj_clear_flag(ui_select_feedback, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(ui_select_feedback);

    lv_anim_t fade;
    lv_anim_init(&fade);
    lv_anim_set_var(&fade, ui_select_feedback);
    lv_anim_set_exec_cb(&fade, ui_set_select_feedback_opa);
    lv_anim_set_values(&fade, 255, 0);
    lv_anim_set_delay(&fade, 1000);
    lv_anim_set_time(&fade, 180);
    lv_anim_set_ready_cb(&fade, ui_hide_select_feedback);
    lv_anim_start(&fade);
}

void LVGL_knob_event(void *event)
{
    ESP_LOGI(TAG, "Knob event: %d", (int)event);
    HF_ui_screen_id = 1;
    HF_open = 0;

    if (ui_mode == UI_MODE_RESPONSE) {
        device_event_emit("knob", "ignored_response", (int)event, avatar_index);
        return;
    }

    if ((int)event == KNOB_LEFT) {
        ui_set_avatar_index(avatar_index - 1);
        device_event_emit("knob", "left", (int)event, avatar_index);
        ui_refresh_avatar_image();
    } else if ((int)event == KNOB_RIGHT) {
        ui_set_avatar_index(avatar_index + 1);
        device_event_emit("knob", "right", (int)event, avatar_index);
        ui_refresh_avatar_image();
    }
}

void LVGL_button_event(void *event)
{
    ESP_LOGI(TAG, "Button event: %d", (int)event);

    if ((int)event == BUTTON_PRESS_DOWN) {
        if (ui_mode == UI_MODE_SELECTING) {
            device_event_emit("button", "select", (int)event, avatar_index);
            ui_show_response();
        } else {
            device_event_emit("button", "ignored_response", (int)event, avatar_index);
        }
    }
}

void ui_event_background(lv_event_t *e)
{
    (void)e;
}

void ui_event_back_button(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_PRESSED && ui_mode == UI_MODE_RESPONSE) {
        if (ui_is_response_return_locked()) {
            device_event_emit("touch", "ignored_return_lock", 0, avatar_index);
            return;
        }

        ui_return_to_selection();
        device_event_emit("touch", "back", 0, avatar_index);
    }
}

void ui_event_Button1(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED && ui_mode == UI_MODE_SELECTING) {
        ui_next_avatar();
        device_event_emit("touch", "button_next", 0, avatar_index);
    }
}

void ui_event_Button2(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED && ui_mode == UI_MODE_SELECTING) {
        ui_prev_avatar();
        device_event_emit("touch", "button_prev", 0, avatar_index);
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
    if (lv_event_get_code(e) == LV_EVENT_CLICKED && ui_mode == UI_MODE_SELECTING) {
        ui_prev_avatar();
        device_event_emit("touch", "button_prev", 0, avatar_index);
    }
}

void ui_event_Button5(lv_event_t *e)
{
    if (lv_event_get_code(e) == LV_EVENT_CLICKED && ui_mode == UI_MODE_SELECTING) {
        ui_next_avatar();
        device_event_emit("touch", "button_next", 0, avatar_index);
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
