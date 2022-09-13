/*
 * @Author: StuTian
 * @Date: 2022-09-05 14:07
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-11 15:19
 * @FilePath: \ESP32S3_LVGL_Project\main\Lvgl_Task\include\gui_guider.h
 * @Description: 
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved. 
 */

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H

#include "lvgl/lvgl.h"

typedef struct
{
	lv_obj_t *bg;

	lv_obj_t *screen;
	lv_obj_t *screen_img_in;
	lv_obj_t *screen_img_out;

	lv_anim_t scr_in;
	lv_anim_t scr_out;
}lv_ui;

extern lv_ui guider_ui;

void setup_ui(lv_ui *ui);
void setup_scr_screen(lv_ui *ui);
void crate_ui_animation(lv_ui *ui, uint8_t direction, lv_img_dsc_t *Input, lv_img_dsc_t *Output);
LV_IMG_DECLARE(_game_150x150);
LV_IMG_DECLARE(_konhz_150x150);
LV_IMG_DECLARE(_set_150x150);
LV_IMG_DECLARE(_cap_150x150);
LV_IMG_DECLARE(_bg_240x240);
#endif