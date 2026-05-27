// This file was adapted from the SquareLine export for the digital human selector.
#include "../ui.h"

void ui_Screen1_screen_init(void)
{
    ui_Screen1 = lv_obj_create(NULL);
    lv_obj_clear_flag(ui_Screen1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(ui_Screen1, lv_color_hex(0xF5F8FC), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Screen1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_background = lv_img_create(ui_Screen1);
    lv_img_set_src(ui_background, &ui_img_avatar_curie);
    lv_obj_set_width(ui_background, 480);
    lv_obj_set_height(ui_background, 480);
    lv_obj_set_align(ui_background, LV_ALIGN_CENTER);
    lv_obj_add_flag(ui_background, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(ui_background, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(ui_background, ui_event_background, LV_EVENT_ALL, NULL);

    ui_Image1 = NULL;
    ui_Button1 = NULL;
    ui_Button2 = NULL;
}
