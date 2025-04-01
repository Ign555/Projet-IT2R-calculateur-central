#include "Driver_USART.h"               // ::CMSIS Driver:USART

#ifndef __MODULE_RFID_H__
#define __MODULE_RFID_H__


void RFID_init();
void RFID_read(char *buff); //à appeler dans une callback

extern ARM_DRIVER_USART Driver_USART2;


#endif