/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:12
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-16 19:20
 * @FilePath: \Software\main\Os_Task\src\app_task.c
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
#include "app_sem_queue.h"
#include "button.h"

static const char *TAG = "Task";

enum
{
	Lavel_NONE,
	Lavel_First,
	Lavel_Secend,
};
uint8_t The_Menu_Lavel = 1;

uint8_t *Image_p[] = {NULL, &_cap_150x150, &_game_150x150, &_konhz_150x150, &_set_150x150};

void Menu_Select_thread_entry(void *pvParameters)
{
	static uint8_t 	Select_Value = 0;

	static uint8_t	GUI_Menu_Num = 1;
	static uint8_t 	Last_GUI_Menu_Num = 1;
	static uint8_t 	Change_Direction = 0;

	static uint8_t 	Level_Num = 1;
	static uint8_t 	Select_Back_Num = 0;
#define LastMenuNum 5
	while (1)
	{
		Select_Value = 0;
		if (xQueueReceive(Key_Num_Queue, &Select_Value, portMAX_DELAY))
		{
			if(The_Menu_Lavel == Lavel_First)
			{
				switch(Select_Value)
				{
					case BT1_DOWN:		GUI_Menu_Num++;
										if (GUI_Menu_Num == LastMenuNum)
										{
											GUI_Menu_Num = 1;
										}
						break;
					case BT2_DOWN:		GUI_Menu_Num--;
										if (GUI_Menu_Num == 0)
										{
											GUI_Menu_Num = LastMenuNum - 1;
										}
						break;
					case BT1_DOUBLE:	/* next */
						break;
					case BT2_DOUBLE:	/* last */
						break;	
					case BT1_LONG:		/* select */
						Level_Num++;
						if(Level_Num > 2)
							Level_Num = 2;
						break;
					case BT2_LONG:		/* back */
						break;
					default:ESP_LOGE(TAG, "Key Num Error");
						break;
				}

				ESP_LOGI(TAG, "Menu_Num = %d Last_Menu_Num = %d Level_Num = %d", GUI_Menu_Num, Last_GUI_Menu_Num, Level_Num);

				if (GUI_Menu_Num == Last_GUI_Menu_Num)	
				{
					xQueueSend(Menu2_Num_Queue,&Level_Num,0);
				}
				else if (Last_GUI_Menu_Num == LastMenuNum - 1 && GUI_Menu_Num == 1)
				{
					Change_Direction = 1;//xia
				}
				else if (Last_GUI_Menu_Num == 1 && GUI_Menu_Num == LastMenuNum - 1)
				{
					Change_Direction = 0;//shang
				}
				else if (GUI_Menu_Num > Last_GUI_Menu_Num)
				{
					Change_Direction = 1;
				}
				else if (GUI_Menu_Num < Last_GUI_Menu_Num)
				{
					Change_Direction = 0;
				}
				ESP_LOGD(TAG, "Dir = %d", Change_Direction);	
				if(Last_GUI_Menu_Num != GUI_Menu_Num)
				{
					crate_ui_animation(&guider_ui, Change_Direction, Image_p[GUI_Menu_Num], Image_p[Last_GUI_Menu_Num]);
					Last_GUI_Menu_Num = GUI_Menu_Num;				
				}				
			}
			else if(The_Menu_Lavel == Lavel_Secend)
			{
				switch(Select_Value)
				{
					case BT1_DOWN:		
						break;
					case BT2_DOWN:		
						break;
					case BT1_DOUBLE:	/* next */
						break;
					case BT2_DOUBLE:	/* last */
						break;	
					case BT1_LONG:		/* select */
						break;
					case BT2_LONG:		/* back */
						Level_Num--;
						if(Level_Num < 1)
							Level_Num = 1;
						break;
					default:ESP_LOGE(TAG, "Key Num Error");
						break;
					
				}
				ESP_LOGD(TAG, "Level_Num = %d", Level_Num);
				xQueueSend(Menu2_Num_Queue,&Level_Num,0);
			}


		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void Back_Select_thread_entry(void *pvParameters)
{
	static uint8_t Level_Num= 0;
	while(1)
	{
		if (xQueueReceive(Menu2_Num_Queue, &Level_Num, portMAX_DELAY))
		{
			ESP_LOGI(TAG, "Recv Level_Num = %d", Level_Num);
			if(Level_Num == Lavel_First)
			{
				The_Menu_Lavel = Lavel_First;	
			}
			else if(Level_Num == Lavel_Secend)
			{
				The_Menu_Lavel = Lavel_Secend;
			}
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
