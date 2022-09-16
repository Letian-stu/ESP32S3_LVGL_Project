/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:12
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-16 17:10
 * @FilePath: \Software\main\Os_Task\src\app_sem.c
 * @Description:
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved.
 */
#include <stdio.h>
#include "app_sem.h"

//static const char *TAG = "sem";
SemaphoreHandle_t StartSysInitSemaphore;
SemaphoreHandle_t xGuiSemaphore;

void SemaphoreInit(void)
{
	//taskENTER_CRITICAL(); //进入临界区
	StartSysInitSemaphore 	= xSemaphoreCreateBinary();
	xGuiSemaphore 			= xSemaphoreCreateMutex();
	//taskEXIT_CRITICAL();
}
