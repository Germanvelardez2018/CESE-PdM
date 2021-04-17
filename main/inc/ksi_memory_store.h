/*
 * ksi_memory_store.h
 *
 *  Created on: 17 abr. 2021
 *      Author: German Velardez
 *      email: germanvelardez16@gmail.com
 */

#ifndef MAIN_INC_MEMORY_STORE_H_
#define MAIN_INC_MEMORY_STORE_H_

#include "ksi_memory_table.h";
#include "nvs_flash.h"
#include "nvs.h"






void ksi_memory_init();
void ksi_memory_deinit();
void ksi_memory_set_wifiid(char *wifi_id);
void ksi_memory_set_wifipass(char *wifi_password);
void ksi_memory_set_mqtturl(char* url);
void ksi_memory_set_mqtttag(char* tag);
void ksi_memory_set_baud(int32_t baud);




int32_t ksi_memory_get_baud();
char* ksi_memory_get_wifiid();
char* ksi_memory_get_wifipass();




#endif /* MAIN_INC_MEMORY_STORE_H_ */
