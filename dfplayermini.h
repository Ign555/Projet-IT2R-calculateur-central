#include "Driver_USART.h" // ::CMSIS Driver:USART

#ifndef DFPLAYER_H
#define DFPLAYER_H

//DFPlayer command for the project
#define DFPLAYER_INIT 0x3F
#define DFPLAYER_NEXT 0x01
#define DFPLAYER_SET_VOLUME 0x06
#define DFPLAYER_PLAY_SPECIFIC_SOUND 0x03
#define DFPLAYER_PLAY_SPECIFIC_SOUND_IN_FOLDER 0x0F

//Specification
/*
Volume max : 30


*/
void DFPlayer_init();
void DFPlayer_next();
void DFPlayer_set_volume(uint8_t volume);
void DFPlayer_play(uint8_t track);
void DFPlayer_play_in_folder(uint8_t folder, uint8_t track);

void DFPlayer_send_command(unsigned char command, unsigned const char param1, unsigned const char param2);

extern ARM_DRIVER_USART Driver_USART1;
#endif