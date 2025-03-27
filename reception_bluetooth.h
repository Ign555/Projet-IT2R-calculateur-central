#include <stdio.h>
#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                   // CMSIS RTOS header file

#ifndef RB_H
#define RB_H


//à mettre dans un thread réveillé par une fonction de callback
int RB_get_data(uint8_t *jx, uint8_t *jy, uint8_t *b); //Récupre les informations du nunchuck, renvoie -1 en cas d'erreur
#endif