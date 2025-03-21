/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "LPC17xx.h"                    		// Device header
#include "Driver_USART.h"               		// ::CMSIS Driver:USART
#include <stdlib.h>

float servo_duty;

#define PLAY 											(0x0D)		// Play
#define PAUSE 										(0x0E)		// Commande pour mettre en pause la piste Audio
#define SPECIFY_VOLUME 						(0x06)		// Commande pour s?l?ctioner un volume sp?cifique
#define REPEAT_PLAY 							(0x11)		// Jouer en boucle ou ne plus jouer en boucle une piste audio

#define __50HZ_FRQ_MR__ 99999.0

extern ARM_DRIVER_USART Driver_USART0;
extern ARM_DRIVER_USART Driver_USART1;
extern ARM_DRIVER_USART Driver_USART2;

void Init_UART ( void );

void LinkDFPlayer ( void );
void Send_DFPlayer_Command ( unsigned char command, unsigned const char param1, unsigned const char param2 );

void tempo ( int ms );

void TaskRFID ( void const * argument );
void TaskDFPlayer ( void const * argument );
void TaskMoteur (void const * argument );

char *Read_RFID ( void );

void Mot_Initialize ( void );
void Servo_Mot_Initialize ( void );
void Mot_Set_Duty ( float duty );
void Servo_Mot_Set_Duty ( float duty );
void TIMER0_IRQHandler ( void );


osThreadId ID_RFID, ID_DFPlayer, ID_MOTEUR;

osThreadDef ( TaskRFID, osPriorityNormal, 1, 0);
osThreadDef ( TaskDFPlayer, osPriorityNormal, 1, 0);
osThreadDef ( TaskMoteur, osPriorityNormal, 1, 0);

osMutexId ID_Mutex_PWM, ID_Mutex_UART;

osMutexDef ( Mutex_PWM );
osMutexDef ( Mutex_UART );

osMailQId ID_RFID2DFPlayer, ID_ETAT_MOT, ID_Lumiere2DFPlayer, ID_Manette2DFPlayer;

osMailQDef(RFID2DFPlayer, 1, uint8_t); // obj une boite au lettre la plus petite possible 
osMailQDef(ETAT_MOT, 1, uint8_t);
osMailQDef(Lumiere, 1, uint8_t);
osMailQDef(Manette, 1, uint8_t);


/*
 * main: initialize and start the system
 */
int main (void) {
	
	Init_UART();
	
  osKernelInitialize ();
	
	ID_DFPlayer = osThreadCreate ( osThread ( TaskDFPlayer ), NULL);
	ID_RFID = osThreadCreate ( osThread ( TaskRFID ), NULL);
	ID_MOTEUR = osThreadCreate ( osThread ( TaskMoteur ), NULL);
	
	ID_Mutex_UART = osMutexCreate( osMutex( Mutex_UART));

  osKernelStart ();
	
	osDelay(osWaitForever);
	
	return 0;
}

