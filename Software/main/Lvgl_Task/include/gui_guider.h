/*
 * @Author: StuTian
 * @Date: 2022-09-05 14:07
 * @LastEditors: error: git config user.name & please set dead value or install git
 * @LastEditTime: 2022-11-10 10:56
 * @FilePath: \lvgl_menu_wifi\main\Lvgl_Task\include\gui_guider.h
 * @Description:
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved.
 */

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H

#include "lvgl/lvgl.h"

typedef struct
{
	//base
	lv_obj_t *bg;
	lv_obj_t *tv;
	lv_obj_t *tile;
	lv_obj_t *screen;
	lv_obj_t *label;
	//spase man
	lv_obj_t *screen_man;
	lv_obj_t *astronaut;
	//wifi
	lv_obj_t *wifi_sucess;
	lv_anim_t scr_in;
	lv_obj_t *label_ip;
	lv_group_t *group_wifi;
	lv_obj_t *table;
	lv_obj_t *wifiname;
	lv_obj_t *ta;
	lv_obj_t *btnm;
	lv_group_t *group_key;
	//weather
	lv_obj_t *weather;
	lv_obj_t *humidity;
	lv_obj_t *imgtemp;
	lv_obj_t *city;
	lv_obj_t *week;
	lv_obj_t *wea;
	lv_obj_t *temp;
	lv_obj_t *humiditylabel;
	lv_obj_t *weekday;	
    lv_obj_t *chart;
	lv_chart_series_t *high;
	lv_chart_series_t *low;
	//fft
	lv_obj_t *adcchart;
	lv_chart_series_t *ser1;
	lv_obj_t *fftchart;
	lv_chart_series_t *ser2;
} lv_ui;

extern char Select_WIFI[32];
extern char *WIFIMima;

extern lv_ui guider_ui;
void setup_ui(lv_ui *ui);
void wifi_success_connect(lv_ui *ui);
void setup_scr_screen(lv_ui *ui);


LV_FONT_DECLARE(lv_font_medium_26);

LV_IMG_DECLARE(_rain_100x100);
LV_IMG_DECLARE(_snow_100x100);
LV_IMG_DECLARE(_windy_100x100);
LV_IMG_DECLARE(_storm_100x100);
LV_IMG_DECLARE(_sun_100x100);
LV_IMG_DECLARE(_humidity_50x50);
LV_IMG_DECLARE(_temp_50x50);


LV_IMG_DECLARE(_xingqiu_120x120);
LV_IMG_DECLARE(_wifi_120x120);
LV_IMG_DECLARE(_tianqi_120x120);
LV_IMG_DECLARE(_music_120x120);
LV_IMG_DECLARE(_set_120x120);

LV_IMG_DECLARE(astronaut1);
LV_IMG_DECLARE(astronaut2);
LV_IMG_DECLARE(astronaut3);
LV_IMG_DECLARE(astronaut4);
LV_IMG_DECLARE(astronaut5);
LV_IMG_DECLARE(astronaut6);
LV_IMG_DECLARE(astronaut7);
LV_IMG_DECLARE(astronaut8);
LV_IMG_DECLARE(astronaut9);
LV_IMG_DECLARE(astronaut10);
LV_IMG_DECLARE(astronaut11);
LV_IMG_DECLARE(astronaut12);
LV_IMG_DECLARE(astronaut13);
LV_IMG_DECLARE(astronaut14);

#endif