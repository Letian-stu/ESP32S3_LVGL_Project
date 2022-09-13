/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:12
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-09 15:34
 * @FilePath: \ESP32S3_LVGL_Project\main\Os_Task\src\app_sem.c
 * @Description:
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved.
 */
#include <stdio.h>
#include "app_sem.h"

//static const char *TAG = "sem";

SemaphoreHandle_t GuiChangeSemaphore;

void SemaphoreInit(void)
{
	//taskENTER_CRITICAL(); //�����ٽ���
	GuiChangeSemaphore = xSemaphoreCreateBinary();
	//taskEXIT_CRITICAL();
}
