#include <stdio.h>
#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                   // CMSIS RTOS header file
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include "LPC17xx.h"                    // Device header
/*
RB stand for "Réception  Bluetooth"



*/
#ifndef RB_H
#define RB_H

//Définition d'une structure pour stocker la position du joystick
typedef struct JoystickPosition
{
	uint8_t x, y;
}JoystickPosition;


void RB_init(void (event_UART)(uint32_t event)); //Mettre en paramètre le nom de la fonction qui gère l'évènement ( fonction CB )

//à mettre dans un thread réveillé par une fonction de callback
int RB_get_data(uint8_t *jx, uint8_t *jy, uint8_t *b); //Récupre les informations du nunchuck, renvoie -1 en cas d'erreur

extern ARM_DRIVER_USART Driver_USART1;
#endif