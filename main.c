#include "lvgl/lvgl.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"
#include "lv_examples/lv_apps/demo/demo.h"
#include "lv_examples/lv_tests/lv_test_group/lv_test_group.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

#define DISP_BUF_SIZE (80*LV_HOR_RES_MAX)


static void general_event_handler(lv_obj_t * obj, lv_event_t event)
{
    (void) obj; /*Unused*/

    switch(event) {
        case LV_EVENT_PRESSED:
            printf("Pressed\n");
            break;

        case LV_EVENT_SHORT_CLICKED:
            printf("Short clicked\n");
            break;

        case LV_EVENT_CLICKED:
            printf("Clicked\n");
            break;

        case LV_EVENT_LONG_PRESSED:
            printf("Long press\n");
            break;

        case LV_EVENT_LONG_PRESSED_REPEAT:
            printf("Long press repeat\n");
            break;

        case LV_EVENT_VALUE_CHANGED:
            printf("Value changed: %s\n", lv_event_get_data() ? (const char *)lv_event_get_data() : "");
            break;

        case LV_EVENT_RELEASED:
            printf("Released\n");
            break;

        case LV_EVENT_DRAG_BEGIN:
            printf("Drag begin\n");
            break;

        case LV_EVENT_DRAG_END:
            printf("Drag end\n");
            break;

        case LV_EVENT_DRAG_THROW_BEGIN:
            printf("Drag throw begin\n");
            break;

        case LV_EVENT_FOCUSED:
            printf("Focused\n");
            break;
        case LV_EVENT_DEFOCUSED:
            printf("Defocused\n");
            break;
        default:
            break;
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

#if 1
    evdev_init();
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = evdev_read;
    lv_indev_t *indev = lv_indev_drv_register(&indev_drv);
#endif

//    lv_theme_set_current(lv_theme_mono_init(0, NULL));

    printf("%d by %d\n", lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
//    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, 0);
    /*Create a Demo*/
//    demo_create();

#if 0
    lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(btn, 50, 50);
    lv_obj_set_pos(btn, 0, 0);
    lv_obj_set_event_cb(btn, general_event_handler);
    lv_obj_t * btn2 = lv_btn_create(lv_scr_act(), btn2);
    lv_obj_set_pos(btn2, 100, 10);
    lv_obj_set_event_cb(btn2, general_event_handler);
    lv_group_t *group = lv_group_create();
    lv_group_add_obj(group, btn);
    lv_group_add_obj(group, btn2);
#endif
    lv_indev_set_group(indev,  lv_test_group_1());

    /*Handle LitlevGL tasks (tickless mode)*/
    while(1) {
        lv_task_handler();
        usleep(5000);
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
