#include <stdio.h>
#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                   // CMSIS RTOS header file
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "LPC17xx.h"                    // Device header
/*
RB stand for "R�ception  Bluetooth"



*/
#ifndef RB_H
#define RB_H

//D�finition d'une structure pour stocker la position du joystick
typedef struct JoystickPosition
{
	uint8_t x, y;
}JoystickPosition;


void RB_init(void (event_UART)(uint32_t event)); //Mettre en param�tre le nom de la fonction qui g�re l'�v�nement ( fonction CB )

//� mettre dans un thread r�veill� par une fonction de callback
int RB_get_data(uint8_t *jx, uint8_t *jy, uint8_t *b); //R�cupre les informations du nunchuck, renvoie -1 en cas d'erreur

extern ARM_DRIVER_USART Driver_USART1;
#endif