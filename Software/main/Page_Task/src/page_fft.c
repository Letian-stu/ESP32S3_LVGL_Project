/*
 * @Author: error: git config user.name & please set dead value or install git
 * @Date: 2022-11-08 14:11
 * @LastEditors: error: git config user.name & please set dead value or install git
 * @LastEditTime: 2022-11-10 16:05
 * @FilePath: \lvgl_menu_wifi\main\Page_Task\src\page_fft.c
 * @Description: 
 * Copyright (c) 2022 by error: git config user.name && git config user.email & please set dead value or install git, All Rights Reserved. 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "app_sem_queue.h"
#include "app_task.h"
#include "gui_guider.h"
#include "driver/adc.h"
#include "esp_dsp.h"
#include "app_sem_queue.h"

static const char *TAG = "fft";

/**
 * @brief about adc
 */
#define TIMES 1024 //数据个数
#define GET_UNIT(x) ((x >> 3) & 0x1)
#define ADC_RESULT_BYTE 4
static uint16_t adc1_chan_mask = BIT(0);
static adc_channel_t channel = ADC1_CHANNEL_0;//io1
uint32_t Result_num = 0;//输出个数
uint8_t Result_adc[TIMES] = {0}; //存储位置
/**
 * @brief about fft
 */
#define N_SAMPLES TIMES/4
uint16_t N = N_SAMPLES;
// Input test array 
__attribute__((aligned(16))) float Adc_In_buff[N_SAMPLES];
__attribute__((aligned(16))) lv_coord_t Adc_buff[N_SAMPLES];
//wind array
__attribute__((aligned(16))) float wind[N_SAMPLES];
// working complex array 
__attribute__((aligned(16))) float FFT_complex_buff[N_SAMPLES * 2];
// Pointers to result arrays
float *P_Out = &FFT_complex_buff[0];
__attribute__((aligned(16))) lv_coord_t fft_buff[N_SAMPLES];

/**
 * @brief about os
 */
TaskHandle_t fft_Task_Handler;
extern SemaphoreHandle_t 	ReadOkSemaphore;

void FFT_Task(float *input, float *output, uint16_t length )
{
    esp_err_t ret;
    //ESP_LOGI(TAG, "Start FFT");
    ret = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Not possible to initialize FFT. Error = %i", ret);
        abort();
    }
    for (int i = 0; i < length; i++)
    {
        output[i] = input[i];
    }
    // FFT
    dsps_fft2r_fc32(output, length);
    dsps_bit_rev_fc32(output, length);
    dsps_cplx2reC_fc32(output, length);
    for (int i = 0; i < length / 2; i++)
    {
        //P_Out[i] = 10 * log10f( (P_Out[i * 2 + 0] * P_Out[i * 2 + 0] + P_Out[i * 2 + 1] * P_Out[i * 2 + 1]) / length );
        P_Out[i] = (P_Out[i * 2 + 0] * P_Out[i * 2 + 0] + P_Out[i * 2 + 1] * P_Out[i * 2 + 1]) / length ;
    }
    dsps_fft2r_deinit_fc32();
    //ESP_LOGI(TAG, "End FFT");
}

static bool check_valid_data(const adc_digi_output_data_t *data)
{
    const unsigned int unit = data->type2.unit;
    if (unit > 2)
        return false;
    if (data->type2.channel >= SOC_ADC_CHANNEL_NUM(unit))
        return false;
    return true;
}

void FFT_thread_entry(void *p)
{
    BaseType_t xReturn = pdFALSE;
    // Generate hann window
    // hann 窗口
    dsps_wind_hann_f32(wind, N);
    static uint16_t color_num = 0;
    while (1)
	{
        xReturn = xSemaphoreTake( ReadOkSemaphore, portMAX_DELAY );
        if (xReturn == pdTRUE)
        {
            //ESP_LOGI(TAG, "START Read");
            for (int i = 0; i < Result_num; i += ADC_RESULT_BYTE)
            {
                adc_digi_output_data_t *p_result = (void *)&Result_adc[i];
                if (check_valid_data(p_result))
                {
                    //printf("%.2f \n", (float)(p_result->type2.data) );
                    Adc_buff[i / ADC_RESULT_BYTE] = (lv_coord_t)(p_result->type2.data);

                    #if 1
                    Adc_In_buff[i / ADC_RESULT_BYTE] = (float)(p_result->type2.data) / 10.0 * wind[i / ADC_RESULT_BYTE];
                    #else
                    Adc_In_buff[i / ADC_RESULT_BYTE] = (float)(p_result->type2.data) / 10.0;
                    #endif
                }
                else
                {
                    ESP_LOGE(TAG, "data err");
                    abort();
                    // ESP_LOGE(TAG, "Invalid data [%d_%d_%x]", p_result->type2.unit + 1, p_result->type2.channel, p_result->type2.data);
                }  
            }
#if 0   //print input
            ESP_LOGI(TAG, "===============adc/10===============");
            for (int i = 0; i < N_SAMPLES; i++)
            {
                printf("%d\n", (int16_t)Adc_buff[i] );
            }
#endif
#if 1   //fft open                  
            portDISABLE_INTERRUPTS();  
            FFT_Task(Adc_In_buff, FFT_complex_buff, TIMES / ADC_RESULT_BYTE);
            portENABLE_INTERRUPTS();
#endif 

#if 1  //128
            for(int i = 0; i < N_SAMPLES / 2; i++)//128
            {
                fft_buff[i] = (lv_coord_t)(P_Out[i]);
                if(i < 6)
                    fft_buff[i] = 0;
                if(i % 2 == 0)
                    fft_buff[i] = 0;
            }
#endif 
#if 0   //output
            ESP_LOGI(TAG, "===============output===============");
            for(int i = 0; i < N_SAMPLES / 2; i++)
            {
                printf("%d \n", (lv_coord_t)(fft_buff[i]) );
            }
#endif 
            lv_chart_set_ext_y_array(guider_ui.adcchart, guider_ui.ser1, (lv_coord_t *)Adc_buff);
            color_num++;
            if(color_num > 3)
            {
                lv_chart_set_series_color(guider_ui.fftchart, guider_ui.ser2, lv_palette_main(rand() % 18));
                lv_chart_set_series_color(guider_ui.adcchart, guider_ui.ser1, lv_palette_main(rand() % 18));
                color_num = 0;
            }
            lv_chart_set_ext_y_array(guider_ui.fftchart, guider_ui.ser2, (lv_coord_t *)fft_buff);  
            //清除剩余数据
            for (int i = 0; i < N_SAMPLES*2; i++)
            {
                FFT_complex_buff[i] = 0; 
            }
        }
        else
        {
            ESP_LOGI(TAG, "Do not recv sem");
        }
    }
}