void TaskDFPlayer ( void const * argument ){
	
	uint8_t * Reception_RFID, * Reception_Manette, * Reception_Moteur, * Reception_Lumiere;
	
	osEvent EV_RFID, EV_Manette, EV_Moteur, EV_Lumiere;
	
	uint8_t RFID, Manette, Moteur, Lumiere;
	
	LinkDFPlayer();
	
	while (1){
		EV_RFID = osMailGet(ID_RFID2DFPlayer, 10);
		Reception_RFID = EV_RFID.value.p;
		RFID = * Reception_RFID;
		osMailFree(ID_ETAT_MOT, Reception_RFID);
		
		EV_Moteur = osMailGet(ID_ETAT_MOT, 10);
		Reception_Moteur = EV_Moteur.value.p;
		Moteur = * Reception_Moteur;
		osMailFree(ID_ETAT_MOT, Reception_Moteur);
		
		EV_Manette = osMailGet(ID_Manette2DFPlayer, 10);
		Reception_Manette = EV_Manette.value.p;
		Manette = * Reception_Manette;
		osMailFree(ID_ETAT_MOT, Reception_Manette);
		
		EV_Lumiere = osMailGet(ID_Lumiere2DFPlayer, 10);
		Reception_Lumiere = EV_Lumiere.value.p;
		Lumiere = * Reception_Lumiere;
		osMailFree(ID_ETAT_MOT, Reception_Lumiere);
		
		if ( 1 == RFID ){
			osMutexWait(ID_Mutex_UART, osWaitForever);
			Send_DFPlayer_Command(0x03, 0x01, 0x01);
			tempo(5);
			osMutexRelease(ID_Mutex_UART);
		}
		
		if ( 1 == Manette ){
			osMutexWait(ID_Mutex_UART, osWaitForever);
			Send_DFPlayer_Command( 0x03, 0x01, 0x02);
			tempo(5);
			Send_DFPlayer_Command( REPEAT_PLAY, 0x00, 0x00);
			osMutexRelease(ID_Mutex_UART);
		}
		
		if ( 1 == Moteur) {
			osMutexWait(ID_Mutex_UART, osWaitForever);
			Send_DFPlayer_Command( 0x03, 0x01, 0x03);
			tempo(5);
			osMutexRelease(ID_Mutex_UART);
		}
		
		if ( 1 == Lumiere ) {
			osMutexWait(ID_Mutex_UART, osWaitForever);
			Send_DFPlayer_Command( 0x03, 0x01, 0x04);
			tempo(5);
			osMutexRelease(ID_Mutex_UART);
		}
		
		if ( 2 == Manette ) {
			osMutexWait(ID_Mutex_UART, osWaitForever);
			Send_DFPlayer_Command( PAUSE, 0x00, 0x00);
			tempo(5);
			osMutexRelease(ID_Mutex_UART);
		}
	}
}

void TaskRFID ( void const * argument ){
	
	char BADGE[14] = {2,48,56,48,48,56,67,50,51,69,57,52,69,3};
	
	uint8_t * Reception ;
	uint8_t * Envoie ;
	
	uint8_t ETAT_MOT;
	
	osEvent EV_Reception;
	
	char * Lecture;
	
	while (1){
		
		osMutexWait(ID_Mutex_UART, osWaitForever);
		Lecture = Read_RFID();
		osMutexRelease(ID_Mutex_UART);
		
		EV_Reception = osMailGet(ID_ETAT_MOT, 10);
		Reception = EV_Reception.value.p;
		ETAT_MOT = * Reception;
		osMailFree(ID_ETAT_MOT, Reception);
		
		
		
		if( 0 == strcmp(BADGE,Lecture)){
			
			Envoie = osMailAlloc(ID_RFID2DFPlayer, osWaitForever);
			* Envoie = 1;
			osMailPut(ID_RFID2DFPlayer, Envoie);
			
			osSignalWait( 0x0001, osWaitForever);
		}
		
		if ( 1 == ETAT_MOT ){
			
			ETAT_MOT = 0;
			osSignalWait( 0x0001, osWaitForever);
			
		}		
	}
}

void TaskMoteur ( void const * argument ) {
	
	Servo_Mot_Initialize();
	Mot_Initialize();
	
	Mot_Set_Duty(0.9);
	LPC_PWM1->TCR = 1;  /*validation de timer  et reset counter */	
	Servo_Mot_Set_Duty(0.1);
	
	while (1);
	
}

void Init_UART ( void ) {
	
	Driver_USART0.Initialize(NULL);
	Driver_USART0.PowerControl(ARM_POWER_FULL);
	
	Driver_USART0.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							9600);
	
	Driver_USART0.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART0.Control(ARM_USART_CONTROL_RX,1);
	
	
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
	
	Driver_USART2.Initialize(NULL);
	Driver_USART2.PowerControl(ARM_POWER_FULL);
	
	Driver_USART2.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							19200);
	
	Driver_USART2.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART2.Control(ARM_USART_CONTROL_RX,1);
	
}

void LinkDFPlayer ( void ){
	
	unsigned char packet[10] = {0x7E,0xFF, 0x06, 0x3F, 0x00, 0x00, 0x00, 0xFE, 0xBC, 0xEF};
	
	while( 1 == Driver_USART0.GetStatus().tx_busy );
	Driver_USART0.Send(packet,10);
}

