/*
 * @Author: error: git config user.name & please set dead value or install git
 * @Date: 2022-10-17 09:55
 * @LastEditors: error: git config user.name & please set dead value or install git
 * @LastEditTime: 2022-11-05 21:04
 * @FilePath: \lvgl_menu_wifi\main\Page_Task\src\page_http.c
 * @Description: 
 * Copyright (c) 2022 by error: git config user.name && git config user.email & please set dead value or install git, All Rights Reserved. 
 */
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "cJSON.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "gui_guider.h"

static const char *TAG = "HTTP";

#define MAX_HTTP_OUTPUT_BUFFER 4096

extern lv_ui guider_ui;

void http_test_task(void *p)
{
    char tempandhumidity[32];
    char output_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0}; 
    int content_length = 0;                           
    esp_http_client_config_t config;
    memset(&config, 0, sizeof(config));
    //https://devapi.qweather.com/v7/weather/7d?location=101210401&key=9b3515bf79414eafb10d4ff6ff5e447b
    //https://yiketianqi.com/free/day?appid=51394988&appsecret=uMRmP2At&cityid=101210401&unescape=1
    static const char *URL = "https://www.yiketianqi.com/free/week?unescape=1&appid=51394988&appsecret=uMRmP2At";
    config.url = URL;
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);
    while (1)
    {
        esp_err_t err = esp_http_client_open(client, 0);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        }
        else
        {
            ESP_LOGI(TAG, "Start recv a week ");
            content_length = esp_http_client_fetch_headers(client);
            if (content_length < 0)
            {
                ESP_LOGE(TAG, "HTTP client fetch headers failed");
            }
            else
            {
                int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
                if (data_read >= 0)
                {
                    
                    ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                             esp_http_client_get_status_code(client),     
                             esp_http_client_get_content_length(client)); 
                    ESP_LOGI(TAG, "data:%s", output_buffer);
                    cJSON *root = NULL;
                    root = cJSON_Parse(output_buffer);
                    cJSON *data = cJSON_GetObjectItem(root, "data");
                    cJSON *day0 = cJSON_GetArrayItem(data, 0);
                    cJSON *day0_high = cJSON_GetObjectItem(day0, "tem_day");
                    cJSON *day0_low = cJSON_GetObjectItem(day0, "tem_night");       
                    ESP_LOGI(TAG, "day0 high:%s low:%s",day0_high->valuestring, day0_low->valuestring );

                    cJSON *day1 = cJSON_GetArrayItem(data, 1);
                    cJSON *day1_high = cJSON_GetObjectItem(day1, "tem_day");
                    cJSON *day1_low = cJSON_GetObjectItem(day1, "tem_night");       
                    ESP_LOGI(TAG, "day1 high:%s low:%s",day1_high->valuestring, day1_low->valuestring );

                    cJSON *day2 = cJSON_GetArrayItem(data, 2);
                    cJSON *day2_high = cJSON_GetObjectItem(day2, "tem_day");
                    cJSON *day2_low = cJSON_GetObjectItem(day2, "tem_night");       
                    ESP_LOGI(TAG, "today high:%s low:%s",day2_high->valuestring, day2_low->valuestring );

                    cJSON *day3 = cJSON_GetArrayItem(data, 3);
                    cJSON *day3_high = cJSON_GetObjectItem(day3, "tem_day");
                    cJSON *day3_low = cJSON_GetObjectItem(day3, "tem_night");       
                    ESP_LOGI(TAG, "today high:%s low:%s",day3_high->valuestring, day3_low->valuestring );

                    cJSON *day4 = cJSON_GetArrayItem(data, 4);
                    cJSON *day4_high = cJSON_GetObjectItem(day4, "tem_day");
                    cJSON *day4_low = cJSON_GetObjectItem(day4, "tem_night");       
                    ESP_LOGI(TAG, "today high:%s low:%s",day4_high->valuestring, day4_low->valuestring );

                    cJSON *day5 = cJSON_GetArrayItem(data, 5);
                    cJSON *day5_high = cJSON_GetObjectItem(day5, "tem_day");
                    cJSON *day5_low = cJSON_GetObjectItem(day5, "tem_night");       
                    ESP_LOGI(TAG, "today high:%s low:%s",day5_high->valuestring, day5_low->valuestring );

                    cJSON *day6 = cJSON_GetArrayItem(data, 6);
                    cJSON *day6_high = cJSON_GetObjectItem(day6, "tem_day");
                    cJSON *day6_low = cJSON_GetObjectItem(day6, "tem_night");       
                    ESP_LOGI(TAG, "today high:%s low:%s",day6_high->valuestring, day6_low->valuestring );

                    /*Directly set points on 'ser2'*/
                    guider_ui.high->y_points[0] = atoi(day0_high->valuestring);
                    guider_ui.high->y_points[1] = atoi(day1_high->valuestring);
                    guider_ui.high->y_points[2] = atoi(day2_high->valuestring);
                    guider_ui.high->y_points[3] = atoi(day3_high->valuestring);
                    guider_ui.high->y_points[4] = atoi(day4_high->valuestring);
                    guider_ui.high->y_points[5] = atoi(day5_high->valuestring);
                    guider_ui.high->y_points[6] = atoi(day6_high->valuestring);

                    /*Directly set points on 'ser2'*/
                    guider_ui.low->y_points[0] = atoi(day0_low->valuestring);
                    guider_ui.low->y_points[1] = atoi(day1_low->valuestring);
                    guider_ui.low->y_points[2] = atoi(day2_low->valuestring);
                    guider_ui.low->y_points[3] = atoi(day3_low->valuestring);
                    guider_ui.low->y_points[4] = atoi(day4_low->valuestring);
                    guider_ui.low->y_points[5] = atoi(day5_low->valuestring);
                    guider_ui.low->y_points[6] = atoi(day6_low->valuestring);

                    lv_chart_refresh(guider_ui.chart); /*Required after direct set*/

                    cJSON_Delete(root);
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to read response");
                }
            }
        esp_http_client_close(client);
        }        
        config.url = "https://yiketianqi.com/free/day?appid=51394988&appsecret=uMRmP2At&cityid=101210401&unescape=1";
        esp_http_client_handle_t client = esp_http_client_init(&config); 
        ESP_LOGI(TAG, "Start recv one day ");
        err = esp_http_client_open(client, 0);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
        }
        else
        {
            content_length = esp_http_client_fetch_headers(client);
            if (content_length < 0)
            {
                ESP_LOGE(TAG, "HTTP client fetch headers failed");
            }
            else
            {

                int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
                if (data_read >= 0)
                {
                    ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                            esp_http_client_get_status_code(client),     
                            esp_http_client_get_content_length(client)); 
                    ESP_LOGI(TAG, "data:%s", output_buffer);

                    cJSON *root = NULL;
                    root = cJSON_Parse(output_buffer);
                    cJSON *city     = cJSON_GetObjectItem(root, "city");
                    cJSON *week     = cJSON_GetObjectItem(root, "week");   
                    cJSON *wea      = cJSON_GetObjectItem(root, "wea"); 
                    cJSON *wea_img  = cJSON_GetObjectItem(root, "wea_img");  
                    cJSON *temp     = cJSON_GetObjectItem(root, "tem");  
                    cJSON *humidity = cJSON_GetObjectItem(root, "humidity");

                    

                    if( !strcmp("yu", wea_img->valuestring) )
                    {   
                        lv_img_set_src(guider_ui.weather, &_rain_100x100);
                    }
                    else if( !strcmp("xue", wea_img->valuestring) )
                    {
                        lv_img_set_src(guider_ui.weather, &_snow_100x100);
                    }
                    else if( !strcmp("lei", wea_img->valuestring) )
                    {
                        lv_img_set_src(guider_ui.weather, &_storm_100x100);
                    }
                    else if( !strcmp("qing", wea_img->valuestring) )
                    {
                        lv_img_set_src(guider_ui.weather, &_sun_100x100);
                    }                                        
                    else if( !strcmp("yin", wea_img->valuestring) )
                    {
                        lv_img_set_src(guider_ui.weather, &_windy_100x100);
                    }   
                    lv_label_set_text(guider_ui.city, city->valuestring);
                    lv_label_set_text(guider_ui.week, week->valuestring);

                    lv_label_set_text(guider_ui.wea, wea->valuestring);
                    sprintf(tempandhumidity, "%s°",temp->valuestring);
                    lv_label_set_text(guider_ui.temp, tempandhumidity);
                    lv_label_set_text(guider_ui.humiditylabel,  humidity->valuestring);
                    
                    ESP_LOGI(TAG, "city:%s  week:%s      wea:%s ",city->valuestring, week->valuestring, wea->valuestring );
                    ESP_LOGI(TAG, "tem:%s   humidity:%s  wea_img:%s       ",temp->valuestring, humidity->valuestring, wea_img->valuestring);
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to read response");
                }
            }
            esp_http_client_close(client);
        }
        vTaskDelay(60000 / portTICK_PERIOD_MS);
        vTaskDelay(60000 / portTICK_PERIOD_MS);
        vTaskDelay(60000 / portTICK_PERIOD_MS);
        vTaskDelay(60000 / portTICK_PERIOD_MS);
        vTaskDelay(60000 / portTICK_PERIOD_MS);
    }        
    ESP_LOGI(TAG, "Finish http example");
    vTaskDelete(NULL);
}



































