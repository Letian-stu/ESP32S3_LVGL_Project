#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "app_sem_queue.h"
#include "app_task.h"
#include "gui_guider.h"

static const char *TAG = "Wifi";

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static EventGroupHandle_t s_wifi_event_group;

extern TaskHandle_t Http_Task_Handler;
extern TaskHandle_t wifi_Task_Handler;
extern void http_test_task(void *p);

uint8_t scan_wifi = 1;
static int s_retry_num = 0;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < 5)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        }
        else
        {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "connect to the AP fail");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "connect wifi cuccess got ip:" IPSTR, IP2STR(&event->ip_info.ip) );
        lv_label_set_text_fmt(guider_ui.label_ip, "IP:%d.%d.%d.%d",IP2STR(&event->ip_info.ip));
        
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_thread_entry(void *p)
{

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    uint16_t number; 
    wifi_ap_record_t ap_info[6];
    char AllWifiName[256];
    uint8_t EventValue = 0;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Wifi Init Success");
    s_wifi_event_group = xEventGroupCreate();
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "tian",
            .password = "11112222",
            .threshold.authmode = WIFI_AUTH_WPA2_WPA3_PSK,
        },
    };
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));
                                                     
    ESP_LOGI(TAG, "ssid:%s", wifi_config.sta.ssid);
    ESP_LOGI(TAG, "password:%s", wifi_config.sta.password);
    esp_wifi_stop();
    
#if 1
    vTaskDelay(500 / portTICK_PERIOD_MS);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                            pdFALSE,
                                            pdFALSE,
                                            portMAX_DELAY);

    xTaskCreate(http_test_task, "http_test_task", 4096*2, NULL, 3, &Http_Task_Handler);       
#endif                         
    vTaskSuspend(NULL);
    while (1)
    {
        if (scan_wifi == 1)
        {
            // portDISABLE_INTERRUPTS();
            lv_textarea_set_text(guider_ui.ta, "");
            // guider_ui.spinner = lv_spinner_create(lv_scr_act(), 1000, 60);
            // lv_obj_set_size(guider_ui.spinner, 60, 60);
            // lv_obj_align(guider_ui.spinner, LV_ALIGN_CENTER, 0, 20);
            // portENABLE_INTERRUPTS();  
            //lv_textarea_clear_selection(guider_ui.ta);            
            
            // lv_textarea_del_char(guider_ui.ta);
            esp_wifi_scan_start(NULL, true);
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&number));
            if (number > 5)
                number = 5;
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
            if (number > 5)
                number = 5;
            sprintf(AllWifiName, "Success\n%s\n%s\n%s\n%s\n%s", ap_info[0].ssid, ap_info[1].ssid, ap_info[2].ssid, ap_info[3].ssid, ap_info[4].ssid);
            // printf("AllWifiName = %s \n", AllWifiName);

            // portDISABLE_INTERRUPTS();
            // lv_obj_del(guider_ui.spinner);
            // portENABLE_INTERRUPTS(); 

            lv_dropdown_set_options(guider_ui.table, AllWifiName);
        }
        EventValue = xEventGroupWaitBits(Wifi_Event_Group, WIFIALLAVENT, pdTRUE, pdFALSE, 9000 / portTICK_PERIOD_MS);
        if (EventValue == WIFISETNAME)
        {
            scan_wifi = 0;
            page_Menu.x.num = 2;
            page_Menu.y.num = 1;

            lv_obj_set_tile_id(guider_ui.tv, page_Menu.x.num, page_Menu.y.num, LV_ANIM_ON);

            ESP_LOGI(TAG, "recv wifi name");
        }
        if(EventValue == WIFISETSSID)
        {
            // portDISABLE_INTERRUPTS();
            // guider_ui.spinner = lv_spinner_create(lv_layer_top(), 1000, 60);
            // lv_obj_set_size(guider_ui.spinner, 60, 60);
            // lv_obj_align(guider_ui.spinner, LV_ALIGN_CENTER, 0, 20);
            // portENABLE_INTERRUPTS();
            // ESP_LOGI(TAG, "create spinner");

            // printf("ssid:%s\n", Select_WIFI);
            // printf("password:%s\n", WIFIMima);
            memcpy(wifi_config.sta.ssid, (uint8_t *)Select_WIFI, 32);
            memcpy(wifi_config.sta.password, (uint8_t *)WIFIMima, 32);
            ESP_LOGI(TAG, "ssid:%s", wifi_config.sta.ssid);
            ESP_LOGI(TAG, "password:%s", wifi_config.sta.password);
            esp_wifi_stop();
            vTaskDelay(500 / portTICK_PERIOD_MS);
            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
            ESP_ERROR_CHECK(esp_wifi_start());
            bits = xEventGroupWaitBits(s_wifi_event_group,
                                                   WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                                   pdFALSE,
                                                   pdFALSE,
                                                   portMAX_DELAY);
            if (bits & WIFI_CONNECTED_BIT)
            {
                ESP_LOGI(TAG, "connected to ap SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
                // portDISABLE_INTERRUPTS();
                // lv_obj_del(guider_ui.spinner);
                // portENABLE_INTERRUPTS();
                // ESP_LOGI(TAG, "del spinner");

                page_Menu.x.num = 0;
                page_Menu.y.num = 1;
                lv_obj_set_tile_id(guider_ui.tv, page_Menu.x.num, page_Menu.y.num, LV_ANIM_ON);
                Task_lock.page_wifi = 1;
                wifi_success_connect(&guider_ui);
                portDISABLE_INTERRUPTS();
                xTaskCreate(http_test_task, "http_test_task", 4096*2, NULL, 3, &Http_Task_Handler);
                vTaskSuspend(wifi_Task_Handler);
                portENABLE_INTERRUPTS();
            }
            else if (bits & WIFI_FAIL_BIT)
            {
                ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
                page_Menu.x.num = 0;
                page_Menu.y.num = 1;
                lv_obj_set_tile_id(guider_ui.tv, page_Menu.x.num, page_Menu.y.num, LV_ANIM_ON);
                Task_lock.page_wifi = 1;
                portDISABLE_INTERRUPTS();                  
                vTaskSuspend(wifi_Task_Handler);
                portENABLE_INTERRUPTS();
                
            }
            else
            {
                ESP_LOGE(TAG, "UNEXPECTED EVENT");
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "wifi task doing");
    }
}
