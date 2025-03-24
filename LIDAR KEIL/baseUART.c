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
	initTimer0(1999,0);
	
		//Init P2.5 en sortie
	LPC_GPIO2 -> FIODIR0 |= (1<<5); 

	//commande lidar
	Send_Lidar_Command(0x82,0x05,tab);
	
	while(1){
 
	}
		
	return 0;
}

void initTimer0(int prescaler, int MR)
{
		// Validation des 4 TIMERS   
		LPC_SC->PCONP = LPC_SC->PCONP | 0x00C00006;   
	
		// Configuration de la période de comptage
		LPC_TIM0->PR = prescaler;  // le registre PR prend la valeur du prescaler
		LPC_TIM0->MR0 = MR;    // le registre MR0 prend la valeur maximum du compteur
		//le compteur, nommé TC ici, est remis à 0 chaque fois qu'il 
	  //atteint la valeur de //MR0, MR0 est le registre qui contient la valeur de N 
		// voir la table 429
		LPC_TIM0->MCR |= 0x00000003;
	
		// validation de timer 0 et reset compteur ceci est toujours fait en dernier		
		LPC_TIM0->TCR = 1; //permet au timer de créer une interruption
	
		NVIC_SetPriority(TIMER0_IRQn,10);	
		NVIC_EnableIRQ(TIMER0_IRQn);
}

void TIMER0_IRQHandler(void){

	LPC_TIM0 -> IR = (1<<0);
	
	LPC_GPIO2 -> FIOPIN0 ^= (1<<5); 
	
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
