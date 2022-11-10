/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:12
 * @LastEditors: error: git config user.name & please set dead value or install git
 * @LastEditTime: 2022-11-08 14:56
 * @FilePath: \lvgl_menu_wifi\main\Os_Task\src\app_sem_queue.c
 * @Description:
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved.
 */
#include <stdio.h>
#include "app_sem_queue.h"


//static const char *TAG = "sem";

SemaphoreHandle_t 	xGuiSemaphore;
SemaphoreHandle_t 	ReadOkSemaphore;
QueueHandle_t 		Task_Num_Queue;
QueueHandle_t 		Page_Event_Queue;
EventGroupHandle_t 	Key_Event_Group;
EventGroupHandle_t 	Wifi_Event_Group;

void Semaphore_Queue_Init(void)
{
	xGuiSemaphore 			= xSemaphoreCreateMutex();
	Task_Num_Queue			= xQueueCreate(1, sizeof(uint8_t));
	Page_Event_Queue		= xQueueCreate(1, sizeof(uint8_t));
	Key_Event_Group 		= xEventGroupCreate();
	Wifi_Event_Group 		= xEventGroupCreate();
	ReadOkSemaphore 		= xSemaphoreCreateBinary();
}
