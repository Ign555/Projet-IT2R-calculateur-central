#include <stdio.h>
#include "osObjects.h"                      // RTOS object definitions
#include "cmsis_os.h"                   // CMSIS RTOS header file

#ifndef RB_H
#define RB_H


//� mettre dans un thread r�veill� par une fonction de callback
int RB_get_data(uint8_t *jx, uint8_t *jy, uint8_t *b); //R�cupre les informations du nunchuck, renvoie -1 en cas d'erreur
#endif