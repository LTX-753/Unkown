// This file was adapted from the SquareLine export for the digital human selector.
#include "../ui.h"

void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Screen1, lv_color_hex(0xF5F8FC), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Screen1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_background = lv_img_create(ui_Screen1);
    lv_img_set_src(ui_background, &ui_img_avatar_newton);
    lv_obj_set_width(ui_background, 480);
    lv_obj_set_height(ui_background, 480);
    lv_obj_set_align(ui_background, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_background, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_background, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(ui_background, ui_event_background, LV_EVENT_ALL, NULL);

    ui_select_feedback = lv_spinner_create(ui_Screen1, 1000, 90);
    lv_obj_set_size(ui_select_feedback, 480, 480);
    lv_obj_set_align(ui_select_feedback, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_select_feedback, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_select_feedback, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_arc_color(ui_select_feedback, lv_color_hex(0xE64500), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_opa(ui_select_feedback, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_arc_width(ui_select_feedback, 20, LV_PART_INDICATOR | LV_STATE_DEFAULT);

    ui_back_button = lv_btn_create(ui_Screen1);
    lv_obj_set_size(ui_back_button, 76, 76);
    lv_obj_align(ui_back_button, LV_ALIGN_LEFT_MID, 16, 0);
    lv_obj_add_flag(ui_back_button, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_back_button, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_clear_flag(ui_back_button, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_ext_click_area(ui_back_button, 18);
    lv_obj_set_style_radius(ui_back_button, 38, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui_back_button, lv_color_hex(0xE64500), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_back_button, 245, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_back_button, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_add_event_cb(ui_back_button, ui_event_back_button, LV_EVENT_ALL, NULL);

    ui_back_label = lv_label_create(ui_back_button);
    lv_label_set_text(ui_back_label, "<");
    lv_obj_center(ui_back_label);
    lv_obj_set_style_text_color(ui_back_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui_back_label, &lv_font_montserrat_40, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Image1 = NULL;
    ui_Button1 = NULL;
    ui_Button2 = NULL;
}
