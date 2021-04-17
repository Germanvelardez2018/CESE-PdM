/*
 * command_processor.c
 *
 *  Created on: 17 abr. 2021
 *      Author: German Velardez
 *      email: germanvelardez16@gmail.com
 */


#include "command_processor.h"
#include "ksi_serial.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ksi_memory_store.h"


extern QueueHandle_t user_interface_queue;


static State_t stateProcessor; // If I'm in this state...


static bool wifi_state_connection =false;

static uint8_t modeProcessor;
static ksi_commands command;
static uint8_t state_size;
static Event_t ev;


static int16_t get_param_int16(char* command)
{

	char * param = command + (SIZESET+SIZECOM+1) ;
	int16_t param_value = (int16_t)strtol(param,NULL,10);
	ksi_serial_print("el parametro es: %d\n",param_value);
	return param_value;

}


static char* set_WIFI_params(char* command)
{
	char *params = command +(SIZESET+SIZECOM+1);
	char* pass = strcasestr(params,"+");  //pass empezara con +,borrar
	int8_t lenid=pass-params;
	char* id =malloc(lenid +1);
	strncpy(id,params,lenid);
	id[lenid]='\0';
	ksi_serial_print("el id es len %s\n",id);
	ksi_serial_print("el pass es %s\n",pass+1);
	ksi_memory_set_wifiid(id);
	ksi_memory_set_wifipass(pass+1);
	return pass;
}



static void get_WIFI_params()
{
	printf("El IDWIFI configurado es: %s\n",ksi_memory_get_wifiid());
}



uint8_t pre_processor(char* command)
{
	if(!strncmp(command,SET,strlen(SET)))
	{
		return 1;   //is a SET command
	}
	if(!strncmp(command,GET,strlen(GET)))
	{
		 return 2;   //is a GET command
	}
	if(!strncmp(command,_ON,strlen(_ON)))
	{
		return 3;   //is a GET command
	}
	if(!strncmp(command,OFF,strlen(OFF)))
	{
	  return 4;   //is a GET command
	}

	return 0;  //DEFAULT COMMANDO INVALIDO
}

static void baud_function(int8_t function, int32_t params) {
	// funct is 0 ==set , funct is 1 == get
	if (function) {
		ksi_memory_get_baud();
	} else {
		ksi_memory_set_baud(params);

	}
}

static void wifi_function(int8_t function, char *command) {
	// funct is 0 ==set , funct is 1 == get

	if (function) {
		printf("get wifi params:\n");
		ksi_memory_get_wifiid();

	} else {
		printf("set wifi data\n");
		set_WIFI_params(command);

	}
}



static uint8_t processor(char *commands, int8_t function) {
	char *maincommand = commands + SIZEPRECOM; // offset set,get or off is 3. offset of on is 2

	if (!strncmp(maincommand, COMBAUD, SIZECOM)) {

		baud_function(function, get_param_int16(commands));
		return 1;   //is a  BAUD COMMAND
	}
	if (!strncmp(maincommand, COMWIFI, SIZECOM)) {
		wifi_function(function, commands);
		return 2;   //is a WIFI COMMAND
	}
	if (!strncmp(maincommand, COMMQTT, SIZECOM)) {
		return 3;   //is a MQTT command
	}

	return 0;  //DEFAULT COMMANDO INVALIDO
}


static uint8_t processor_bool(char *commands, int8_t function) {
	char *maincommand = commands + SIZEPRECOM; // offset set,get or off is 3. offset of on is 2

	if (!strncmp(maincommand, COMWIFI, SIZECOM)) {
		return 2;   //is a WIFI COMMAND
	}
	if (!strncmp(maincommand, COMMQTT, SIZECOM)) {
		return 3;   //is a MQTT command
	}

	return 0;  //DEFAULT COMMANDO INVALIDO
}


static void wifi_connected()
{
	if(!wifi_state_connection){
		wifi_state_connection=true;
	//	wifiBegin(get_wifi_id(), get_wifi_pass());
	}
	else{
		printf("el wifi esta activado\n");
	}

}


static void wifi_disconnected(){
	if(wifi_state_connection){
	//	wifi_Close();
		wifi_state_connection = false;
	}

}

static void readCommand(char *command) {

	printf("comando leido por interfaz: %s\n", command);

	int num_com = pre_processor(command);

	switch (num_com) {

	case NUM_ERROR:
		ksi_serial_print("comando invalido\n");
		break;
	case NUM_SET:
		ksi_serial_print("El comando recibido es un SET\n");
		processor(command, 0);   // func 0 is set

		break;
	case NUM_GET:
		ksi_serial_print("El comando recibido es un GET\n");
		processor(command, 1); // func 1 is get
		break;

	case NUM__ON:
		ksi_serial_print("El comando recibido es un ON WIFI\n");
		wifi_connected();
		break;
	case NUM_OFF:
		ksi_serial_print("El comando recibido es un OFF\n");
		wifi_disconnected();
		break;

	default:
		ksi_serial_print("comando invalido\n");
	}

}

static void EndingResponse() {      // Aqui irian funciones de cierre a los comandos realizados
	printf("Finalizando la respuesta\n ");
}




static Event_t ProcessCommand() {
	readCommand(command.commands);
	return EVENT_REPONDED;
}

static Event_t ProcessFinally() {
	EndingResponse();
	return EVENT_ENDED;
}









FSMUser_interface_t fsmTable[] = {

{ WAITING, EVENT_NEXT_COMMAND, WORKING, &ProcessCommand }, { WORKING,
		EVENT_REPONDED, ENDING, &ProcessFinally }, { ENDING, EVENT_ENDED,
		WAITING, NULL } };





static void initProcessor(uint8_t mode) //Modo 0 interfaz usuario pc , modo 1 interfaz microcontrolador wentux
{
	modeProcessor = mode;
	stateProcessor = WAITING;  //Empezamos en modo no hacer nada
	command.commands = NULL;
	command.size = 0;
	state_size = sizeof(fsmTable) / sizeof(FSMUser_interface_t);

}


void ksi_task_command_processor(void *pvParameter){

	initProcessor(0);   // configuraciones inciiales


	for (;;) {

		ev = xQueueReceive(user_interface_queue, &command,
				(portTickType) portMAX_DELAY) ?
				EVENT_NEXT_COMMAND : EVENT_EMPTY_QUEUE;

		if (ev == EVENT_NEXT_COMMAND) {

			for (uint8_t i = 0; i < state_size; i++) {

				if (fsmTable[i].current == stateProcessor
						&& fsmTable[i].event == ev) {

					if (fsmTable[i].action != NULL) {
						ev = (Event_t) (*(fsmTable[i].action))(); // supuestemnte estoy llamoando al callback
					}
					stateProcessor = fsmTable[i].next;
				}
			}

		}
		vTaskDelay(200 / portTICK_PERIOD_MS);

	}


}

