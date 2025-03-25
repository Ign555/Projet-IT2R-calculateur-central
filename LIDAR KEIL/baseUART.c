#include "Driver_USART.h"
#include <LPC17XX.h>
#include "GPIO_LPC17xx.h"               
#include "LPC17xx.h"                    

extern ARM_DRIVER_USART Driver_USART0;

void initTimer0(int prescaler, int MR);

void TIMER0_IRQHandler(void);

void Init_UART0(void);

void Send_Lidar_Command(unsigned char command, unsigned char payloadsize, unsigned char payload[]);	

void Temporisation_ms(int a); 													//fonction de temporisation en ms


int main (void){
	
	//Variables
	unsigned char tab[5] = {0x00,0x00,0x00,0x00,0x00};
	
	//Initialisations
	Init_UART0();
	initTimer0(0,999);
	
	//commande lidar
	LPC_PWM1->MR6 = 500; 																	// 0 = 0% , 999 = 100%
	
	while(1){
 
	}
		
	return 0;
}

void initTimer0(int prescaler, int MR)
{
	LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040;   // enable PWM1
	LPC_PWM1->PR = 0;  // prescaler
	LPC_PWM1->MR0 = 999;    // MR0+1=100   la période de la PWM vaut 50ms 20000hz 1249
	
	LPC_PWM1->MCR = LPC_PWM1->MCR | 0x00000002; // Compteur relancé quand MR0 repasse à 0
	LPC_PWM1->LER = LPC_PWM1->LER | 0x0000007F;  // ceci donne le droit de modifier dynamiquement la valeur du rapport cyclique
																						 // bit 0 = MR0    bit 1 MR1 bit2 MR2 bit3 = MR3
	LPC_PWM1->PCR = LPC_PWM1->PCR | 0x0000ff00;  // autorise les sortie PWM1/2/3 bits 9, 10, 11
	LPC_PINCON->PINSEL4 = LPC_PINCON->PINSEL4| 0x00000400;
	LPC_PWM1->PCR = LPC_PWM1->PCR | 0x00000e00;  // autorise les sortie PWM1/2/3 bits 9, 10, 11
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7| 0x000C0000;
	
	LPC_PWM1->TCR = 1;  /*validation de timer  et reset counter */	
	
}




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
