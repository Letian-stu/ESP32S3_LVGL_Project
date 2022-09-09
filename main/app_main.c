/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:12
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-09 15:33
 * @FilePath: \ESP32S3_LVGL_Project\main\app_main.c
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
#include "app_sem.h"

static const char *TAG = "main";

SemaphoreHandle_t xGuiSemaphore;
TaskHandle_t KEY_Scan_Task_Handler;
TaskHandle_t GUI_Change_Task_Handler;
QueueHandle_t Key_Queue;   		

void Menu_Switch(void)
{
	static uint8_t err = 1;
	if (Button_Value)
	{
		err=xQueueSend(Key_Queue,&Button_Value,0);
        if(err==errQUEUE_FULL)   	//·¢ËÍ°´¼üÖµ
        {
            ESP_LOGE(TAG, "Key_Queue send err");
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
		vTaskDelay(30 / portTICK_PERIOD_MS);
	}
}

void app_main(void)
{
    ESP_LOGI(TAG, "Hello World!");
	SemaphoreInit();
	xGuiSemaphore = xSemaphoreCreateMutex();
	Key_Queue = xQueueCreate(1, sizeof(uint8_t));
	xTaskCreatePinnedToCore(bootguiTask, 	"GUI_Start", 	4096, 	NULL, 1, NULL, 1);
	xTaskCreate(KEY_Scan_thread_entry, 		"KEY_Scan", 	4096, 	NULL, 2, &KEY_Scan_Task_Handler);
	xTaskCreate(GUI_Change_thread_entry, 	"GUI_Change", 	4096, 	NULL, 3, &GUI_Change_Task_Handler);
    
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
