/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:12
 * @LastEditors: error: git config user.name & please set dead value or install git
 * @LastEditTime: 2022-11-08 23:30
 * @FilePath: \lvgl_menu_wifi\main\Os_Task\src\app_task.c
 * @Description:
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "lvgl/lvgl.h"
#include "lvgl_init.h"
#include "easyio.h"
#include "button.h"
#include "gui_guider.h"
#include "app_sem_queue.h"
#include "button.h"
#include "app_task.h"
#include "driver/adc.h"
#include "esp_dsp.h"

extern TaskHandle_t Key_Scan_Task_Handler;
extern TaskHandle_t Key_Select_Task_Handler;
extern TaskHandle_t New_page_Task_Handler;
extern TaskHandle_t Open_wifi_Task_Handler;
extern TaskHandle_t Spase_man_Task_Handler;
extern TaskHandle_t readadc_Task_Handler;
extern TaskHandle_t fft_Task_Handler;
extern char Select_WIFI[32];
extern char *WIFIMima;
struct lock Task_lock = {0};
static const char *TAG = "Task";

/* menu */
/*************
 *   Y       *
 * X   0 1 2 *
 *   0 * *   *
 *   1 * * * *
 *   2 * *   *
 *   3 * *   *
 *   4 * *   *
 *************/
Menu page_Menu = {
    .x.max = 1,
    .x.min = 0,
    .x.num = 0,
    .y.max = 4,
    .y.min = 0,
    .y.num = 0,
};



