/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define DEBUG //Uncomment for debbuging 


#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "LPC17xx.h"                    		// Device header
#include "Driver_USART.h"               		// ::CMSIS Driver:USART
#include <stdlib.h>
#include "Lib_UART.h"
#include "pwm_moteur.h"
#include "dfplayermini.h"
#include "reception_bluetooth.h"

/********Debug include********/
#ifdef DEBUG
	#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD 
	#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#endif
//Font for debbuing
#ifdef DEBUG
	extern GLCD_FONT GLCD_Font_6x8;
	extern GLCD_FONT GLCD_Font_16x24;
#endif
/*----------------------------------------------------------------------------
 * Task ID & DEF - RTOS
 *---------------------------------------------------------------------------*/

void TaskReceptionBT (void const * argument );
void TaskRFID ( void const * argument );
void TaskDFPlayer ( void const * argument );
void TaskMoteur (void const * argument );
void TaskServoMoteur (void const * argument );

osThreadId ID_RFID, ID_DFPlayer, ID_MOTEUR, ID_SERVOMOTEUR, ID_RECEPTIONBT;

osThreadDef ( TaskRFID, osPriorityNormal, 1, 0);
osThreadDef ( TaskDFPlayer, osPriorityNormal, 1, 0);
osThreadDef ( TaskReceptionBT, osPriorityNormal, 1, 0);
osThreadDef ( TaskMoteur, osPriorityNormal, 1, 0);
osThreadDef ( TaskServoMoteur, osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
 * Event/CB function prototype
 *---------------------------------------------------------------------------*/
void RB_event(uint32_t event);


/*----------------------------------------------------------------------------
 * Task ID & DEF - MUTEX
 *---------------------------------------------------------------------------*/
/*
osMutexId ID_Mutex_PWM, ID_Mutex_UART;

osMutexDef ( Mutex_PWM );
osMutexDef ( Mutex_UART );
*/
/*----------------------------------------------------------------------------
 * Task ID & DEF - MAILS BOXS
 *---------------------------------------------------------------------------*/
/*
osMailQId ID_RFID2DFPlayer, ID_ETAT_MOT, ID_Lumiere2DFPlayer, ID_Manette2DFPlayer;

osMailQDef(RFID2DFPlayer, 1, uint8_t); // obj une boite au lettre la plus petite possible 
osMailQDef(ETAT_MOT, 1, uint8_t);
osMailQDef(Lumiere, 1, uint8_t);
osMailQDef(Manette, 1, uint8_t);
*/

/*----------------------------------------------------------------------------
 * main: initialize and start the system
 *---------------------------------------------------------------------------*/

int main (void) {
	
	osKernelInitialize ();
	
	
	/********Init for debbuging (it means you can comment it dummy)********/
	#ifdef DEBUG
		GLCD_Initialize();
		GLCD_ClearScreen();
		GLCD_SetFont(&GLCD_Font_16x24);
		GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
	#endif
	/********Normal Init********/
	
	//DF Player init
	DFPlayer_init();
	
	//Motor init
	init_servo_moteur();
	init_moteur();
	moteur_set_duty(0);
	moteur_set_direction(0);
	servo_moteur_set_duty(0.075);
	
	//Bluetooth Reception Init
	RB_init(RB_event);
	
	//Motor init process
	osDelay(5000);
	servo_moteur_set_duty(0.1);
	osDelay(5000);
	servo_moteur_set_duty(0.075);
	osDelay(5000);
	servo_moteur_set_duty(0.05);
	osDelay(5000);
	servo_moteur_set_duty(0.075);
	
	//DfPlayerInitProcess
	DFPlayer_set_volume(30);
	DFPlayer_play_in_folder(0x02, 0x02);
	
	
	//Init UART
	//Init_UART();
	
  
	
	ID_DFPlayer = osThreadCreate ( osThread ( TaskDFPlayer ), NULL);
	ID_RFID = osThreadCreate ( osThread ( TaskRFID ), NULL);
	ID_MOTEUR = osThreadCreate ( osThread ( TaskMoteur ), NULL);
	ID_SERVOMOTEUR = osThreadCreate ( osThread ( TaskServoMoteur ), NULL);
	ID_RECEPTIONBT = osThreadCreate ( osThread ( TaskReceptionBT ), NULL);
	
	//ID_Mutex_UART = osMutexCreate( osMutex( Mutex_UART));

  osKernelStart ();
	
	osDelay(osWaitForever);
	
	return 0;
}

/*----------------------------------------------------------------------------
 * Task - DFPlayer
 *---------------------------------------------------------------------------*/

void TaskDFPlayer ( void const * argument ){
	/*
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
	}*/
	while(1){
		
	}
}


 /*----------------------------------------------------------------------------
  * Task RFID
  *---------------------------------------------------------------------------*/

void TaskRFID ( void const * argument ){
	/*
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
	}*/
	while(1){
		
	}
}

/*----------------------------------------------------------------------------
 * Task MOTEUR
 *---------------------------------------------------------------------------*/

void TaskMoteur ( void const * argumsent ) {
	/*
	Servo_Mot_Initialize();
	Mot_Initialize();
	
	Mot_Set_Duty(0.9); */
	//Servo_Mot_Set_Duty(0.1);
	
	while (1){
		
	}
	
}
/*----------------------------------------------------------------------------
 * Task SERVOMOTEUR
 *---------------------------------------------------------------------------*/

void TaskServoMoteur ( void const * argumsent ) {
	/*
	Servo_Mot_Initialize();
	Mot_Initialize();
	
	Mot_Set_Duty(0.9); */
	//Servo_Mot_Set_Duty(0.1);
	
	while (1){
		
	}
	
}
/*----------------------------------------------------------------------------
 * Task RECEPTIONBT
 *---------------------------------------------------------------------------*/

void TaskReceptionBT (void const * argument ){
	
	
	char tab[9], texte[30];
	char jx, jy, b;
  while (1) {
		RB_get_data(&jx, &jy, &b);
		
		#ifdef DEBUG
			sprintf(texte,"Jx: %3d",jx);
			GLCD_DrawString(1,1,texte);
			sprintf(texte,"Jy: %3d",jy);
			GLCD_DrawString(1,100,texte);		
			sprintf(texte,"B: %3d",b);
			GLCD_DrawString(1,200,texte);		
		#endif
	}
	
}

/*----------------------------------------------------------------------------
 * UART Event function
 *---------------------------------------------------------------------------*/
void RB_event(uint32_t event){
	switch (event) {
		case ARM_USART_EVENT_RECEIVE_COMPLETE : 	osSignalSet(ID_RECEPTIONBT, 0x01);
																							break;
		default : break;
	}	
}