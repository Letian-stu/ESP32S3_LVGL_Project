/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:14
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-06 18:41
 * @FilePath: \ESP32_LVGL\main\app_sem.h
 * @Description: 
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved. 
 */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"

extern SemaphoreHandle_t GuiChangeSemaphore;

void SemaphoreInit(void);
