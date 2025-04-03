#include "Driver_USART.h"
#include <LPC17XX.h>
#include "GPIO_LPC17xx.h"               
#include "LPC17xx.h"                    
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "Driver_SPI.h"                 // ::CMSIS Driver:SPI
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

extern ARM_DRIVER_USART Driver_USART0;
extern GLCD_FONT GLCD_Font_16x24;

void Receive_UART(void const * argument);                                                                 //Thread pour recevoir trame UART
osThreadDef(Receive_UART, osPriorityNormal,1,0);
osThreadId ID_Receive;

void Afficher_LCD(void const * argument);                                                                 //Thread pour traiter les données et les afficher
osThreadDef(Afficher_LCD, osPriorityNormal,1,0);
osThreadId ID_LCD;

void initTimer0(int prescaler, int MR);																																		//INITIALISATION ET MISE EN PLACE TIMER SUR P2.5

void Init_UART0(void);																																										//INITIALISATION UART0 SUR P0.2 (TX) et P0.3 (RX) à 115200 bauds

void Temporisation_ms(int a);																																							//TEMPORISATION EN MS	

void Send_Lidar_Command(unsigned char command, unsigned char payloadsize, unsigned char payload[]);				//SEND LIDAR COMMAND

//Commandes spécifiques du LIDAR
void Lidar_Start_Scan(void);																																							//LIDAR START SCAN
void Lidar_Stop_Scan(void);																																								//LIDAR STOP SCAN

char ValeurRecue[5];																																							        //Variable globale pour recevoir les données du LIDAR

//---------------------------------------------------------------------------------------------------------------------------------------//
//																												MAIN
//---------------------------------------------------------------------------------------------------------------------------------------//


