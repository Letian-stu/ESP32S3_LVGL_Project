/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:12
 * @LastEditors: error: git config user.name & please set dead value or install git
 * @LastEditTime: 2022-11-10 16:08
 * @FilePath: \lvgl_menu_wifi\main\app_main.c
 * @Description:
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "lvgl_init.h"
#include "easyio.h"
#include "button.h"
#include "gui_guider.h"
#include "app_task.h"
#include "app_sem_queue.h"
#include "driver/adc.h"
#include "esp_dsp.h"

static const char *TAG = "Main";

enum
{
	page_spaceman = 0,
	page_wifi,
	page_weather,
	page_game,
	page_set,
};

extern Menu page_Menu;
extern uint8_t scan_wifi;
extern struct lock Task_lock;

TaskHandle_t fft_Task_Handler;
TaskHandle_t Key_Scan_Task_Handler;
TaskHandle_t Key_Select_Task_Handler;
TaskHandle_t New_page_Task_Handler;
TaskHandle_t wifi_Task_Handler;
TaskHandle_t Spase_man_Task_Handler;
TaskHandle_t Http_Task_Handler;
TaskHandle_t readadc_Task_Handler;

void KEY_Scan_thread_entry(void *pvParameters)
{
	Button_Init();
	while (1)
	{
		Button_Process();
		vTaskDelay(30 / portTICK_PERIOD_MS);
	}
}

void app_main(void)
{  
    ESP_LOGI(TAG, "Hello Tian!");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
	portDISABLE_INTERRUPTS(); 
	Semaphore_Queue_Init();
	xTaskCreatePinnedToCore(appguiTask, 	"App_Gui", 		4096*6, NULL, 4, NULL, 1);
	xTaskCreate(KEY_Scan_thread_entry, 		"Key_Scan", 	4096, 	NULL, 5, &Key_Scan_Task_Handler);
	xTaskCreate(Key_Select_thread_entry, 	"Short_Select", 4096, 	NULL, 5, &Key_Select_Task_Handler);		
	xTaskCreate(New_page_thread_entry, 		"New_page", 	4096*3, NULL, 3, &New_page_Task_Handler);	
    xTaskCreate(wifi_thread_entry, 	        "wifi",	        4096*4, NULL, 4, &wifi_Task_Handler);	
    
	portENABLE_INTERRUPTS(); 
	ESP_LOGI(TAG,"System Init OK!");
    
	vTaskDelete(NULL);
}

void New_page_thread_entry(void *p)
{
    static uint8_t Select_Value;
    while (1)
    {
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (xQueueReceive(Page_Event_Queue, &Select_Value, portMAX_DELAY))
        {
            printf("recv event = %d \n",Select_Value);
            switch (Select_Value)
            {
            case page_spaceman:
                
				if (Task_lock.page_spaceman_lock == 2)
				{
                    ESP_LOGI(TAG, "Resume Spase_man");
					Task_lock.page_spaceman_lock = 1;
					portDISABLE_INTERRUPTS();                    
					vTaskResume(Spase_man_Task_Handler);
					portENABLE_INTERRUPTS();
				}
                else if(Task_lock.page_spaceman_lock == 1)
                {
					Task_lock.page_spaceman_lock = 2;
					lv_obj_set_tile_id(guider_ui.tv, page_Menu.x.num, page_Menu.y.num, LV_ANIM_ON);
					break;
				}
				else
                {
					Task_lock.page_spaceman_lock = 1;
					portDISABLE_INTERRUPTS();                    
					xTaskCreate(Spase_man_thread_entry, "Spase_man", 4096*4, NULL, 3, &Spase_man_Task_Handler);
					portENABLE_INTERRUPTS();
                    ESP_LOGI(TAG, "Create Spase_man");
					break;
				}
                break;
            case page_wifi:printf("page_wifi \n");
                if(Task_lock.page_wifi == 0) 
                {
                    Task_lock.page_wifi = 1;
                    portDISABLE_INTERRUPTS();    
                    // xTaskCreate(wifi_thread_entry, 	"wifi",	4096*4, 	NULL, 3, &wifi_Task_Handler);	
                    vTaskResume(wifi_Task_Handler);
                    portENABLE_INTERRUPTS();
                    ESP_LOGI(TAG, "Create wifi");
                    break;
                }
                else if(Task_lock.page_wifi == 1)
                {
                    ESP_LOGI(TAG, "Resume wifi");
                    Task_lock.page_wifi = 2;
                    scan_wifi = 1;
                    lv_dropdown_set_options(guider_ui.table, "wait a moment\n");
                    portDISABLE_INTERRUPTS();
                    vTaskResume(wifi_Task_Handler);
					portENABLE_INTERRUPTS();
                    lv_obj_set_tile_id(guider_ui.tv, page_Menu.x.num, page_Menu.y.num, LV_ANIM_ON); 
                    break;
                }
                else
                {
                    ESP_LOGI(TAG, "Suspend wifi");
                    Task_lock.page_wifi = 1;
                    portDISABLE_INTERRUPTS();
                    esp_wifi_stop();
                    vTaskSuspend(wifi_Task_Handler);
					portENABLE_INTERRUPTS();
                    lv_obj_set_tile_id(guider_ui.tv, page_Menu.x.num, page_Menu.y.num, LV_ANIM_ON); 
                    break;                    
                }
                
                break;
            case page_weather:printf("page_weather \n");
                break;
            case page_game:printf("page_game \n");
                if(Task_lock.page_game == 0)
                {
                    Task_lock.page_game = 1;
                    portDISABLE_INTERRUPTS();    
                    xTaskCreate(readadc_thread_entry,       "adc",          4096*5, NULL, 4, &readadc_Task_Handler);
                    portENABLE_INTERRUPTS();
                    ESP_LOGI(TAG, "Create adc");
                    break;
                }
                if(Task_lock.page_game == 1 && page_Menu.x.num == 0)
                {
                    ESP_LOGI(TAG,"Suspend adc"); //ø®À¿
                    Task_lock.page_game = 2;
                    vTaskSuspend(readadc_Task_Handler);
                    break;               
                }
                if(Task_lock.page_game == 2)
                {
                    ESP_LOGI(TAG, "Resume adc");
                    Task_lock.page_game = 1;
                    portDISABLE_INTERRUPTS();    
                    vTaskResume(readadc_Task_Handler);
                    portENABLE_INTERRUPTS();
                    break;
                }
                break;
            case page_set:printf("page_set \n");
                break;
            }
        }
    }
}
