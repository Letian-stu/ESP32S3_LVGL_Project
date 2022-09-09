/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:14
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-05 15:07
 * @FilePath: \ESP32_LVGL\main\lvgl_init.h
 * @Description: 
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved. 
 */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "lvgl.h"
#include "freertos/semphr.h"
#include "esp_system.h"

/* Littlevgl specific */
#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

#include "gui_guider.h"

#include "lvgl_helpers.h"
#include "lvgl/src/hal/lv_hal_disp.h"
#include "esp_log.h"


#define LV_TICK_PERIOD_MS 1

lv_obj_t *background;
lv_obj_t *label_1;
lv_obj_t *bar;


void APP_UI_Init(void);
void Boot_UI_Init(void);

void lv_tick_task(void *arg);
void bootguiTask(void *pvParameter);
void appguiTask(void *pvParameter);

