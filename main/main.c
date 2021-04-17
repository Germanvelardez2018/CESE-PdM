#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"



#include "ksi_serial.h"

void app_main(void)
{

	ksi_serial_init(1, 19200);
	int counter=0;
    while (true) {
    	ksi_serial_print("counter: %d\n",counter++);
        vTaskDelay(2500 / portTICK_PERIOD_MS);
    }
}