void Send_DFPlayer_Command ( unsigned char command, unsigned const char param1, unsigned const char param2 ){
	
	
	unsigned char chkb1; 																																		//checksum high
	unsigned char chkb2; 																																		//checksum low
	int checksum; 																																					//checksum pour calculer
	
  unsigned char packet[10]; 																															//trame à envoyer
	packet[0] = 0x7E; 																																			//start 
	packet[1] = 0xFF; 																																			//version
	packet[2] = 0x06; 																																			//taille de la data (toujours 06)
	packet[3] = command; 																																		//commande
	packet[4] = 0x00; 																																			//feedback : 00 = pas de feedback
	packet[5] = param1; 																																		//data high
	packet[6] = param2; 																																		//data low
	
	checksum = 0 - packet[1]- packet[2]- packet[3]- packet[4]- packet[5]- packet[6]; 				//calcul du checksum
	
	//Octet 1 et 2 de checksum
	chkb1 = (checksum >> 8) & 0xFF; 																												//r?partion du checksum en int sur 2 chars, ici le char high
	chkb2 = checksum & 0xFF; 																																//checksum partie low

	packet[7] = chkb1;
	packet[8] = chkb2;
	packet[9] = 0xEF; 																																			//bit de stop
  while( 1 == Driver_USART0.GetStatus().tx_busy ); 																					// attente buffer TX vide
	Driver_USART0.Send(packet,10); 																													//envoie du packet
	
}

void tempo ( int ms ) {
	
	int j;
	for (j=0; j<(16667*ms);j++);

}

char *Read_RFID ( void ) {
	char *buff = (char *)malloc(14);
	
	Driver_USART1.Receive(buff,14); // la fonction remplira jusqu'à 16 cases
	while ( 14 < Driver_USART1.GetRxCount() ) ;
	
	return buff;
}

void Mot_Initialize ( void ) {
	
	LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040;   // enable PWM1
	LPC_PWM1->PR = 0;  // prescaler
	LPC_PWM1->MR0 = 1249;    // MR0+1=100   la période de la PWM vaut 1ms
	
	LPC_PWM1->MCR = LPC_PWM1->MCR | 0x00000002; // Compteur relancé quand MR0 repasse à 0
	LPC_PWM1->LER = LPC_PWM1->LER | 0x0000000F;  // ceci donne le droit de modifier dynamiquement la valeur du rapport cyclique
																						 // bit 0 = MR0    bit 1 MR1 bit2 MR2 bit3 = MR3
	LPC_PWM1->PCR = LPC_PWM1->PCR | 0x00000e00;  // autorise les sortie PWM1/2/3 bits 9, 10, 11
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7| 0x000C0000;
	
}

void Servo_Mot_Initialize ( void ) {
	
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7| 0x00200000;
	LPC_TIM0->EMR=0x00C0; 
	LPC_SC->PCONP = LPC_SC->PCONP | 0x00000002;   
	LPC_TIM0->PR = 4;  // le registre PR prend la valeur du prescaler
	LPC_TIM0->MR0 = (int)(__50HZ_FRQ_MR__*(1-servo_duty));
	LPC_TIM0->MCR=LPC_TIM0->MCR | 0x00000003;
	LPC_TIM0->TCR = 1;  
	NVIC_SetPriority(TIMER0_IRQn,0); // TIMER0 (IRQ1) : interruption de priorité 0
	NVIC_EnableIRQ(TIMER0_IRQn); // active les interruptions TIMER0
	
}

void Mot_Set_Duty( float duty ) {
	
	LPC_PWM1->MR2 = (int)((float)(LPC_PWM1->MR0 + 1)*duty);

}

void Servo_Mot_Set_Duty( float duty ) {
	
	servo_duty = duty;

}

void TIMER0_IRQHandler ( void ) { // Fonction qui gère le pwm du servo moteur
	
	LPC_TIM0->IR = (1<<0); //baisse le drapeau dû à MR0
	
	if( (int) (__50HZ_FRQ_MR__*servo_duty) == LPC_TIM0->MR0 ){
		
		LPC_TIM0->MR0 = (int)(__50HZ_FRQ_MR__*(1-servo_duty));
		
	}
	
	else{
		
		LPC_TIM0->MR0 = (int)(__50HZ_FRQ_MR__*servo_duty);
		
	}
	
	LPC_TIM0->TCR = 1; 

}
