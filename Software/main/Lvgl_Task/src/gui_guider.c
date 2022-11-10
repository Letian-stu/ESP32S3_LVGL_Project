/*
 * @Author: StuTian
 * @Date: 2022-09-05 14:07
 * @LastEditors: error: git config user.name & please set dead value or install git
 * @LastEditTime: 2022-11-10 15:10
 * @FilePath: \lvgl_menu_wifi\main\Lvgl_Task\src\gui_guider.c
 * @Description:
 * Copyright (c) 2022 by StuTian 1656733975@qq.com, All Rights Reserved.
 */
/*
 * Copyright 2022 NXP
 * SPDX-License-Identifier: MIT
 */

#include "lvgl/lvgl.h"
#include <stdio.h>
#include "app_task.h"
#include "gui_guider.h"
#include "string.h"
#include "app_sem_queue.h"

static const char *TAG = "UI";


extern Menu page_Menu;
extern lv_coord_t fft_buff[];
extern lv_coord_t Adc_buff[];

static lv_style_t style_bg;

static const char *btnm_map[] = {"1", "2", "3", "\n",
								 "4", "5", "6", "\n",
								 "7", "8", "9", "\n",
								 LV_SYMBOL_BACKSPACE, "0", LV_SYMBOL_NEW_LINE, ""};

char Select_WIFI[32];
char *WIFIMima;

