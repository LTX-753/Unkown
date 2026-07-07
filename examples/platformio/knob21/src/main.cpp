/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <Arduino.h>
#include <atomic>
#include <esp_display_panel.hpp>
#include <lvgl.h>
#include "lvgl_v8_port.h"

#include <ESP_Knob.h>
#include <Button.h>
#include <ui.h>

/**
/* To use the built-in examples and demos of LVGL uncomment the includes below respectively.
 * You also need to copy `lvgl/examples` to `lvgl/src/examples`. Similarly for the demos `lvgl/demos` to `lvgl/src/demos`.
 */
// #include <demos/lv_demos.h>
// #include <examples/lv_examples.h>

#define GPIO_NUM_KNOB_PIN_A     6
#define GPIO_NUM_KNOB_PIN_B     5
#define GPIO_BUTTON_PIN         GPIO_NUM_0

using namespace esp_panel::drivers;
using namespace esp_panel::board;

/*Knob event definition*/
ESP_Knob *knob;
static uint32_t event_sequence = 0;
static std::atomic<uint32_t> pending_button_press_down {0};

static uint32_t next_event_sequence()
{
    return ++event_sequence;
}

extern "C" void device_event_emit(const char *source, const char *action, int value, int avatar_index)
{
    static const char *avatar_names[] = {
        "zhuge_liang",
        "einstein",
        "curie",
        "sushi",
        "confucius",
    };
    const int avatar_count = sizeof(avatar_names) / sizeof(avatar_names[0]);
    const char *name = (avatar_index >= 0 && avatar_index < avatar_count) ? avatar_names[avatar_index] : "unknown";

    Serial.printf(
        "{\"type\":\"input\",\"seq\":%lu,\"ms\":%lu,\"source\":\"%s\",\"action\":\"%s\",\"value\":%d,\"avatar\":%d,\"name\":\"%s\"}\n",
        (unsigned long)next_event_sequence(),
        (unsigned long)millis(),
        source,
        action,
        value,
        avatar_index,
        name
    );
}

static void emit_device_status(const char *event)
{
    Serial.printf(
        "{\"type\":\"device\",\"seq\":%lu,\"ms\":%lu,\"event\":\"%s\",\"model\":\"UEDX48480021-MD80ET\",\"avatar\":%d,\"baud\":115200}\n",
        (unsigned long)next_event_sequence(),
        (unsigned long)millis(),
        event,
        ui_get_avatar_index()
    );
}

static void handle_serial_command(const String &raw_command)
{
    String command = raw_command;
    command.trim();
    command.toLowerCase();

    if (command.length() == 0) {
        return;
    }

    if (command == "ping") {
        emit_device_status("pong");
        return;
    }

    if (command == "status") {
        emit_device_status("status");
        return;
    }

    if (command == "next" || command == "prev" || command.startsWith("select:")) {
        lvgl_port_lock(-1);
        if (command == "next") {
            ui_select_avatar(ui_get_avatar_index() + 1);
            device_event_emit("serial", "next", 0, ui_get_avatar_index());
        } else if (command == "prev") {
            ui_select_avatar(ui_get_avatar_index() - 1);
            device_event_emit("serial", "prev", 0, ui_get_avatar_index());
        } else {
            int index = command.substring(7).toInt();
            ui_select_avatar(index);
            device_event_emit("serial", "select", index, ui_get_avatar_index());
        }
        lvgl_port_unlock();
        return;
    }

    Serial.printf("{\"type\":\"error\",\"message\":\"unknown_command\",\"command\":\"%s\"}\n", command.c_str());
}

void onKnobLeftEventCallback(int count, void *usr_data)
{
    lvgl_port_lock(-1);
    LVGL_knob_event((void*)KNOB_RIGHT);
    lvgl_port_unlock();
}

void onKnobRightEventCallback(int count, void *usr_data)
{
    lvgl_port_lock(-1);
    LVGL_knob_event((void*)KNOB_LEFT);
    lvgl_port_unlock();
}

static void PressDownCb(void *button_handle, void *usr_data)
{
    (void)button_handle;
    (void)usr_data;
    pending_button_press_down.fetch_add(1, std::memory_order_relaxed);
}

static void process_pending_button_events()
{
    uint32_t press_count = pending_button_press_down.exchange(0, std::memory_order_relaxed);

    while (press_count-- > 0) {
        lvgl_port_lock(-1);
        LVGL_button_event((void*)BUTTON_PRESS_DOWN);
        lvgl_port_unlock();
    }
}

