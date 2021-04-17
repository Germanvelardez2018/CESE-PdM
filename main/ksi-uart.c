/*
 * ksi-uart.c
 *
 *  Created on: 17 abr. 2021
 *      Author: German Velardez
 *      email: germanvelardez16@gmail.com
 */

#include "ksi-uart.h"
#include <stdarg.h>

//Default value
static int8_t _uart = 1;
static int32_t _baud = 9600;
static QueueHandle_t uart_queue;   // QUEUE del uart
static QueueHandle_t user_interface_queue;
const static int32_t VALID_BAUD[5] = { 9600, 19200, 37400, 57200, 115200 };

#define PATTERN_CHR_NUM    (3)
#define PATTERN_CHR       ('C')

#define PRINT printf
/*

 Private functions
 */







/*
 * support functions
 * */
static void vprint(const char *fmt, va_list argp) {
	char string[200];
	if (0 < vsprintf(string, fmt, argp)) // build string
			{
		//  HAL_USART_Transmit(&husart2, (uint8_t*)string, strlen(string), 0xffffff); // send message via UART
		uart_write_bytes(_uart, (const char*) string, strlen(string));
	}
}

void ksi_serial_print(const char *fmt, ...) // custom printf() function
{
	va_list argp;
	va_start(argp, fmt);
	vprint(fmt, argp);
	va_end(argp);
}

static int32_t get_valid_baud(int32_t baud) {

	for (int8_t i = 0; i < sizeof(VALID_BAUD); i++) {
		if (baud <= VALID_BAUD[i]) {
			return VALID_BAUD[i];
		}
	}
	return 9600;   // never happens
}

int8_t ksi_serial_setbaud(int32_t baud) {
	if (baud == _baud) { //If _baud=19200 and set baud in 19200, than ignore the command.
		return 0;
	}
	if (!( (baud >= 9600) && (baud <= 115200) )) {
		return 0;                         //invalid param
	} else {
		_baud = get_valid_baud(baud);
		return 1;
	}

}

int32_t ksi_serial_getbaud() {

	return _baud;
}


void ksi_serial_init(int8_t uart, int32_t baud) {
	uart_config_t uart_config = { .baud_rate = baud, .data_bits =
			UART_DATA_8_BITS, .parity = UART_PARITY_DISABLE, .stop_bits =
			UART_STOP_BITS_1, .flow_ctrl = UART_HW_FLOWCTRL_DISABLE, //UART_HW_FLOWCTRL_DISABLE    UART_HW_FLOWCTRL_CTS_RTS

			};
	ksi_serial_setbaud(baud);
	ESP_ERROR_CHECK(uart_param_config(uart, &uart_config));
	ESP_ERROR_CHECK(
			uart_set_pin(uart,16,17,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK(
			uart_driver_install(uart,(const int)2*BUF_RX,(const int)2*BUF_TX,20,&uart_queue,0));
	//Set uart pattern detect function.
	uart_enable_pattern_det_baud_intr(uart, PATTERN_CHR, PATTERN_CHR_NUM, 9, 0,
			0); // pattern is LF
	//Reset the pattern queue length to record at most 20 pattern positions.
	uart_pattern_queue_reset(uart, 20); //Se resetea la cola de posiciones en 20

}

void ksi_serial_close(int num_uart){
	uart_driver_delete(num_uart);
}



void ksi_task_serial(void *pvParameter) {
	ksi_serial_init(_uart, _baud);

	uart_event_t event; //Estructura para gestionar evento de uart, es el objeto que se usa en la QueueUart
	size_t buffered_size;
	uint8_t *input_data = (uint8_t*) malloc(BUF_SIZE);
	char *com;    //BUFFER ESCLUSIVO PARA COMANDOS
	ksi_commands uart_commads;

	for (;;) {
		if (xQueueReceive(uart_queue, (void*) &event,
				(portTickType) portMAX_DELAY)) {
			switch (event.type) {
			case UART_DATA:
				PRINT("[UART DATA]: %d", event.size);
				uart_read_bytes(_uart, input_data, event.size, portMAX_DELAY); //Leo un comando
				uart_write_bytes(_uart, (const char*) input_data, event.size);
				ksi_serial_print(" -> No es un comando valido \n");
				break;

			case UART_FIFO_OVF:
				PRINT( "hw fifo overflow");
				uart_flush_input(_uart);
				xQueueReset(uart_queue);
				break;

			case UART_BUFFER_FULL:
				PRINT("ring buffer full");
				uart_flush_input(_uart);
				xQueueReset(uart_queue);
				break;

			case UART_BREAK:
				PRINT("uart rx break");
				break;
			case UART_PARITY_ERR:
				PRINT("uart parity error");
				break;

			case UART_FRAME_ERR:
				PRINT("uart frame error");
				break;

			case UART_PATTERN_DET:
				//printf("Se detecto el patron\n");
				uart_get_buffered_data_len(_uart, &buffered_size);
				int pos = uart_pattern_pop_pos(_uart);
				if (pos == -1) {
					uart_flush_input(_uart);
				} else {

					com = (char*) malloc(pos + 1);
					uart_read_bytes(_uart, com, pos, 100 / portTICK_PERIOD_MS); //lectura del comando
					uint8_t pat[PATTERN_CHR_NUM + 1];
					memset(pat, 0, sizeof(pat)); //sive para agregar final a cadenas
					uart_read_bytes(_uart, pat, PATTERN_CHR_NUM,
							100 / portTICK_PERIOD_MS);
					uart_flush(_uart);
					com[pos] = '\0';

					//printf("ESte es el comando recibido %s\n",com);

					uart_commads.commands = (char*) malloc(pos + 1);
					strcpy(uart_commads.commands, com);
					uart_commads.size = pos + 1;
					// printf("ESte es el comando recibidos %s\n",uart_commads.commands);
					xQueueSendToBack(user_interface_queue, &uart_commads,
							500/portTICK_RATE_MS);
				}
				break;
				//Others
			default:
				PRINT("defaut case event type: %d", event.type);
				break;

			}
		}  //if

	} //for
}


