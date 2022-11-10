/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:14
 * @LastEditors: error: git config user.name & please set dead value or install git
 * @LastEditTime: 2022-10-28 15:23
 * @FilePath: \lvgl_menu_wifi\main\Lvgl_Task\src\lvgl_init.c
 * @Description:
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved.
 */
#include "lvgl_init.h"
#include "app_sem_queue.h"
#include "button.h"

/******************************************************************
 *  STATIC PROTOTYPES
 ******************************************************************/
static const char *TAG = "lvgl";

lv_ui guider_ui;

const void* spaceman_img[14]={&astronaut1,&astronaut2,&astronaut3,&astronaut4,&astronaut5,&astronaut6,&astronaut7,&astronaut8,&astronaut9,&astronaut10,&astronaut11,&astronaut12,&astronaut13,&astronaut14};


void lv_tick_task(void *arg)
{
    lv_tick_inc(LV_TICK_PERIOD_MS);
}

void APP_UI_Init(void)
{
    setup_ui(&guider_ui);
}

void Menu_Switch(void)
{
    switch (Button_Value)
    {
    case BT1_DOWN:
        xEventGroupSetBits(Key_Event_Group, BT1SHORTEVENT);
        break;
    case BT2_DOWN:
        xEventGroupSetBits(Key_Event_Group, BT2SHORTEVENT);
        break;
    case BT1_DOUBLE:
        xEventGroupSetBits(Key_Event_Group, BT1DOUBLEEVENT);
        break;
    case BT2_DOUBLE:
        xEventGroupSetBits(Key_Event_Group, BT2DOUBLEEVENT);
        break;
    case BT1_LONG:
        xEventGroupSetBits(Key_Event_Group, BT1LONGEVENT);
        break;
    case BT2_LONG:
        xEventGroupSetBits(Key_Event_Group, BT2LONGEVENT);
        break;
    default:
        break;
    }
    Button_Value = 0;
}

void appguiTask(void *pvParameter)
{
    lv_init();
    /* Initialize SPI or I2C bus used by the drivers */
    lvgl_driver_init();
    lv_color_t *buf1 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    // memset(buf1,0x00ff,DISP_BUF_SIZE * sizeof(lv_color_t));
    assert(buf1 != NULL);

    /* Use double buffered when not working with monochrome displays */
    lv_color_t *buf2 = heap_caps_malloc(DISP_BUF_SIZE * sizeof(lv_color_t), MALLOC_CAP_DMA);
    // memset(buf2,0x00ff,DISP_BUF_SIZE * sizeof(lv_color_t));
    assert(buf2 != NULL);

    static lv_disp_draw_buf_t disp_buf;
    uint32_t size_in_px = DISP_BUF_SIZE;
    /* Initialize the working buffer depending on the selected display.
     * NOTE: buf2 == NULL when using monochrome displays. */
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, size_in_px);

    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = disp_driver_flush;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);

    /* Create and start a periodic timer interrupt to call lv_tick_inc */
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &lv_tick_task,
        .name = "periodic_gui"};
    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000));
    ESP_LOGI(TAG, "Start APP UI Init ");
    APP_UI_Init(); //基础UI渲染函数
    while (1)
    {
        if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
        {
            Menu_Switch();
            lv_task_handler();
            xSemaphoreGive(xGuiSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    /* A task should NEVER return */
    free(buf1);
    free(buf2);
    vTaskDelete(NULL);
}