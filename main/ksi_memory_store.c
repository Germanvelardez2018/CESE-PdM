/*
 * ksi_memory_store.c
 *
 *  Created on: 17 abr. 2021
 *      Author: German Velardez
 *      email: germanvelardez16@gmail.com
 */

#include "ksi_memory_store.h"
#include "ksi_serial.h"

#include <stdio.h>
//static var

static nvs_handle_t _memory_handle;
static int8_t _memory_initialized = 0;
static char *_wifi_id;
static char *_wifi_password;
const static int32_t VALID_BAUD[5] = { 9600, 19200, 37400, 57200, 115200 };

/*var periferal*/

int32_t m_baudrate = 9600;



/*
 *  all validation functions are done in this file
 *
 * */

static int32_t get_valid_baud(int32_t baud) {

	for (int8_t i = 0; i < sizeof(VALID_BAUD); i++) {
		if (baud <= VALID_BAUD[i]) {
			return VALID_BAUD[i];
		}
	}
	return 9600;   // never happens
}

static void ksi_memory_open() {

	esp_err_t err = nvs_open(CONFIG_REG, NVS_READWRITE, &_memory_handle);
	if (err != ESP_OK) {
		printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
	} else {
		printf("Open config success\n");
		_memory_initialized = 1;

	}
}

static void ksi_memory_close() {
	nvs_commit(_memory_handle);
	nvs_close(_memory_handle);
	_memory_initialized = 0;
}

void ksi_memory_init() {

	printf("Iniciando store \n");
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//			   ESP_ERROR_CHECK(nvs_flash_erase());
		//		   ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	_memory_initialized = 1;
}

void ksi_memory_deinit() {
	printf("Iniciando store \n");
	esp_err_t ret = nvs_flash_deinit();
	ESP_ERROR_CHECK(ret);
	_memory_initialized = 0;
}

void ksi_memory_set_wifiid(char *wifi_id) {
	if (wifi_id != NULL) {
		ksi_memory_open();
		esp_err_t err = nvs_set_str(_memory_handle, ID_WIFI, wifi_id);
		if (err != ESP_OK) {
			printf("Error al conectarse a red %s\n", wifi_id);
		} else {
			_wifi_id = malloc(sizeof(strlen(wifi_id) + 1));
			strcpy(wifi_id, _wifi_id);
			printf("Se configuro ID_WIFI: %s\n", _wifi_id);
		}
		ksi_memory_close();
	} else {
		printf("(SETWIFI) Parametro invalido ignorado %s\n", wifi_id);
	}

}

void ksi_memory_set_wifipass(char *wifi_password) {
	ksi_memory_open();
	if (wifi_password != NULL) {
		esp_err_t err = nvs_set_str(_memory_handle, PASSWORD_WIFI,
				wifi_password);

		if (err != ESP_OK) {
			printf("Error al guarda el password  \n");
		} else {
			_wifi_password = malloc(sizeof(strlen(wifi_password) + 1));
			strcpy(wifi_password, _wifi_password);

			printf("Se configuro el password de la red  %s\n", _wifi_password);

		}

		ksi_memory_close();
	} else {
		printf("(SETWIFI) Parametro invalido ignorado %s\n", wifi_password);
	}

}

void ksi_memory_set_mqtturl(char *url) {

	if (url != NULL) {
		ksi_memory_open();
		esp_err_t err = nvs_set_str(_memory_handle, MQTT_URL, url);

		if (err != ESP_OK) {
			printf("Error al configurar el servicio MQTT->url: %s\n", url);
		} else {
			printf("Se configuro MQTT->url: %s\n", url);
		}
		ksi_memory_close();
	} else {
		printf("url invalido ignorado %s\n", url);
	}

}
void ksi_memory_set_mqtttag(char *tag) {

	if (tag != NULL) {
		ksi_memory_open();
		esp_err_t err = nvs_set_str(_memory_handle, MQTT_TAG, tag);

		if (err != ESP_OK) {
			printf("Error al configurar el MQTT->TAG: %s\n", tag);
		} else {
			printf("Se configuro MQTT->:%s\n", tag);
		}
		ksi_memory_close();
	} else {
		printf(" Parametro TAG invalido ignorado %s\n", tag);
	}

}

void ksi_memory_set_baud(int32_t baud) {

	if (baud == m_baudrate) { //If _baud=19200 and set baud in 19200, than ignore the command.
		return;
	}
	if (!((baud >= 9600) && (baud <= 115200))) {
		return;                         //invalid param
	} else {
		m_baudrate = get_valid_baud(baud);
		ksi_serial_getbaud(m_baudrate);
		ksi_memory_open();
		esp_err_t err = nvs_set_i32(_memory_handle, BAUDRATE, m_baudrate);

		if (err != ESP_OK) {
			printf("Error al configurar el baudrate\n");
			printf("El baudrate se mantuvo en %d\n", ksi_memory_get_baud());
			ksi_memory_close();
		} else {
			printf("Se configuro exitosamente  baudrate: %d\n", m_baudrate);
		}

		return;
	}

}

int32_t ksi_memory_get_baud() {
	int32_t baudrate;
	ksi_memory_open();
	esp_err_t err = nvs_get_i32(_memory_handle, BAUDRATE, &baudrate);
	if (err != ESP_OK) {
		printf("Error al leer el baudrate\n");
	}
	ksi_memory_close();
	return baudrate;
}

char* ksi_memory_get_wifiid() {

	ksi_memory_open();
	size_t required_size;
	nvs_get_str(_memory_handle, ID_WIFI, NULL, &required_size);
	char *id = (char*) malloc(required_size);
	esp_err_t err = nvs_get_str(_memory_handle, ID_WIFI, id, &required_size);
	if (err != ESP_OK) {
		printf("Error al leer el el WIFI_ID\n");
		id = NULL;
	} else {
		printf("EL ID es : %s\n", id);
	}
	ksi_memory_close();
	return id;
}

char* ksi_memory_get_wifipass() {

	ksi_memory_open();
	size_t required_size;
	nvs_get_str(_memory_handle, PASSWORD_WIFI, NULL, &required_size);
	char *pass = (char*) malloc(required_size);
	esp_err_t err = nvs_get_str(_memory_handle, PASSWORD_WIFI, pass,
			&required_size);

	if (err != ESP_OK) {
		printf("Error al leer el el PASS_ID\n");
		pass = NULL;
	} else {
		printf("EL PASS es : %s\n", pass);
	}

	ksi_memory_close();
	return pass;
}







