/*
 * command_processor.h
 *
 *  Created on: 17 abr. 2021
 *      Author: German Velardez
 *      email: germanvelardez16@gmail.com
 */


#ifndef MAIN_INC_COMMAND_PROCESSOR_H_
#define MAIN_INC_COMMAND_PROCESSOR_H_




void ksi_task_command_processor(void *pvParameter);




typedef enum{
	WAITING,
	WORKING,    //workin tiene substados
	ENDING,


	UNDEFINED
} State_t;

typedef enum {
	EVENT_NEXT_COMMAND,
	EVENT_EMPTY_QUEUE,
	EVENT_REPONDED,
	EVENT_ENDED,
	EVENT_ERROR

} Event_t;


typedef Event_t (*Runnable_t)();




typedef struct {
	State_t current;
	Event_t event;
	State_t next;
	Runnable_t action;
} FSMUser_interface_t;





#endif /* MAIN_INC_COMMAND_PROCESSOR_H_ */
