#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_examples/lv_apps/demo/demo.h"
#include "lv_examples/lv_tests/lv_test_group/lv_test_group.h"
#include "lv_examples/lv_tests/lv_test_objx/lv_test_img/lv_test_img.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <ev3dev.h>


using ev3dev::led;

#define DISP_BUF_SIZE (80*LV_HOR_RES_MAX)

lv_obj_t * left_roller,* right_roller;
lv_indev_state_t last_state = LV_INDEV_STATE_REL;

static bool ev3_evdev_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
	bool res = evdev_read(drv, data);
	if(data->state == LV_INDEV_STATE_PR && data->key == LV_KEY_BACKSPACE)
		exit(0);

	if(data->state != last_state) {
		last_state = data->state;
		lv_refr_now(lv_disp_get_default());
	}
	return res;
}


const std::vector<float> * colors[] = { &led::black, &led::red, &led::yellow, &led::orange, &led::green };
static void roller_event_cb(lv_obj_t * roller, lv_event_t event)
{
	printf("Event %d\n", event);
	if(event == LV_EVENT_VALUE_CHANGED) {
		std::vector<led*> *led_r;
		if(roller == left_roller)
			led_r = &led::left;
		else
			led_r = &led::right;
		const std::vector<float> *color = colors[lv_roller_get_selected(roller)];
		led::set_color(*led_r, *color);
	}
}

int main(void)
{
    /*LittlevGL init*/
    lv_init();

    /*Linux frame buffer device init*/
    fbdev_init();

    /*A small buffer for LittlevGL to draw the screen's content*/
    static lv_color_t buf[DISP_BUF_SIZE];

    /*Initialize a descriptor for the buffer*/
    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

    /*Initialize and register a display driver*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.buffer = &disp_buf;
    disp_drv.flush_cb = fbdev_flush;
    lv_disp_drv_register(&disp_drv);


    /*Initialize and register an input driver*/
    evdev_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = ev3_evdev_read;
    lv_indev_t *indev = lv_indev_drv_register(&indev_drv);

    /*Set the mono theme*/
    lv_theme_set_current(lv_theme_mono_init(0, NULL));

#if 1
    /*Create a screen and style */
    static lv_style_t screen_style;
    lv_obj_t * screen = lv_obj_create(NULL, NULL);
    lv_style_copy(&screen_style, &lv_style_scr);
    screen_style.body.main_color = screen_style.body.grad_color = lv_color_make(255, 255, 255);
    lv_obj_set_style(screen, &screen_style);
    lv_scr_load(screen);

    /*Create a simple label*/
    lv_obj_t * label = lv_label_create(screen, NULL);
    lv_label_set_text(label, "Hello LEGO EV3 World! This is LittlevGL running on ev3dev. Cool, right?");
    /*Align the label in the center of the screen*/
    lv_label_set_long_mode(label, LV_LABEL_LONG_SROLL_CIRC);
    lv_obj_set_width(label, LV_HOR_RES);
    lv_obj_align(label, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

    /*Create a button as well*/
    left_roller = lv_roller_create(screen, NULL);
    lv_roller_set_options(left_roller, "Off\nRed\nYellow\nOrange\nGreen", LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_fix_width(left_roller, 70);
    lv_roller_set_selected(left_roller, 0, false);
    lv_obj_set_event_cb(left_roller, roller_event_cb);

    right_roller = lv_roller_create(screen, left_roller);

    lv_obj_align(left_roller, NULL, LV_ALIGN_CENTER, -40, 0);
    lv_obj_align(right_roller, NULL, LV_ALIGN_CENTER, 40, 0);

    lv_group_t * g = lv_group_create();
    lv_group_add_obj(g, left_roller);
    lv_group_add_obj(g, right_roller);
    lv_indev_set_group(indev, g);
#endif

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_task_handler();
        usleep(10 * 1000);
    }

    return 0;
}


/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
    static uint64_t start_ms = 0;
    if(start_ms == 0) {
        struct timeval tv_start;
        gettimeofday(&tv_start, NULL);
        start_ms = (tv_start.tv_sec * 1000000 + tv_start.tv_usec) / 1000;
    }

    struct timeval tv_now;
    gettimeofday(&tv_now, NULL);
    uint64_t now_ms;
    now_ms = (tv_now.tv_sec * 1000000 + tv_now.tv_usec) / 1000;

    uint32_t time_ms = now_ms - start_ms;
    return time_ms;
}
