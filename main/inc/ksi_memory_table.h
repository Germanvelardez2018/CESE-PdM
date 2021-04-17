/*
 * ksi_memory_table.h
 *
 *  Created on: 17 abr. 2021
 *      Author: root
 */

#ifndef MAIN_INC_KSI_MEMORY_TABLE_H_
#define MAIN_INC_KSI_MEMORY_TABLE_H_



#define CONFIG_REG    "CONFIG-REG"
#define DATA		   "DATA"

#define CONFIG_REG         "CONFIG-REG"
#define CONECCTION_WIFI	   "CONECCTION_WIFI"
#define ID_WIFI            "ID_WIFI"
#define PASSWORD_WIFI      "PASSWORD_WIFI"
#define MQTT_URL			"MQTT_URL"
#define MQTT_TAG			"MQTT_TAG"

// NAMES




//------------------CONNECTIONS
#define MQTT_ON			  0
#define MQTT_CONNECTED	  0
#define WIFI_ON           0
#define WIFI_CONNECTED	  0



//------------------UART CONFIG

#define BAUDRATE           "BAUDRATE"
#define BAUDRATE_VALUE     9600
#define MODE  			   "MODE"
#define MODE_VALUE  		0


//-----------------ALARM GLOBAL

#define GLOBAL_ALARM "GLOBAL_ALARM"
#define GLOBAL_ALARM_VALUE 0       //O == OFF      1 ==ON


#define MAX_NUM_NODES 10



//-----------------------COMMANDS


#define SET             "SET"
#define GET             "GET"
#define	_ON				"_ON"
#define OFF				"OFF"


#define COMBAUD	    "BAUD"
#define COMWIFI	    "WIFI"
#define COMMQTT		"MQTT"

#define SIZECOM		4


#define SIZESET    3    //size of SET O GET
#define SIZEGET    3
#define SIZE_ON	   3
#define SIZEOFF    3
#define SIZEPRECOM 3


/*num_processed
 * */
#define NUM_ERROR           0
#define NUM_SET             1
#define NUM_GET             2
#define NUM__ON             3
#define NUM_OFF             4





#endif /* MAIN_INC_KSI_MEMORY_TABLE_H_ */
