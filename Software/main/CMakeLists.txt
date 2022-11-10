idf_component_register(
SRCS    "app_main.c" 
        
        "Os_Task/src/app_task.c" 
        "Os_Task/src/app_sem_queue.c" 

        "Page_Task/src/page_http.c" 
        "Page_Task/src/page_wifi.c" 
        "Page_Task/src/page_fft.c" 

        "Lvgl_Task/src/lvgl_init.c" 
        "Lvgl_Task/src/gui_guider.c" 
		
        "Lvgl_Task/weather/_humidity_50x50.c"
        "Lvgl_Task/weather/_temp_50x50.c"
        "Lvgl_Task/weather/_rain_100x100.c"
        "Lvgl_Task/weather/_snow_100x100.c"
        "Lvgl_Task/weather/_storm_100x100.c"
        "Lvgl_Task/weather/_sun_100x100.c"
        "Lvgl_Task/weather/_windy_100x100.c"

        "Lvgl_Task/image/_xingqiu_120x120.c"
        "Lvgl_Task/image/_wifi_120x120.c"
        "Lvgl_Task/image/_tianqi_120x120.c"
        "Lvgl_Task/image/_music_120x120.c"
        "Lvgl_Task/image/_set_120x120.c"
        "Lvgl_Task/image/lv_font_medium_26.c"
        
        "Lvgl_Task/spaceman/astronaut1.c" 
        "Lvgl_Task/spaceman/astronaut2.c" 
        "Lvgl_Task/spaceman/astronaut3.c" 
        "Lvgl_Task/spaceman/astronaut4.c" 
        "Lvgl_Task/spaceman/astronaut5.c" 
        "Lvgl_Task/spaceman/astronaut6.c" 
        "Lvgl_Task/spaceman/astronaut7.c" 
        "Lvgl_Task/spaceman/astronaut8.c" 
        "Lvgl_Task/spaceman/astronaut9.c" 
        "Lvgl_Task/spaceman/astronaut10.c" 
        "Lvgl_Task/spaceman/astronaut11.c" 
        "Lvgl_Task/spaceman/astronaut12.c" 
        "Lvgl_Task/spaceman/astronaut13.c" 
        "Lvgl_Task/spaceman/astronaut14.c" 

INCLUDE_DIRS 
        "." 
        "Lvgl_Task/include"
        "Os_Task/include"
)

