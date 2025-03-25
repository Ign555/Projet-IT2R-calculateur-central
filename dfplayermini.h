#include "Driver_USART.h" // ::CMSIS Driver:USART

#ifndef DFPLAYER_H
#define DFPLAYER_H

void DFPlayer_init();
void DFPlayer_play();
void DFPlayer_next();

extern ARM_DRIVER_USART Driver_USART1;
#endif