static void SingleClickCb(void *button_handle, void *usr_data) {
    lvgl_port_lock(-1);
    LVGL_button_event((void*)BUTTON_SINGLE_CLICK);
    lvgl_port_unlock();
}

static void DoubleClickCb(void *button_handle, void *usr_data)
{
    lvgl_port_lock(-1);
    LVGL_button_event((void*)BUTTON_DOUBLE_CLICK);
    lvgl_port_unlock();
}

static void LongPressStartCb(void *button_handle, void *usr_data) {
    lvgl_port_lock(-1);
    LVGL_button_event((void*)BUTTON_LONG_PRESS_START);
    lvgl_port_unlock();
}

void setup()
{
    Serial.begin(115200);

    emit_device_status("boot");
    Board *board = new Board();
    board->init();
#if LVGL_PORT_AVOID_TEARING_MODE
    auto lcd = board->getLCD();
    // When avoid tearing function is enabled, the frame buffer number should be set in the board driver
    lcd->configFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
#if ESP_PANEL_DRIVERS_BUS_ENABLE_RGB && CONFIG_IDF_TARGET_ESP32S3
    auto lcd_bus = lcd->getBus();
    /**
     * As the anti-tearing feature typically consumes more PSRAM bandwidth, for the ESP32-S3, we need to utilize the
     * "bounce buffer" functionality to enhance the RGB data bandwidth.
     * This feature will consume `bounce_buffer_size * bytes_per_pixel * 2` of SRAM memory.
     */
    if (lcd_bus->getBasicAttributes().type == ESP_PANEL_BUS_TYPE_RGB) {
        static_cast<BusRGB *>(lcd_bus)->configRGB_BounceBufferSize(lcd->getFrameWidth() * 10);
    }
#endif
#endif
    assert(board->begin());

    lvgl_port_init(board->getLCD(), board->getTouch());

    /*knob initialization*/
    knob = new ESP_Knob(GPIO_NUM_KNOB_PIN_A, GPIO_NUM_KNOB_PIN_B);
    knob->begin();
    knob->attachLeftEventCallback(onKnobLeftEventCallback);
    knob->attachRightEventCallback(onKnobRightEventCallback);

    Button *btn = new Button(GPIO_BUTTON_PIN, false);
    btn->attachPressDownEventCb(&PressDownCb, NULL);

    /* Lock the mutex due to the LVGL APIs are not thread-safe */
    lvgl_port_lock(-1);

    /**
     * Create the simple labels
     */
    // lv_obj_t *label_1 = lv_label_create(lv_scr_act());
    // lv_label_set_text(label_1, "Hello World!");
    // lv_obj_set_style_text_font(label_1, &lv_font_montserrat_30, 0);
    // lv_obj_align(label_1, LV_ALIGN_CENTER, 0, -20);
    // lv_obj_t *label_2 = lv_label_create(lv_scr_act());
    // lv_label_set_text_fmt(
    //     label_2, "ESP32_Display_Panel(%d.%d.%d)",
    //     ESP_PANEL_VERSION_MAJOR, ESP_PANEL_VERSION_MINOR, ESP_PANEL_VERSION_PATCH
    // );
    // lv_obj_set_style_text_font(label_2, &lv_font_montserrat_16, 0);
    // lv_obj_align_to(label_2, label_1, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    // lv_obj_t *label_3 = lv_label_create(lv_scr_act());
    // lv_label_set_text_fmt(label_3, "LVGL(%d.%d.%d)", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
    // lv_obj_set_style_text_font(label_3, &lv_font_montserrat_16, 0);
    // lv_obj_align_to(label_3, label_2, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    /**
     * Try an example. Don't forget to uncomment header.
     * See all the examples online: https://docs.lvgl.io/master/examples.html
     * source codes: https://github.com/lvgl/lvgl/tree/e7f88efa5853128bf871dde335c0ca8da9eb7731/examples
     */
    //  lv_example_btn_1();

    /**
     * Or try out a demo.
     * Don't forget to uncomment header and enable the demos in `lv_conf.h`. E.g. `LV_USE_DEMO_WIDGETS`
     */
    // lv_demo_widgets();
    // lv_demo_benchmark();
    // lv_demo_music();
    // lv_demo_stress();
    ui_init();

    /* Release the mutex */
    lvgl_port_unlock();
}

void loop()
{
    static bool host_connected = false;
    bool connected = Serial;

    if (connected && !host_connected) {
        emit_device_status("ready");
    }
    host_connected = connected;

    while (Serial.available() > 0) {
        handle_serial_command(Serial.readStringUntil('\n'));
    }

    process_pending_button_events();

    delay(20);
}
