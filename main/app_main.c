#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "board.h"

#include "audio_mem.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"
#include "freq_finder.h"


#include "MenuSystem.h"
#include "freertos/timers.h"
#include "TreasureFinder.h"
#include "Rotary.h"

int id = 15;
static const char *TAG = "MAIN";

int app_main(void){

    //  menu_main();

    //Callback for the rotary encoder. This checks the rotary encoder for inputs every 0.1 sec.
	TimerHandle_t timer_100_ms = xTimerCreate("MyTimer0.1s", pdMS_TO_TICKS(100), pdTRUE, (void *)id, &timer_100_ms_callback);
     if (xTimerStart(timer_100_ms, 10) != pdPASS)
     {
         ESP_LOGE(TAG, "Cannot start 0.1 second timer");
     }

    RotaryEncoder_init();
	
    RotaryEncoder_setColorRed(0);
    RotaryEncoder_setColorGrn(100);
    RotaryEncoder_setColorBlu(255);

    return 1;
}