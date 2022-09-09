/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:12
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-09 13:58
 * @FilePath: \ESP32_LVGL\main\Os_Task\src\app_task.c
 * @Description:
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#include "lvgl_init.h"
#include "easyio.h"
#include "button.h"
#include "gui_guider.h"
#include "app_sem.h"
#include "button.h"

//static const char *TAG = "task";

extern QueueHandle_t Key_Queue;  

void GUI_Change_thread_entry(void *pvParameters)
{	
	static uint8_t Select_Value = 0;
	while (1)
	{
		Select_Value = 0;
		if( xQueueReceive(Key_Queue, &Select_Value, portMAX_DELAY) )
		{	
			if (Select_Value == BT1_DOWN)
			{
				lv_img_set_src(guider_ui.screen_img_in, &_game_150x150);
				lv_img_set_src(guider_ui.screen_img_out, &_konhz_150x150);
			}
			else if (Select_Value == BT1_DOUBLE)		
			{
				lv_img_set_src(guider_ui.screen_img_in, &_set_150x150);
				lv_img_set_src(guider_ui.screen_img_out, &_game_150x150);
	
			}
			// else if (Select_Value == BT1_LONG)	
			// {

			// }
			
			if (Select_Value == BT2_DOWN)
			{
				lv_img_set_src(guider_ui.screen_img_in, &_cap_150x150);
				lv_img_set_src(guider_ui.screen_img_out, &_set_150x150);				
			}
			else if (Select_Value == BT2_DOUBLE)
			{
				lv_img_set_src(guider_ui.screen_img_in, &_konhz_150x150);
				lv_img_set_src(guider_ui.screen_img_out, &_cap_150x150);			
			}		
			// else if (Select_Value == BT2_LONG)
			// {

			// }
			crate_ui_animation(&guider_ui);
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
