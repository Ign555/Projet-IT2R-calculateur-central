#include "dfplayermini.h"

void Init_UART3(void){
	Driver_USART3.Initialize(NULL);
	Driver_USART3.PowerControl(ARM_POWER_FULL);
	Driver_USART3.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							9600);
	Driver_USART3.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART3.Control(ARM_USART_CONTROL_RX,1);
}
void DFPlayer_send_command(unsigned char command, unsigned const char param1, unsigned const char param2) {
	
		unsigned char chkb1; //checksum high
		unsigned char chkb2; //checksum low
		int checksum; //checksum pour calculer
	
    unsigned char packet[10]; //trame à envoyer
		packet[0] = 0x7E; //start 
		packet[1] = 0xFF; //version
		packet[2] = 0x06; //taille de la data (toujours 06)
		packet[3] = command; //commande
		packet[4] = 0x00; //feedback : 00 = pas de feedback
		packet[5] = param1; //data high
		packet[6] = param2; //data low
	
		checksum = 0 - packet[1]- packet[2]- packet[3]- packet[4]- packet[5]- packet[6]; //calcul du checksum
	
		//Octet 1 et 2 de checksum
		chkb1 = (checksum >> 8) & 0xFF; //répartion du checksum en int sur 2 chars, ici le char high
		chkb2 = checksum & 0xFF; //checksum partie low
	
		packet[7] = chkb1;
		packet[8] = chkb2;
		packet[9] = 0xEF; //bit de stop
    while(Driver_USART3.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART3.Send(packet,10); //envoie du packet
		osDelay(50);
}

void DFPlayer_init(){
	Init_UART3();
	DFPlayer_send_command(DFPLAYER_INIT, 0x00, 0x00); //Initialise le DFPlayer
}
void DFPlayer_set_volume(uint8_t volume){
	if(volume > 30) volume = 30;
	DFPlayer_send_command(DFPLAYER_SET_VOLUME, 0x00, volume); //Initialise le DFPlayer
}
void DFPlayer_next(void){
	DFPlayer_send_command(DFPLAYER_NEXT,0x00,0x00); //Next	
}
void DFPlayer_play(uint8_t track){
	DFPlayer_send_command(DFPLAYER_PLAY_SPECIFIC_SOUND,0x00,0x00); //Next	
}
void DFPlayer_play_in_folder(uint8_t folder, uint8_t track){
	DFPlayer_send_command(DFPLAYER_PLAY_SPECIFIC_SOUND_IN_FOLDER,folder,track); //Next	
}