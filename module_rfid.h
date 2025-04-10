#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                   // CMSIS RTOS header file
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "LPC17xx.h"                    // Device header

#ifndef __MODULE_RFID_H__
#define __MODULE_RFID_H__


void RFID_init(void (event_RFID)(uint32_t event));
void RFID_read(char *buff); //à appeler dans une callback

extern ARM_DRIVER_USART Driver_USART2;


#endif