void Key_Select_thread_entry(void *p)
{
    static EventBits_t EventValue;
    while (1)
    {
        EventValue = xEventGroupWaitBits(Key_Event_Group, BTALLEVENT, pdTRUE, pdFALSE, portMAX_DELAY);
        switch (EventValue)
        {
        case BT1SHORTEVENT:
            // printf("1short\n");
            if (page_Menu.x.num == 1) //第二列
            {
                if (page_Menu.y.num == pagewifi)
                {
                    lv_group_send_data(guider_ui.group_wifi, LV_KEY_RIGHT);
                }
            }
            else if (page_Menu.x.num == 2) //第三列
            {
                if (page_Menu.y.num == pagewifi)
                {
                    lv_group_send_data(guider_ui.group_key, LV_KEY_RIGHT);
                }
            }
            else
            {
                page_Menu.y.num++;
                if (page_Menu.y.num > page_Menu.y.max)
                    page_Menu.y.num = page_Menu.y.min;
                ESP_LOGI(TAG,"x = %d y = %d", page_Menu.x.num, page_Menu.y.num);
                lv_obj_set_tile_id(guider_ui.tv, page_Menu.x.num, page_Menu.y.num, LV_ANIM_ON);
            }
            break;
        case BT2SHORTEVENT:
            //printf("2short\n");
            if (page_Menu.x.num == 1) //第二列
            {
                if (page_Menu.y.num == pagewifi)
                {
                    lv_group_send_data(guider_ui.group_wifi, LV_KEY_LEFT);
                }
            }
            else if (page_Menu.x.num == 2) //第三列
            {
                if (page_Menu.y.num == pagewifi)
                {
                    lv_group_send_data(guider_ui.group_key, LV_KEY_LEFT);
                }
            }
            else
            {
                page_Menu.y.num--;
                if (page_Menu.y.num > page_Menu.y.max)
                    page_Menu.y.num = page_Menu.y.max;
                ESP_LOGI(TAG,"x = %d y = %d", page_Menu.x.num, page_Menu.y.num);
                lv_obj_set_tile_id(guider_ui.tv, page_Menu.x.num, page_Menu.y.num, LV_ANIM_ON);
            }
            break;
        case BT1LONGEVENT://跳转界面 发送事件的号
            // printf("1long\n");
            // printf("x = %d y = %d\n", page_Menu.x.num, page_Menu.y.num);
            page_Menu.x.num++;
            if(page_Menu.y.num == pageweather || page_Menu.y.num == pagegame)//天气又两个页面
            {
                page_Menu.x.max = 2;
            }
            if (page_Menu.x.num > page_Menu.x.max)
            {
                page_Menu.x.num = page_Menu.x.max;
            }
            ESP_LOGI(TAG,"x = %d y = %d", page_Menu.x.num, page_Menu.y.num);
            lv_obj_set_tile_id(guider_ui.tv, page_Menu.x.num, page_Menu.y.num, LV_ANIM_ON);
            
            vTaskDelay(800 / portTICK_PERIOD_MS);//先进入新界面防止卡顿   防止太空人失败重启
            xQueueSend(Page_Event_Queue, &page_Menu.y.num, 0);//y.num = page
            break;
        case BT2LONGEVENT://退出界面
            // printf("2long\n");
            // printf("x = %d y = %d\n", page_Menu.x.num, page_Menu.y.num);
            page_Menu.x.num--;
            if (page_Menu.x.num < page_Menu.x.min )
            {
                page_Menu.x.num = page_Menu.x.min;
            }
            if(page_Menu.x.num > page_Menu.x.max)
                page_Menu.x.num = page_Menu.x.min;
            ESP_LOGI(TAG,"New page ");
            ESP_LOGI(TAG,"x = %d y = %d", page_Menu.x.num, page_Menu.y.num);

            if (Task_lock.page_spaceman_lock == 1)
            {
                ESP_LOGI(TAG,"Suspend space");
                portDISABLE_INTERRUPTS();  
                vTaskSuspend(Spase_man_Task_Handler);
                portENABLE_INTERRUPTS();           
            }

            if(page_Menu.y.num != pagetaikon && page_Menu.y.num != pagewifi) //除了太空人 wifi 就退回
            {
                lv_obj_set_tile_id(guider_ui.tv, page_Menu.x.num, page_Menu.y.num, LV_ANIM_ON);
            }
            xQueueSend(Page_Event_Queue, &page_Menu.y.num, 0);
            break;
        case BT1DOUBLEEVENT:
            // printf("1double\n");
            if (page_Menu.x.num == 1 && page_Menu.y.num == pagewifi)
            {
                lv_group_send_data(guider_ui.group_wifi, LV_KEY_ENTER);
            }
            else if (page_Menu.x.num == 2 && page_Menu.y.num == pagewifi)
            {
                lv_event_send(lv_group_get_focused(guider_ui.group_key), LV_EVENT_VALUE_CHANGED, NULL);
            }
            break;
        case BT2DOUBLEEVENT:
            // printf("2double\n");
            if (page_Menu.x.num == 1 && page_Menu.y.num == pagewifi)
            {
                lv_group_send_data(guider_ui.group_wifi, LV_KEY_ENTER);
            }
            else if (page_Menu.x.num == 2 && page_Menu.y.num == pagewifi)
            {
                lv_event_send(lv_group_get_focused(guider_ui.group_key), LV_EVENT_VALUE_CHANGED, NULL);
            }
            break;
        }
    }
}

extern void* spaceman_img[14];
void Spase_man_thread_entry(void *p)
{
    while (1)
    {
        for (int i = 0; i < 14; i++)
        {
            if (guider_ui.astronaut == NULL)
            {
                guider_ui.astronaut = lv_img_create(guider_ui.screen_man);
                lv_img_set_src(guider_ui.astronaut, spaceman_img[i]); //然后显示此图片
                lv_obj_align(guider_ui.astronaut, LV_ALIGN_CENTER, 0, 0);
                vTaskDelay(pdMS_TO_TICKS(200));
            }
            portDISABLE_INTERRUPTS();    
            lv_obj_del(guider_ui.astronaut);
            guider_ui.astronaut = NULL;
            portENABLE_INTERRUPTS();
        }
    }
}
