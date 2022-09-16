/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:12
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-16 18:46
 * @FilePath: \Software\main\Os_Task\src\app_sem_queue.c
 * @Description:
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved.
 */
#include <stdio.h>
#include "app_sem_queue.h"

//static const char *TAG = "sem";
SemaphoreHandle_t 	StartSysInitSemaphore;
SemaphoreHandle_t 	xGuiSemaphore;
QueueHandle_t 		Key_Num_Queue;
QueueHandle_t		Menu2_Num_Queue;

void Semaphore_Queue_Init(void)
{
	StartSysInitSemaphore 	= xSemaphoreCreateBinary();
	xGuiSemaphore 			= xSemaphoreCreateMutex();
	Key_Num_Queue 			= xQueueCreate(1, sizeof(uint8_t));
	Menu2_Num_Queue 		= xQueueCreate(1, sizeof(uint8_t));
}
