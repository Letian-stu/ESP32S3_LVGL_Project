# FFT Example 

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use FFT functionality from esp-dsp library. Example does the following steps:

1. Initialize the library
2. Initialize input signals with 1024 samples: one 0 dB, second with -20 dB
3. Combine two signals as one complex input signal and apply window to input signals paar.
4. Calculate FFT for 1024 complex samples
5. Apply bit reverse operation for output complex vector
6. Split one complex FFT output spectrum to two real signal spectrums 
7. Show results on the plots
8. Show execution time of FFT

## How to use example

### Hardware required

This example does not require any special hardware, and can be run on any common development board.

### Configure the project

If using Make based build system, run `make menuconfig` and set serial port under Serial Flasher Options. 
If using CMake based build system, no configuration is required.
Also, under Component Config/DSP Library/Optimization for ESP32, it's possible to select optimized or ansi implementation to compare. 

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
make -j4 flash monitor
```

Or, for CMake based build system (replace PORT with serial port name):

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example output

Here is an typical example console output. 

```
I (59) main: Start Example.
W (89) main: Signal x1
I (89) view: Data min[495] = -162.760925, Data max[164] = 23.938747
 ________________________________________________________________
0                                                                |
1                    |                                           |
2                    |                                           |
3                    |                                           |
4                    |                                           |
5                    |                                           |
6                   | |                                          |
7                   | |                                          |
8                  || ||                                         |
9||||||||||||||||||     ||||||||||||||||||||||||||||||||||||||||||
 0123456789012345678901234567890123456789012345678901234567890123
I (159) view: Plot: Length=512, min=-60.000000, max=40.000000
W (169) main: Signal x2
I (169) view: Data min[502] = -164.545135, Data max[205] = 3.857752
 ________________________________________________________________
0                                                                |
1                                                                |
2                                                                |
3                         |                                      |
4                         |                                      |
5                         |                                      |
6                         |                                      |
7                         ||                                     |
8                        | |                                     |
9||||||||||||||||||||||||   ||||||||||||||||||||||||||||||||||||||
 0123456789012345678901234567890123456789012345678901234567890123
I (249) view: Plot: Length=512, min=-60.000000, max=40.000000
W (249) main: Signals x1 and x2 on one plot
I (259) view: Data min[505] = -159.215271, Data max[164] = 23.938747
 ________________________________________________________________
0                                                                |
1                    |                                           |
2                    |                                           |
3                    |    |                                      |
4                    |    |                                      |
5                    |    |                                      |
6                   | |   |                                      |
7                   | |   ||                                     |
8                  || || | |                                     |
9||||||||||||||||||     |   ||||||||||||||||||||||||||||||||||||||
 0123456789012345678901234567890123456789012345678901234567890123
I (339) view: Plot: Length=512, min=-60.000000, max=40.000000
I (339) main: FFT for 1024 complex points take 140472 cycles
I (349) main: End Example.
```

```c
// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include <math.h>

#include "esp_dsp.h"

static const char *TAG = "main";

// This example shows how to use FFT from esp-dsp library

#define N_SAMPLES 1024
int N = N_SAMPLES;
// Input test array
__attribute__((aligned(16))) float x1[N_SAMPLES] = {
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
    128,140,153,165,177,188,199,209,218,226,234,240,245,250,253,254,255,254,253,250,245,240,234,226,218,209,199,188,177,165,153,140,128,116,103,91,79,68,57,47,38,30,22,16,11,6,3,2,1,2,3,6,11,16,22,30,38,47,57,68,79,91,103,116,
};
__attribute__((aligned(16))) float x2[N_SAMPLES];
// Window coefficients
__attribute__((aligned(16))) float wind[N_SAMPLES];
// working complex array
__attribute__((aligned(16))) float y_cf[N_SAMPLES * 2];
// Pointers to result arrays
float *y1_cf = &y_cf[0];
float *y2_cf = &y_cf[N_SAMPLES];