int main (void){
	
	//Variables
	
	//Initialisations
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	Init_UART0();
	initTimer0(0,999);
	
	osKernelInitialize();
	
	ID_Receive = osThreadCreate(osThread(Receive_UART), NULL);
	ID_LCD = osThreadCreate(osThread(Afficher_LCD), NULL);
	
	//commande lidar
	LPC_PWM1->MR6 = 500; 																											// PWM : 0 = 0% , 999 = 100%
	
	osKernelStart();                                                          //Start l'OSTR
	
	osSignalSet(ID_Receive,0x0001);                                           //Event 1 de la fonction receive à 1, pour démarrer avec elle
	
	osDelay(osWaitForever);                                                   //Sommeil inini du main

	while(1){
	
	}
		
	return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------------//
//																												INITIALISATION ET MISE EN PLACE TIMER SUR P2.5
//---------------------------------------------------------------------------------------------------------------------------------------//


void initTimer0(int prescaler, int MR)
{
	LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040;  														  // enable PWM1
	LPC_PWM1->PR = 0;  																												// prescaler
	LPC_PWM1->MR0 = 999;  																									  // MR0+1=100   la période de la PWM vaut 50ms 20000hz 1249
	
	LPC_PWM1->MCR = LPC_PWM1->MCR | 0x00000002;															  // Compteur relancé quand MR0 repasse à 0
	LPC_PWM1->LER = LPC_PWM1->LER | 0x0000007F;  															// ceci donne le droit de modifier dynamiquement la valeur du rapport cyclique
																																						// bit 0 = MR0    bit 1 MR1 bit2 MR2 bit3 = MR3
	LPC_PWM1->PCR = LPC_PWM1->PCR | 0x0000ff00;  															// autorise les sortie PWM1/2/3 bits 9, 10, 11
	LPC_PINCON->PINSEL4 = LPC_PINCON->PINSEL4| 0x00000400;
	LPC_PWM1->PCR = LPC_PWM1->PCR | 0x00000e00;  															// autorise les sortie PWM1/2/3 bits 9, 10, 11
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7| 0x000C0000;
	
	LPC_PWM1->TCR = 1;  																											/*validation de timer  et reset counter */	
	
}


//---------------------------------------------------------------------------------------------------------------------------------------//
//																												INITIALISATION UART0 SUR P0.2 (TX) et P0.3 (RX) à 115200 bauds
//---------------------------------------------------------------------------------------------------------------------------------------//


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

//---------------------------------------------------------------------------------------------------------------------------------------//
//																												TEMPORISATION EN MS											
//---------------------------------------------------------------------------------------------------------------------------------------//


void Temporisation_ms(int a){ 																							//fonction de temporisation en ms
	int j;
	for (j=0; j<(16667*a);j++){
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------//
//																												SEND LIDAR COMMAND
//---------------------------------------------------------------------------------------------------------------------------------------//


void Send_Lidar_Command(unsigned char command, unsigned char payloadsize, unsigned char payload[]) {	
		
		////////// Pour envoyer une commande sans payload, mettre payloadsize à 0x00. ///////////
	
    unsigned char packetpayload[9]; 																				//trame à envoyer
		unsigned char packetnopayload[2];																				//trame à envoyer
		char i, j, etat;
		
		if (payloadsize != 0x00) {etat=2;}																			//aller dans case avec payload
		if (payloadsize == 0x00) {etat=1;}																			//aller dans case sans payload
		
		switch (etat) {
			case 1 : 																															//case sans payload
				packetnopayload[0] = 0xA5;																					//octet de start
				packetnopayload[1] = command;																				//octet de commande
			
				while(Driver_USART0.GetStatus().tx_busy == 1); 											//attente buffer TX vide
				Driver_USART0.Send(packetnopayload,2); 															//envoi des données	
				break;
			
			case 2 : 
				packetpayload[0] = 0xA5;																						//octet de start
				packetpayload[1] = command;																					//octet de commande
				packetpayload[2] = payloadsize;																			//taille du payload
			
				for (i=0; i<payloadsize;i++){																				//on rempli les octets suivants avec ceux du payload
					packetpayload[3+i] = payload[i];																	//on rempli les octets suivants avec ceux du payload
				}
			
				packetpayload[3+payloadsize] = 0 ^ 0xA5 ^ command ^ payloadsize;		//calcul checksum
				for (j=0; j<payloadsize; j++){																			//calcul checksum
					packetpayload[3+payloadsize] ^= payload[j];												//calcul checksum
				}
			
				while(Driver_USART0.GetStatus().tx_busy == 1); 											//attente buffer TX vide
				Driver_USART0.Send(packetpayload,9); 																//envoi des données
				break;
		}
}		

//---------------------------------------------------------------------------------------------------------------------------------------//
//																												LIDAR START SCAN
//---------------------------------------------------------------------------------------------------------------------------------------//


void Lidar_Start_Scan(void){
	
	Send_Lidar_Command(0x20, 0x00, NULL);																			//start le scan avec la commande 0x20, payloadsize à 0x00 pour pas de payload
	
}

//---------------------------------------------------------------------------------------------------------------------------------------//
//																												LIDAR STOP SCAN
//---------------------------------------------------------------------------------------------------------------------------------------//


void Lidar_Stop_Scan(void){
	
	Send_Lidar_Command(0x25, 0x00, NULL);																			//stop le scan avec la commande 0x25, payloadsize à 0x00 pour pas de payload
	
}


//---------------------------------------------------------------------------------------------------------------------------------------//
//																												RTOS Recevoir de l'UART
//---------------------------------------------------------------------------------------------------------------------------------------//

void Receive_UART(void const * argument){

  char chain[20];
	while(1){

		osSignalWait(0x0001,osWaitForever);                                     //Attente de l'event 1 à 1
    osSignalClear(ID_Receive,0x0001);                                       //On remet l'event à 0
    Lidar_Start_Scan();                                                     //Start le scan
		
 
    Driver_USART0.Receive(ValeurRecue,5);															      //la fonction remplira jusqu'à 150 cases
    while (Driver_USART0.GetRxCount() <5);															 	  //on attend que 150 case soit pleine
    
		osSignalSet(ID_LCD, 0x0001);                                            //Réveil tâche afficher LCD

	}

}

//---------------------------------------------------------------------------------------------------------------------------------------//
//																												RTOS Afficher LCD & traitement données
//---------------------------------------------------------------------------------------------------------------------------------------//

void Afficher_LCD(void const * argument){

	unsigned short SetAngle = 0x0000;
	float SetAngle_f;
	unsigned short SetDistance = 0x0000;
	float SetDistance_f;
	
	char chain[30];                                                           //tableau pour afficher
	
	while(1){
		
		osSignalWait(0x0001,osWaitForever);                                     //Attente de l'event 1 à 1
    osSignalClear(ID_LCD,0x0001);                                           //On remet l'event 1 à 0
		
		SetAngle = (ValeurRecue[1] >> 1) + (short)(ValeurRecue[2] << 7);				//on récupère les données de l'angle
		SetAngle_f = (((float)SetAngle) / 64.0)*0.9;														//d'après la doc, valeur angle = donnée angle / 64 (en degrés), et a un pas de 0.9 (à vérifier)
		
		sprintf(chain,"angle = %04d",(short)SetAngle_f);                        //Affichage
		GLCD_DrawString(0,0,chain);                                             //Affichage
		
		SetDistance = (ValeurRecue[3]) + (short)(ValeurRecue[4] << 8);					//on récupère les données de la distance
		SetDistance_f = (((float)SetDistance) / 4.0) / 1000;										//d'après la doc, valeur distance = donnée distance / 4 (en mm)
		
		sprintf(chain,"distance = %06f",SetDistance_f);                         //Affichage
		GLCD_DrawString(0,30,chain);                                            //Affichage
		
		osSignalSet(ID_Receive, 0x0001);                                        //Réveil tâche receive
		
	}

}

