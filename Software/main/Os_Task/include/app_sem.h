/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:14
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-16 17:10
 * @FilePath: \Software\main\Os_Task\include\app_sem.h
 * @Description: 
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved. 
 */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"


extern SemaphoreHandle_t StartSysInitSemaphore;
extern SemaphoreHandle_t xGuiSemaphore;
void SemaphoreInit(void);
