/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:14
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-16 17:16
 * @FilePath: \Software\main\Lvgl_Task\src\lvgl_init.c
 * @Description: 
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved. 
 */
#include "lvgl_init.h"

/******************************************************************
 *  STATIC PROTOTYPES
 ******************************************************************/
static const char *TAG = "lvgl";

lv_ui guider_ui;

void lv_tick_task(void *arg)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}
/* Creates a semaphore to handle concurrent call to lvgl stuff
 * If you wish to call *any* lvgl function from other threads/tasks
 * you should lock on the very same semaphore! */
/*********************** GUI_SHOW_CODE_START***********************/
static void set_value(void *bar, int32_t v)
{
    lv_bar_set_value(bar, v, LV_ANIM_OFF);
}

static void event_cb(lv_event_t * e)
{
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
    if(dsc->part != LV_PART_INDICATOR) return;

    lv_obj_t * obj= lv_event_get_target(e);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = LV_FONT_DEFAULT;

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", (int)lv_bar_get_value(obj));

    lv_point_t txt_size;
    lv_txt_get_size(&txt_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX, label_dsc.flag);

    lv_area_t txt_area;
    /*If the indicator is long enough put the text inside on the right*/
    if(lv_area_get_width(dsc->draw_area) > txt_size.x + 20) {
        txt_area.x2 = dsc->draw_area->x2 - 5;
        txt_area.x1 = txt_area.x2 - txt_size.x + 1;
        label_dsc.color = lv_color_white();
    }
    /*If the indicator is still short put the text out of it on the right*/
    else {
        txt_area.x1 = dsc->draw_area->x2 + 5;
        txt_area.x2 = txt_area.x1 + txt_size.x - 1;
        label_dsc.color = lv_color_black();
    }

    txt_area.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - txt_size.y) / 2;
    txt_area.y2 = txt_area.y1 + txt_size.y - 1;

    lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area, buf, NULL);
}

void Boot_UI_Init(lv_ui *ui)
{	
    lv_obj_t *background;
    lv_obj_t *label_1;
    lv_obj_t *bar;

    background = lv_scr_act();   /*Create a parent object on the current screen*/
    lv_obj_set_pos(background, 0, 0);
    lv_obj_set_size(background, 240, 240); /*Set the size of the parent*/
    lv_scr_load(background);

	//Write codes bg
	ui->bg = lv_img_create(background);
	lv_obj_set_pos(ui->bg, 0, 0);
	lv_obj_set_size(ui->bg, 240, 240);

	//Write style state: LV_STATE_DEFAULT for style_bg_main_main_default
	static lv_style_t style_bg_main_main_default;
	lv_style_reset(&style_bg_main_main_default);
	lv_style_set_img_recolor(&style_bg_main_main_default, lv_color_make(0x00, 0x00, 0x00));
	lv_style_set_img_recolor_opa(&style_bg_main_main_default, 0);
	lv_style_set_img_opa(&style_bg_main_main_default, 255);
	lv_obj_add_style(ui->bg, &style_bg_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->bg, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->bg,&_bg_240x240);
	lv_img_set_pivot(ui->bg, 0,0);
	lv_img_set_angle(ui->bg, 0);

    label_1 = lv_label_create(ui->bg);//创建label
    lv_label_set_recolor(label_1,1);//颜色可变换
    lv_label_set_long_mode(label_1,LV_LABEL_LONG_WRAP);//设置滚动模式
    // lv_obj_set_pos(label_1,50,50);//设置位置
    lv_obj_align(label_1, LV_ALIGN_TOP_MID, 0, 90);
    lv_label_set_text(label_1, "Hello Tian!");//设定文本内容

    bar = lv_bar_create(ui->bg);
    lv_obj_add_event_cb(bar, event_cb, LV_EVENT_DRAW_PART_END, NULL);
    lv_obj_set_size(bar, 200, 20);
    lv_obj_center(bar);

    lv_anim_t a;
    lv_anim_init(&a);//初始化动画
    lv_anim_set_var(&a, bar);
    lv_anim_set_values(&a, 0, 100);//开始和结束的值
    lv_anim_set_exec_cb(&a, set_value);//设置动画函数
    lv_anim_set_time(&a, 2000);//设置动画的持续时间
    //lv_anim_set_playback_time(&a, 1000);//恢复动画时间
    lv_anim_start(&a);

    lv_obj_del_delayed(label_1, 2200);
    lv_obj_del_delayed(bar, 2200);
}

void APP_UI_Init(void)
{
    setup_ui(&guider_ui);
}
/*********************** GUI_SHOW_CODE_END***********************/
void bootguiTask(void *pvParameter)
{
    lv_init();
    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();

    lv_color_t* buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    //memset(buf1,0x00ff,DISP_BUF_SIZE * sizeof(lv_color_t));
    assert(buf1 != NULL);
    /* Use double buffered when not working with monochrome displays */
    lv_color_t* buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    //memset(buf2,0x00ff,DISP_BUF_SIZE * sizeof(lv_color_t));
    assert(buf2 != NULL);

    static lv_disp_draw_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;

    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res=LV_HOR_RES_MAX;
    disp_drv.ver_res=LV_VER_RES_MAX;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);
    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));

    Boot_UI_Init(&guider_ui);//基础UI渲染函数
    vTaskDelay(2200 / portTICK_PERIOD_MS);

    free(buf1);
    free(buf2);

    xSemaphoreGive(StartSysInitSemaphore);
    vTaskDelete(NULL);
}

void appguiTask(void *pvParameter)
{
    lv_color_t* buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    //memset(buf1,0x00ff,DISP_BUF_SIZE * sizeof(lv_color_t));
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
    lv_color_t* buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    //memset(buf2,0x00ff,DISP_BUF_SIZE * sizeof(lv_color_t));
    assert(buf2 != NULL);

    static lv_disp_draw_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;
    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res=LV_HOR_RES_MAX;
    disp_drv.ver_res=LV_VER_RES_MAX;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"
    };
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
    ESP_LOGI(TAG, "Start APP UI Init ");

    APP_UI_Init();//基础UI渲染函数
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(100));
    }
    /* A task should NEVER return */
    free(buf1);
    free(buf2);
    vTaskDelete(NULL);
}