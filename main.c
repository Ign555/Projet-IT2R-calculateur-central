/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "LPC17xx.h"                    		// Device header
#include "Driver_USART.h"               		// ::CMSIS Driver:USART
#include <stdlib.h>


#define PLAY 											(0x0D)		// Play
#define PAUSE 										(0x0E)		// Commande pour mettre en pause la piste Audio
#define SPECIFY_VOLUME 						(0x06)		// Commande pour s?l?ctioner un volume sp?cifique
#define REPEAT_PLAY 							(0x11)		// Jouer en boucle ou ne plus jouer en boucle une piste audio


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
		
		if ( RFID == 1 ){
			osMutexWait(ID_Mutex_UART, osWaitForever);
			Send_DFPlayer_Command(0x03, 0x01, 0x01);
			tempo(5);
			osMutexRelease(ID_Mutex_UART);
		}
		
		if ( Manette == 1 ){
			osMutexWait(ID_Mutex_UART, osWaitForever);
			Send_DFPlayer_Command( 0x03, 0x01, 0x02);
			tempo(5);
			Send_DFPlayer_Command( REPEAT_PLAY, 0x00, 0x00);
			osMutexRelease(ID_Mutex_UART);
		}
		
		if ( Moteur == 1) {
			osMutexWait(ID_Mutex_UART, osWaitForever);
			Send_DFPlayer_Command( 0x03, 0x01, 0x03);
			tempo(5);
			osMutexRelease(ID_Mutex_UART);
		}
		
		if ( Lumiere == 1 ) {
			osMutexWait(ID_Mutex_UART, osWaitForever);
			Send_DFPlayer_Command( 0x03, 0x01, 0x04);
			tempo(5);
			osMutexRelease(ID_Mutex_UART);
		}
		
		if ( Manette == 2) {
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
		
		
		
		if(strcmp(BADGE,Lecture)==0){
			
			Envoie = osMailAlloc(ID_RFID2DFPlayer, osWaitForever);
			* Envoie = 1;
			osMailPut(ID_RFID2DFPlayer, Envoie);
			
			osSignalWait( 0x0001, osWaitForever);
		}
		
		if ( ETAT_MOT == 1 ){
			
			ETAT_MOT = 0;
			osSignalWait( 0x0001, osWaitForever);
			
		}		
	}
}

void TaskMoteur ( void const * argument ) {
	
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
	
	while(Driver_USART0.GetStatus().tx_busy == 1);
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
  while(Driver_USART0.GetStatus().tx_busy == 1); 																					// attente buffer TX vide
	Driver_USART0.Send(packet,10); 																													//envoie du packet
	
}

void tempo ( int ms ) {
	
	int j;
	for (j=0; j<(16667*ms);j++);

}

char *Read_RFID ( void ) {
	char *buff = (char *)malloc(14);
	
	Driver_USART1.Receive(buff,14); // la fonction remplira jusqu'à 16 cases
	while (Driver_USART1.GetRxCount() <14 ) ;
	
	return buff;
}
