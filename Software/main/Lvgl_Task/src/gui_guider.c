/*
 * @Author: StuTian
 * @Date: 2022-09-05 14:07
 * @LastEditors: StuTian
 * @LastEditTime: 2022-09-11 14:08
 * @FilePath: \ESP32S3_LVGL_Project\main\Lvgl_Task\src\gui_guider.c
 * @Description: 
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved. 
 */
/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl/lvgl.h"
#include <stdio.h>
#include "gui_guider.h"


void crate_ui_animation(lv_ui *ui, uint8_t direction, lv_img_dsc_t *Input, lv_img_dsc_t *Output)
{
	lv_anim_init(&ui->scr_in);	/* 初始化动画*/
	lv_anim_set_exec_cb(&ui->scr_in, (lv_anim_exec_xcb_t) lv_obj_set_y);/* 设置“动画制作”功能 */	
	lv_anim_set_var(&ui->scr_in, ui->screen_img_in);	
	lv_anim_set_time(&ui->scr_in, 1000);/* 动画时长[ms] */
	if(direction)
	{
		lv_anim_set_values(&ui->scr_in, -240, 50);/* 设置开始结束值 */
	}
	else
	{
		lv_anim_set_values(&ui->scr_in, 240, 50);/* 设置开始结束值 */
	}
	lv_img_set_src(ui->screen_img_in, Input);
	lv_anim_set_path_cb(&ui->scr_in, lv_anim_path_overshoot  );/* 设置路径（曲线）。默认为线性 */    
	lv_anim_start(&ui->scr_in); 		
	
	lv_anim_init(&ui->scr_out);
	lv_anim_set_exec_cb(&ui->scr_out, (lv_anim_exec_xcb_t) lv_obj_set_y);
	lv_anim_set_var(&ui->scr_out, ui->screen_img_out);
	lv_anim_set_time(&ui->scr_out, 1000);
	if(direction)
	{
		lv_anim_set_values(&ui->scr_out, 50, 240);
	}
	else
	{
		lv_anim_set_values(&ui->scr_out, 50, -240);
	}
	lv_img_set_src(ui->screen_img_out, Output);
	lv_anim_set_path_cb(&ui->scr_out, lv_anim_path_overshoot  );
	lv_anim_start(&ui->scr_out); 
}

void setup_scr_screen(lv_ui *ui){

	//Write codes screen
	ui->screen = lv_obj_create(NULL);

	//Write style state: LV_STATE_DEFAULT for style_screen_main_main_default
	static lv_style_t style_screen_main_main_default;
	lv_style_reset(&style_screen_main_main_default);
	lv_style_set_bg_color(&style_screen_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_bg_opa(&style_screen_main_main_default, 0);
	lv_obj_add_style(ui->screen, &style_screen_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_img_1
	ui->screen_img_in = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_in, 50, 50);
	lv_obj_set_size(ui->screen_img_in, 150, 150);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_in_main_main_default
	static lv_style_t style_screen_img_in_main_main_default;
	lv_style_reset(&style_screen_img_in_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_in_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_in_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_in_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_in, &style_screen_img_in_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_in, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_in,&_cap_150x150);
	lv_img_set_pivot(ui->screen_img_in, 0,0);
	lv_img_set_angle(ui->screen_img_in, 0);

	//Write codes screen_img_1
	ui->screen_img_out = lv_img_create(ui->screen);
	lv_obj_set_pos(ui->screen_img_out, 50, 50);
	lv_obj_set_size(ui->screen_img_out, 150, 150);

	//Write style state: LV_STATE_DEFAULT for style_screen_img_out_main_main_default
	static lv_style_t style_screen_img_out_main_main_default;
	lv_style_reset(&style_screen_img_out_main_main_default);
	lv_style_set_img_recolor(&style_screen_img_out_main_main_default, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_img_recolor_opa(&style_screen_img_out_main_main_default, 0);
	lv_style_set_img_opa(&style_screen_img_out_main_main_default, 255);
	lv_obj_add_style(ui->screen_img_out, &style_screen_img_out_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_add_flag(ui->screen_img_out, LV_OBJ_FLAG_CLICKABLE);
	lv_img_set_src(ui->screen_img_out,&_cap_150x150);
	lv_img_set_pivot(ui->screen_img_out, 0,0);
	lv_img_set_angle(ui->screen_img_out, 0);                 
}


void setup_ui(lv_ui *ui){
	setup_scr_screen(ui);
	lv_scr_load(ui->screen);		
}

