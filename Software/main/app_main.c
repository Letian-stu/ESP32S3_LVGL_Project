/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:12
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-16 18:53
 * @FilePath: \Software\main\app_main.c
 * @Description:
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "lvgl_init.h"
#include "easyio.h"
#include "button.h"
#include "gui_guider.h"
#include "app_task.h"
#include "app_sem_queue.h"

static const char *TAG = "Main";

extern uint8_t *Image_p[];

TaskHandle_t Key_Scan_Task_Handler;
TaskHandle_t Menu_Select_Task_Handler;
TaskHandle_t Back_Select_Task_Handler;


void Menu_Switch(void)
{
	static uint8_t err = 1;
	if (Button_Value)
	{
		err = xQueueSend(Key_Num_Queue,&Button_Value,0);
        if( err == errQUEUE_FULL )  
        {
            ESP_LOGE(TAG, "Key_Num_Queue send err");
        }
		Button_Value = 0;
	}
}

void KEY_Scan_thread_entry(void *arg)
{
	Button_Init();
	while (1)
	{
		Button_Process();
		vTaskDelay(20 / portTICK_PERIOD_MS);
	}
}

void app_main(void)
{
	esp_log_level_set("Main", ESP_LOG_INFO);   
	esp_log_level_set("Key",  ESP_LOG_ERROR);  
	esp_log_level_set("Task", ESP_LOG_INFO);  

    ESP_LOGI(TAG, "Hello Tian!");
	Semaphore_Queue_Init();

	xTaskCreatePinnedToCore(bootguiTask, 	"GUI_Start", 	4096, 	NULL, 1, NULL, 1);
	while (!xSemaphoreTake(StartSysInitSemaphore, 0))
    {
		lv_task_handler();
		vTaskDelay(20 / portTICK_PERIOD_MS);
    }
	xTaskCreatePinnedToCore(appguiTask, 	"App_Gui", 		4096*3, NULL, 2, NULL, 1);	
	vTaskDelay(100 / portTICK_PERIOD_MS);
	xTaskCreate(KEY_Scan_thread_entry, 		"Key_Scan", 	4096, 	NULL, 2, &Key_Scan_Task_Handler);
	xTaskCreate(Menu_Select_thread_entry, 	"Menu_Select", 	4096, 	NULL, 3, &Menu_Select_Task_Handler);	
	xTaskCreate(Back_Select_thread_entry, 	"Back_Select", 	4096, 	NULL, 4, &Back_Select_Task_Handler);

	ESP_LOGI(TAG,"System Init OK!");
	crate_ui_animation(&guider_ui, 0, Image_p[1], Image_p[2]);
    while (1)
    {
		Menu_Switch();
		if (pdTRUE == xSemaphoreTake(xGuiSemaphore, portMAX_DELAY))
		{
			lv_task_handler();
		 	xSemaphoreGive(xGuiSemaphore);
		}
		vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}