static void btnm_event_handler(lv_event_t *e)
{
	lv_obj_t *obj = lv_event_get_target(e);
	lv_obj_t *ta = lv_event_get_user_data(e);
	const char *txt = lv_btnmatrix_get_btn_text(obj, lv_btnmatrix_get_selected_btn(obj));
	if (strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
	{
		lv_textarea_del_char(ta);
	}
	else if (strcmp(txt, LV_SYMBOL_NEW_LINE) == 0)
	{
		lv_event_send(ta, LV_EVENT_READY, NULL);
	}
	else
	{
		lv_textarea_add_text(ta, txt);
	}
}

static void wifimima_event_handler(lv_event_t *e)
{
	lv_obj_t *ta = lv_event_get_target(e);
	WIFIMima = (char *)(lv_textarea_get_text(ta));
	ESP_LOGI(TAG,"wifi mima = %s", WIFIMima);

	xEventGroupSetBits(Wifi_Event_Group, WIFISETSSID);
}

static void wifiname_event_handler(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t *obj = lv_event_get_target(e);
	if (code == LV_EVENT_VALUE_CHANGED)
	{
		lv_dropdown_get_selected_str(obj, Select_WIFI, sizeof(Select_WIFI));
		ESP_LOGI(TAG,"wifi name:%s",Select_WIFI);
		lv_label_set_text(guider_ui.wifiname, Select_WIFI);
		xEventGroupSetBits(Wifi_Event_Group, WIFISETNAME);
	}
}

void wifi_success_connect(lv_ui *ui)
{
    ui->label = lv_label_create(ui->wifi_sucess);
    lv_label_set_text(ui->label, "wifi connect ok");
    lv_obj_set_style_text_color(ui->label, lv_color_black(), 0);
    lv_obj_set_style_text_opa(ui->label, LV_OPA_COVER, 0);
    lv_obj_align_to(ui->label, ui->wifi_sucess, LV_ALIGN_CENTER, 0, 0);
	
	lv_anim_init(&ui->scr_in);	/* 初始化动�?*/
	lv_anim_set_exec_cb(&ui->scr_in, (lv_anim_exec_xcb_t) lv_obj_set_y);/* 设置“动画制作”功�? */	
	lv_anim_set_var(&ui->scr_in, ui->wifi_sucess);	
	lv_anim_set_time(&ui->scr_in, 4000);/* 动画时长[ms] */
	lv_anim_set_values(&ui->scr_in, -240, 18);/* 设置开始结束�? */
	lv_anim_set_path_cb(&ui->scr_in, lv_anim_path_ease_in_out  );/* 设置路径（曲线）。默认为线�? */
	lv_anim_set_playback_time(&ui->scr_in, 4000);
	lv_anim_start(&ui->scr_in);
}

void setup_scr_screen(lv_ui *ui)
{
	ui->bg = lv_obj_create(NULL);
	lv_obj_set_pos(ui->bg, 0, 0);
	lv_obj_set_size(ui->bg, 240, 240);

	ui->tv = lv_tileview_create(ui->bg);
	lv_obj_set_pos(ui->tv, 0, 0);
	lv_obj_set_size(ui->tv, 240, 240);
	lv_obj_set_style_clip_corner(ui->tv, 1, 0);				  
	lv_obj_set_scrollbar_mode(ui->tv, LV_SCROLLBAR_MODE_OFF); 

	/********************0.0********************/
	ui->tile = lv_tileview_add_tile(ui->tv, 0, 0, LV_DIR_BOTTOM | LV_DIR_RIGHT);
	ui->screen = lv_img_create(ui->tile);
	lv_obj_center(ui->screen);
	lv_img_set_src(ui->screen, &_xingqiu_120x120);

	/********************0.1********************/
	ui->tile = lv_tileview_add_tile(ui->tv, 1, 0, LV_DIR_LEFT);
	ui->screen_man = lv_img_create(ui->tile);
	lv_obj_set_pos(ui->screen_man, 0, 0);
	lv_obj_set_size(ui->screen_man, 240, 240);
	//lv_obj_set_style_radius(ui->screen_man,115,0);
	lv_style_init(&style_bg);
	lv_style_set_bg_opa(&style_bg, LV_OPA_COVER);
	lv_style_set_radius(&style_bg, 115);
	lv_style_set_bg_color(&style_bg, lv_color_make(0x00, 0x00, 0x00));
	lv_obj_add_style(ui->screen_man, &style_bg, LV_PART_MAIN | LV_STATE_DEFAULT);

	ui->astronaut = lv_img_create(ui->tile);
    lv_img_set_src(ui->astronaut, &astronaut1); 
    lv_obj_align(ui->astronaut, LV_ALIGN_CENTER, 0, 0);

	/********************1.0********************/
	ui->tile = lv_tileview_add_tile(ui->tv, 0, 1, LV_DIR_VER | LV_DIR_RIGHT);

	ui->wifi_sucess = lv_obj_create(ui->tile);
	lv_obj_remove_style_all(ui->wifi_sucess);
    lv_obj_set_size(ui->wifi_sucess, 160, 40); 
    lv_obj_set_style_radius(ui->wifi_sucess, 12, 0);
    lv_obj_set_style_bg_opa(ui->wifi_sucess, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(ui->wifi_sucess, lv_palette_main(LV_PALETTE_LIGHT_BLUE), 0);
	lv_obj_align_to(ui->wifi_sucess, ui->tile, LV_ALIGN_TOP_MID, 0, -40);

	ui->screen = lv_img_create(ui->tile);
	lv_obj_center(ui->screen);
	lv_img_set_src(ui->screen, &_wifi_120x120);


	/********************1.1********************/
	ui->tile = lv_tileview_add_tile(ui->tv, 1, 1, LV_DIR_HOR);


	ui->label = lv_label_create(ui->tile);
	lv_label_set_text(ui->label, "WIFI");
	lv_obj_align(ui->label, LV_ALIGN_TOP_MID, 0, 20);

	ui->table = lv_dropdown_create(ui->tile);
	lv_obj_set_size(ui->table, 160, 40);
	lv_obj_add_flag(ui->table, LV_OBJ_FLAG_CLICK_FOCUSABLE); /*To keep the text area focused on button clicks*/
	lv_dropdown_set_options(ui->table, "wait a moment\n");

	ui->label_ip = lv_label_create(ui->tile);
	lv_label_set_text(ui->label_ip, "IP:xxx.xx.xx");
	lv_obj_align(ui->label_ip, LV_ALIGN_CENTER, 0, 0);

	lv_obj_align(ui->table, LV_ALIGN_TOP_MID, 0, 50);
	lv_obj_add_event_cb(ui->table, wifiname_event_handler, LV_EVENT_ALL, NULL);
	ui->group_wifi = lv_group_create();
	lv_group_add_obj(ui->group_wifi, ui->table);

	/********************1.2********************/
	ui->tile = lv_tileview_add_tile(ui->tv, 2, 1, LV_DIR_LEFT);
	ui->wifiname = lv_label_create(ui->tile);
	lv_label_set_text(ui->wifiname, "name");
	lv_obj_align(ui->wifiname, LV_ALIGN_TOP_MID, 0, 20);

	ui->ta = lv_textarea_create(ui->tile);
	lv_textarea_set_one_line(ui->ta, 0); 
	lv_obj_align(ui->ta, LV_ALIGN_TOP_MID, 0, 40);
	lv_obj_set_size(ui->ta, 180, 40);
	lv_obj_add_event_cb(ui->ta, wifimima_event_handler, LV_EVENT_READY, ui->ta);
	lv_obj_add_state(ui->ta, LV_STATE_FOCUSED); /*To be sure the cursor is visible*/

	ui->btnm = lv_btnmatrix_create(ui->tile);
	lv_obj_set_size(ui->btnm, 180, 140);
	lv_obj_align(ui->btnm, LV_ALIGN_BOTTOM_MID, 0, -20);
	lv_obj_add_event_cb(ui->btnm, btnm_event_handler, LV_EVENT_VALUE_CHANGED, ui->ta);
	lv_obj_add_state(ui->btnm, LV_STATE_FOCUS_KEY);
	lv_obj_add_flag(ui->btnm, LV_OBJ_FLAG_CLICK_FOCUSABLE); /*To keep the text area focused on button clicks*/
	lv_btnmatrix_set_map(ui->btnm, btnm_map);

	ui->group_key = lv_group_create();
	lv_group_add_obj(ui->group_key, ui->btnm);

	/********************2.0********************/
	ui->tile = lv_tileview_add_tile(ui->tv, 0, 2, LV_DIR_VER | LV_DIR_RIGHT);
	ui->screen = lv_img_create(ui->tile);
	lv_obj_center(ui->screen);
	lv_img_set_src(ui->screen, &_tianqi_120x120);

	ui->tile = lv_tileview_add_tile(ui->tv, 1, 2, LV_DIR_LEFT);

	//Write style state: LV_STATE_DEFAULT for style_screen
	static lv_style_t style_screen;
	if (style_screen.prop_cnt > 1)
		lv_style_reset(&style_screen);
	else
		lv_style_init(&style_screen);
	lv_style_set_radius(&style_screen, 120);
	lv_style_set_bg_color(&style_screen, lv_color_make(0xed, 0xf4, 0x8a));
	lv_style_set_bg_grad_color(&style_screen, lv_color_make(0x3a, 0xac, 0xf2));
	lv_style_set_bg_grad_dir(&style_screen, LV_GRAD_DIR_VER);
	lv_style_set_bg_opa(&style_screen, 254);
	lv_style_set_border_color(&style_screen, lv_color_make(0xff, 0xff, 0xff));
	lv_style_set_border_width(&style_screen, 0);
	lv_style_set_border_opa(&style_screen, 255);
	lv_style_set_pad_left(&style_screen, 0);
	lv_style_set_pad_right(&style_screen, 0);
	lv_style_set_pad_top(&style_screen, 0);
	lv_style_set_pad_bottom(&style_screen, 0);
	lv_obj_add_style(ui->tile, &style_screen, LV_PART_MAIN|LV_STATE_DEFAULT);

	ui->weather = lv_img_create(ui->tile);
	lv_obj_align(ui->weather, LV_ALIGN_TOP_MID, 0, 10);
	lv_img_set_src(ui->weather, &_sun_100x100);

	ui->humidity = lv_img_create(ui->tile);
	lv_obj_set_pos(ui->humidity,20,80);
	lv_img_set_src(ui->humidity, &_humidity_50x50);

	ui->imgtemp = lv_img_create(ui->tile);
	lv_obj_set_pos(ui->imgtemp,170,80);
	lv_img_set_src(ui->imgtemp, &_temp_50x50);

	ui->city = lv_label_create(ui->tile);
	lv_obj_set_style_text_font(ui->city, &lv_font_medium_26, 0);
	lv_obj_align_to(ui->city,ui->weather,LV_ALIGN_OUT_BOTTOM_MID,0,5);
	lv_label_set_text(ui->city, "宁波");
	
	ui->wea = lv_label_create(ui->tile);
	lv_obj_set_style_text_font(ui->wea, &lv_font_medium_26, 0);
	lv_obj_align_to(ui->wea,ui->city,LV_ALIGN_OUT_BOTTOM_MID,0,5);
	lv_label_set_text(ui->wea, "多云");

	ui->week = lv_label_create(ui->tile);
	lv_obj_set_style_text_font(ui->week, &lv_font_medium_26, 0);
	lv_obj_align_to(ui->week,ui->wea,LV_ALIGN_OUT_BOTTOM_MID,-10,5);
	lv_label_set_text(ui->week, "星期一");

	ui->temp = lv_label_create(ui->tile);
	lv_obj_align_to(ui->temp,ui->imgtemp,LV_ALIGN_OUT_BOTTOM_MID,5,10);
	lv_label_set_text(ui->temp, "20°");

	ui->humiditylabel = lv_label_create(ui->tile);
	lv_obj_align_to(ui->humiditylabel,ui->humidity,LV_ALIGN_OUT_BOTTOM_MID,3,10);
	lv_label_set_text(ui->humiditylabel, "50%");

	ui->tile = lv_tileview_add_tile(ui->tv, 2, 2, LV_DIR_LEFT);

	ui->label = lv_label_create(ui->tile);
	lv_label_set_long_mode(ui->label, LV_LABEL_LONG_SCROLL_CIRCULAR);     /*Circular scroll*/
	lv_label_set_text(ui->label, "Weather for last seven days");
	lv_obj_set_width(ui->label, 120);
	lv_obj_align(ui->label, LV_ALIGN_TOP_MID, 0, 20);

	ui->weekday = lv_label_create(ui->tile);
	lv_label_set_text(ui->weekday, "1   2   3   4   5   6   7");
	lv_obj_align(ui->weekday, LV_ALIGN_BOTTOM_MID, 7, -25);

    ui->chart = lv_chart_create(ui->tile);
    lv_obj_set_size(ui->chart, 160, 140);
	lv_obj_align(ui->chart, LV_ALIGN_CENTER, 10, 0);
    //lv_obj_center(ui->chart);
    lv_chart_set_type(ui->chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/
	lv_chart_set_div_line_count(ui->chart, 5, 10);

	lv_chart_set_range(ui->chart, LV_CHART_AXIS_PRIMARY_Y, 0, 35);
	lv_chart_set_range(ui->chart, LV_CHART_AXIS_SECONDARY_Y, 0, 35);
	lv_chart_set_range(ui->chart, LV_CHART_AXIS_PRIMARY_X, 0, 7);
	lv_chart_set_range(ui->chart, LV_CHART_AXIS_SECONDARY_X, 0, 7);
	lv_chart_set_zoom_x(ui->chart, 370);

    lv_chart_set_axis_tick(ui->chart, LV_CHART_AXIS_PRIMARY_Y, 8, 4, 5, 5, true, 50);
    lv_chart_set_axis_tick(ui->chart, LV_CHART_AXIS_PRIMARY_X, 8, 4, 10, 1, false, 50);

	lv_obj_set_scrollbar_mode(ui->chart, LV_SCROLLBAR_MODE_OFF); //����scrollbar

	/*Add two data series*/
    ui->high = lv_chart_add_series(ui->chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_X);
    ui->low = lv_chart_add_series(ui->chart, lv_palette_main(LV_PALETTE_BLUE), LV_CHART_AXIS_SECONDARY_X);

    /*Directly set points on 'ser2'*/
	for (int i = 0; i < 7; i++)
	{
		ui->high->y_points[i] = 20 + i;
		ui->low->y_points[i]  = 10 + i;
	}
    lv_chart_refresh(ui->chart); /*Required after direct set*/

	/********************3.0********************/
	ui->tile = lv_tileview_add_tile(ui->tv, 0, 3, LV_DIR_VER | LV_DIR_RIGHT);
	ui->screen = lv_img_create(ui->tile);
	lv_obj_center(ui->screen);
	lv_img_set_src(ui->screen, &_music_120x120);

	ui->tile = lv_tileview_add_tile(ui->tv, 1, 3, LV_DIR_LEFT | LV_DIR_RIGHT);
	ui->label = lv_label_create(ui->tile);
	lv_label_set_text(ui->label, "adc data");
	lv_obj_align(ui->label, LV_ALIGN_TOP_MID, 0, 20);

    ui->adcchart = lv_chart_create(ui->tile);
    lv_obj_set_size(ui->adcchart, 200, 150);
    lv_obj_align(ui->adcchart, LV_ALIGN_CENTER, 0, 0);
	lv_chart_set_range(ui->adcchart, LV_CHART_AXIS_PRIMARY_Y, 0, 1500);
    lv_obj_set_style_size(ui->adcchart, 0, LV_PART_INDICATOR);
	lv_chart_set_div_line_count(ui->adcchart, 5, 10);
	lv_chart_set_point_count(ui->adcchart, 256);

    ui->ser1 = lv_chart_add_series(ui->adcchart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_ext_y_array(ui->adcchart, ui->ser1, (lv_coord_t *)Adc_buff);

	ui->tile = lv_tileview_add_tile(ui->tv, 2, 3, LV_DIR_LEFT);
	ui->label = lv_label_create(ui->tile);
	lv_label_set_text(ui->label, "fft data");
	lv_obj_align(ui->label, LV_ALIGN_TOP_MID, 0, 20);
	
    ui->fftchart = lv_chart_create(ui->tile);
    lv_obj_set_size(ui->fftchart, 200, 150);
    lv_obj_align(ui->fftchart, LV_ALIGN_CENTER, 0, 0);
    lv_chart_set_range(ui->fftchart, LV_CHART_AXIS_PRIMARY_Y, 0, 350);
	lv_obj_set_style_size(ui->fftchart, 0, LV_PART_INDICATOR);
	lv_chart_set_div_line_count(ui->fftchart, 5, 10);
	lv_chart_set_point_count(ui->fftchart, 128); // 64 
    /*Add two data series*/
    ui->ser2 = lv_chart_add_series(ui->fftchart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
	
	/********************4.0********************/
	ui->tile = lv_tileview_add_tile(ui->tv, 0, 4, LV_DIR_TOP | LV_DIR_RIGHT);
	ui->screen = lv_img_create(ui->tile);
	lv_obj_center(ui->screen);
	lv_img_set_src(ui->screen, &_set_120x120);

	ui->tile = lv_tileview_add_tile(ui->tv, 1, 4, LV_DIR_LEFT);
	ui->label = lv_label_create(ui->tile);
	lv_label_set_text(ui->label, "setting");
	lv_obj_align(ui->label, LV_ALIGN_CENTER, 0, 0);
}

void setup_ui(lv_ui *ui)
{
	setup_scr_screen(ui);
	lv_scr_load(ui->bg);
}
