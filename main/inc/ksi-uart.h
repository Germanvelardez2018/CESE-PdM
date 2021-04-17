/*
 * ksi-uart.h
 *
 *  Created on: 17 abr. 2021
 *      Author: German Velardez
 *      email: germanvelardez16@gmail.com
 */

#ifndef MAIN_INC_KSI_UART_H_
#define MAIN_INC_KSI_UART_H_

#include <stdio.h>
#include <string.h>
#include "driver/uart.h"
#include "soc/uart_struct.h"

/*
 * Defines
 * */

#define BUF_SIZE 256
#define BUF_RX (BUF_SIZE)
#define BUF_TX	(BUF_SIZE)

/*
  Functions uart
  */

void ksi_serial_init(int8_t num_uart,int32_t baud);
void ksi_serial_close(int num_uart);
void ksi_serial_print(const char *fmt, ...); // custom printf() function

int8_t ksi_serial_setbaud(int32_t baud);
int32_t ksi_serial_getbaud();



/*
 * Task
 * */
void ksi_task_serial(void *pvParameter);

/*
 *  Custom var
 * */


struct ksi_commands{
	char* commands;
	int8_t size;
};
typedef struct ksi_commands ksi_commands;


#endif /* MAIN_INC_KSI_UART_H_ */