// Sum of y1 and y2
__attribute__((aligned(16))) float sum_y[N_SAMPLES / 2];

void app_main()
{
    esp_err_t ret;
    ESP_LOGI(TAG, "Start Example.");
    ret = dsps_fft2r_init_fc32(NULL, CONFIG_DSP_MAX_FFT_SIZE);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Not possible to initialize FFT. Error = %i", ret);
        return;
    }

    // hann 窗口
    dsps_wind_hann_f32(wind, N);
    // Generate input signal for x1 A=1 , F=1
    //生成一个输入的声音信号 x1
    //dsps_tone_gen_f32(x1, N, 128.0, 0.15, 0);
    //方波
    // for (int i = 0; i < N; i++)
    // {
    //     if( i % 10 >= 5 )
    //         x1[i] = 0.0;
    //     else
    //         x1[i] = 100.0;
    // } 
    //正弦波

    // Generate input signal for x2 A=0.1,F=0.1
    // //生成一个输入的声音信号 x2
    // dsps_tone_gen_f32(x2, N, 8.0, 0.15, 0);

    for (int i = 0; i < N; i++)
    {
        printf("%.2f \n", x1[i]);
    }
    ESP_LOGI(TAG, "=======================================");
    // for (int i = 0; i < N; i++)
    // {
    //     printf("%.2f \n", x2[i]);
    // }
    // Convert two input vectors to one complex vector
    // 将两个输入向量转换为一个复向量
    for (int i = 0; i < N; i++)
    {
        y_cf[i * 2 + 0] = x1[i] * wind[i];
        //y_cf[i * 2 + 1] = x2[i] * wind[i];
    }
    // FFT
    unsigned int start_b = dsp_get_cpu_cycle_count();
    dsps_fft2r_fc32(y_cf, N);
    unsigned int end_b = dsp_get_cpu_cycle_count();
    // Bit reverse
    // 反向
    dsps_bit_rev_fc32(y_cf, N);
    // Convert one complex vector to two complex vectors
    // 把一个复向量转换成两个复向量
    dsps_cplx2reC_fc32(y_cf, N);

    for (int i = 0; i < N / 2; i++)
    {
        y1_cf[i] = (y1_cf[i * 2 + 0] * y1_cf[i * 2 + 0] + y1_cf[i * 2 + 1] * y1_cf[i * 2 + 1]) / N;
        //y2_cf[i] = (y2_cf[i * 2 + 0] * y2_cf[i * 2 + 0] + y2_cf[i * 2 + 1] * y2_cf[i * 2 + 1]) / N;
        // Simple way to show two power spectrums as one plot
        //sum_y[i] = fmax(y1_cf[i], y2_cf[i]);
    }
    ESP_LOGI(TAG, "=======================================");
    for(int i = 0; i < N / 2; i++)
    {
        printf("%.2f \n", y1_cf[i] );
    }
    // ESP_LOGI(TAG, "=======================================");
    // for(int i = 0; i < N / 2; i++)
    // {
    //     printf("%.2f \n", y2_cf[i] );
    // }
    // for(int i = 0; i < N / 2; i++)
    // {
    //     printf("%.2f \n", sum_y[i] );
    // }
    // Show power spectrum in 64x10 window from -100 to 0 dB from 0..N/4 samples
    ESP_LOGW(TAG, "Signal x1");
    dsps_view(y1_cf, N / 2, 128, 20, -60, 40, '|');
    // ESP_LOGW(TAG, "Signal x2");
    // dsps_view(y2_cf, N / 2, 128, 20, -60, 40, '|');
    // ESP_LOGW(TAG, "Signals x1 and x2 on one plot");
    // dsps_view(sum_y, N / 2, 128, 20, -60, 40, '|');
    // ESP_LOGI(TAG, "FFT for %i complex points take %i cycles", N, end_b - start_b);

    ESP_LOGI(TAG, "End Example.");
}   
```
