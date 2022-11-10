/*
 * @Author: StuTian
 * @Date: 2022-09-03 22:14
 * @LastEditors: error: git config user.name & please set dead value or install git
 * @LastEditTime: 2022-10-13 12:00
 * @FilePath: \lvgl_menu_wifi\main\Os_Task\include\app_sem_queue.h
 * @Description: 
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved. 
 */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define BT1SHORTEVENT   (1<<0)
#define BT2SHORTEVENT   (1<<1)
#define BT1LONGEVENT    (1<<2)
#define BT2LONGEVENT    (1<<3)
#define BT1DOUBLEEVENT  (1<<4)
#define BT2DOUBLEEVENT  (1<<5)
#define BTALLEVENT        (BT1SHORTEVENT | BT2SHORTEVENT | BT1LONGEVENT | BT2LONGEVENT | BT1DOUBLEEVENT | BT2DOUBLEEVENT)


#define WIFISETNAME      (1<<2)
#define WIFISETSSID      (1<<3)
#define WIFIOK           (1<<4)
#define WIFIALLAVENT ( WIFISETNAME | WIFISETSSID | WIFIOK)

extern SemaphoreHandle_t    xGuiSemaphore;
extern QueueHandle_t 		Task_Num_Queue;
extern QueueHandle_t 		Page_Event_Queue;
extern EventGroupHandle_t 	Key_Event_Group;
extern EventGroupHandle_t   Wifi_Event_Group;


void Semaphore_Queue_Init(void);
