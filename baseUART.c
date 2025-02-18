#include "Driver_USART.h" // ::CMSIS Driver:USART
#include <LPC17XX.h>


extern ARM_DRIVER_USART Driver_USART1;
extern ARM_DRIVER_USART Driver_USART2;

void Init_UART1(void){
	Driver_USART1.Initialize(NULL);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							9600);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}

void Init_UART2(void){
	Driver_USART2.Initialize(NULL);
	Driver_USART2.PowerControl(ARM_POWER_FULL);
	Driver_USART2.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							9600);
	Driver_USART2.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART2.Control(ARM_USART_CONTROL_RX,1);
}

void Send_DFPlayer_Command(unsigned char command, unsigned const char param1, unsigned const char param2) {
	
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
    while(Driver_USART1.GetStatus().tx_busy == 1); // attente buffer TX vide
		Driver_USART1.Send(packet,10); //envoie du packet
	}	

	void Temporisation_ms(int a){ //fonction de temporisation en ms
		int j;
		for (j=0; j<(16667*a);j++){
		}
	}

int main (void){

	Init_UART1();
	Init_UART2();
	
	Send_DFPlayer_Command(0x3F,0x00,0x00); //Initialise le DFPlayer
	Temporisation_ms(1000);
	Send_DFPlayer_Command(0x03,0x01,0x01); //Joue un son mais ça marche pas, à revoir
	Temporisation_ms(5000);
	Send_DFPlayer_Command(0x01,0x00,0x00); //Next
	Temporisation_ms(5000);
	Send_DFPlayer_Command(0x01,0x00,0x00); //Next
	
	

	while (1){

	}	
	return 0;
}
