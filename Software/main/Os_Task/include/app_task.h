/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:14
 * @LastEditors: error: git config user.name & please set dead value or install git
 * @LastEditTime: 2022-11-08 15:11
 * @FilePath: \lvgl_menu_wifi\main\Os_Task\include\app_task.h
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


struct lock
{
    uint8_t page_spaceman_lock;
    uint8_t page_wifi;
    uint8_t page_weather;
    uint8_t page_game;
    uint8_t page_set;
};

struct numobj{
    uint8_t min;
    uint8_t max;
    uint8_t num;   
};

typedef struct{
    struct numobj x;
    struct numobj y;
} Menu;

enum page
{
    pagetaikon = 0,
    pagewifi,
    pageweather,
    pagegame,
    pageset,
};

extern Menu page_Menu;
extern struct lock Task_lock;

void Key_Select_thread_entry(void *p);
void New_page_thread_entry(void *p);
void wifi_thread_entry(void *p);
void Spase_man_thread_entry(void *p);
void readadc_thread_entry(void *p);