void readadc_thread_entry(void *p)
{
    portDISABLE_INTERRUPTS();
    xTaskCreate(FFT_thread_entry, 	"fft", 	4096*4, 	NULL, 5, &fft_Task_Handler);
    portENABLE_INTERRUPTS(); 
    esp_err_t ret;
    memset(Result_adc, 0xcc, TIMES);
    //
    adc_digi_init_config_t adc_dma_config = {
        .max_store_buf_size = TIMES,
        .conv_num_each_intr = TIMES, //一个中断转换多少字节数据
        .adc1_chan_mask = adc1_chan_mask,
        //.adc2_chan_mask = adc2_chan_mask,
    };
    ESP_ERROR_CHECK(adc_digi_initialize(&adc_dma_config));

    adc_digi_configuration_t dig_cfg = {
        .conv_limit_en = 0,//是否adc采样次数限制
        .conv_limit_num = 150,//adc采样次数
        .sample_freq_hz = 30 * 1000,// 10k 采样 adc采样频率
        .conv_mode = ADC_CONV_SINGLE_UNIT_1, //adc转换模式(adc1工作 adc2工作 adc1-adc2同时工作)
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE2,//adc输出格式
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = 1;//几个通道adc被使用

    uint8_t unit = GET_UNIT(channel);
    uint8_t ch = channel;
    adc_pattern[0].atten = ADC_ATTEN_DB_11;//衰减
    adc_pattern[0].channel = ch;//通道
    adc_pattern[0].unit = unit;//单位
    adc_pattern[0].bit_width = SOC_ADC_DIGI_MAX_BITWIDTH;//数据宽度

    ESP_LOGI(TAG, "adc_pattern[0].atten is :%x",  adc_pattern[0].atten);
    ESP_LOGI(TAG, "adc_pattern[0].channel is :%x",  adc_pattern[0].channel);
    ESP_LOGI(TAG, "adc_pattern[0].unit is :%x",  adc_pattern[0].unit);

    dig_cfg.adc_pattern = adc_pattern;
    ESP_ERROR_CHECK(adc_digi_controller_configure(&dig_cfg));
    //启动数字ADC和DMA外围设备。在此之后，硬件开始工作。
    adc_digi_start();
    ESP_LOGI(TAG, "Init Success ");
    while (1)
    {
        ret = adc_digi_read_bytes(Result_adc, TIMES, &Result_num, ADC_MAX_DELAY);
        if (ret == ESP_OK || ret == ESP_ERR_INVALID_STATE)
        {
            if (ret == ESP_ERR_INVALID_STATE)
            {
                //ESP_LOGE(TAG, "ESP_ERR_INVALID_STATE");
            }
            ESP_LOGI(TAG, "====num is %d====",Result_num);
            ret = xSemaphoreGive( ReadOkSemaphore );
            if ( ret == pdTRUE )  
            {
                //ESP_LOGI(TAG, "Give ReadOkSemaphore ok");
            }
            else 
            {
                ESP_LOGE(TAG, "Give ReadOkSemaphore err"); 	
            }
            vTaskDelay(120 / portTICK_PERIOD_MS);
        }
        else if (ret == ESP_ERR_TIMEOUT)
        {
            ESP_LOGW(TAG, "No data, increase timeout or reduce conv_num_each_intr");
            vTaskDelay(1000);
        }
    }
    adc_digi_stop();
    ret = adc_digi_deinitialize();
    assert(ret == ESP_OK);
}

