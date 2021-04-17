#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "command_processor.h"
#include "ksi_memory_store.h"
#include "ksi_serial.h"
#define KB 1000

QueueHandle_t user_interface_queue;


void app_main(void)
{
	ksi_memory_init();




	xTaskCreatePinnedToCore(ksi_task_serial, "uart listining", 50*KB, NULL, 1, NULL, 0);

	//xTaskCreatePinnedToCore(ksi_task_command_processor, "command listining", 50*KB, NULL, 1, NULL, 0);

    while (true) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        //ksi_serial_print("conteo...\n");
    }
}

