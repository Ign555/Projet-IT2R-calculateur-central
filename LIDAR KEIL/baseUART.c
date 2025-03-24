#include "Driver_USART.h" // ::CMSIS Driver:USART
#include <LPC17XX.h>
#include "GPIO_LPC17xx.h"               // Keil::Device:GPIO


extern ARM_DRIVER_USART Driver_USART0;


void Init_UART0(void){
	Driver_USART0.Initialize(NULL);
	Driver_USART0.PowerControl(ARM_POWER_FULL);
	Driver_USART0.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							115200);
	Driver_USART0.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART0.Control(ARM_USART_CONTROL_RX,1);
}


void Send_Lidar_Command(unsigned char command, unsigned char payloadsize, unsigned char payload[]) {	
		
		////////// Pour envoyer une commande sans payload, mettre payloadsize à 0x00. ///////////
	
    unsigned char packet[9]; 														//trame à envoyer // modifier la taille avec la size du payload
		char i, j;

		packet[0] = 0xA5;																			//octet de start
		packet[1] = command;																	//octet de commande
	
		if (payloadsize != 0x00) {														//si on veut un payload (taille différente de 0x00)
			
			packet[2] = payloadsize;														//taille du payload
			
			for (i=0; i<payloadsize;i++){												//on rempli les octets suivants avec ceux du payload
				packet[3+i] = payload[i];													//on rempli les octets suivants avec ceux du payload
			}
			
		packet[3+payloadsize] = 0 ^ 0xA5 ^ command ^ payloadsize;			//calcul checksum
			for (j=0; j<payloadsize; j++){															//calcul checksum
				packet[3+payloadsize] ^= payload[j];											//calcul checksum
			}
		}
		
    while(Driver_USART0.GetStatus().tx_busy == 1); 				// attente buffer TX vide
		Driver_USART0.Send(packet,9); 												//envoie du packet

}		

	void Temporisation_ms(int a){ 													//fonction de temporisation en ms
		int j;
		for (j=0; j<(16667*a);j++){
		}
	}
	
	

int main (void){

	unsigned char tab[5] = {0x00,0x00,0x00,0x00,0x00};
	
	Init_UART0();
	
	GPIO_SetDir(2,5,GPIO_DIR_OUTPUT); 													//P2.5 en sortie
	GPIO_PinWrite(2,5,1);
	
	Send_Lidar_Command(0x82,0x05,tab);
	
	while(1){
		
			Send_Lidar_Command(0x82,0x05,tab);

	}
		
	return 0;
}
