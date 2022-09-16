idf_component_register(
SRCS    "app_main.c" 
        
        "Os_Task/src/app_task.c" 
        "Os_Task/src/app_sem_queue.c" 

        "Lvgl_Task/src/lvgl_init.c" 
        "Lvgl_Task/src/gui_guider.c" 

        "Lvgl_Task/image/_cap_150x150.c"
        "Lvgl_Task/image/_game_150x150.c"
        "Lvgl_Task/image/_konhz_150x150.c"
        "Lvgl_Task/image/_set_150x150.c"
        "Lvgl_Task/image/_bg_240x240.c"
INCLUDE_DIRS 
        "." 
        "Lvgl_Task/include"
        "Os_Task/include"
)

