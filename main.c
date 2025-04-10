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
#include "module_rfid.h"
#include <string.h>

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
void TaskReadRFID ( void const * argument );
void TaskDFPlayer ( void const * argument );
void TaskMoteur (void const * argument );

osThreadId ID_RFID, ID_DFPlayer, ID_MOTEUR, ID_RECEPTIONBT;

osThreadDef ( TaskReadRFID, osPriorityNormal, 10, 0);
osThreadDef ( TaskDFPlayer, osPriorityNormal, 1, 0);
osThreadDef ( TaskReceptionBT, osPriorityNormal, 4, 0);
osThreadDef ( TaskMoteur, osPriorityAboveNormal, 1, 0);

/*----------------------------------------------------------------------------
 * Event/CB function prototype
 *---------------------------------------------------------------------------*/
void RB_event(uint32_t event);
void RFID_event(uint32_t event);

/*----------------------------------------------------------------------------
 * Task ID & DEF - MUTEX
 *---------------------------------------------------------------------------*/
#ifdef DEBUG
	osMutexId ID_mut_GLCD;
	osMutexDef ( mut_GLCD );
#endif


osMutexId /*ID_Mutex_PWM,*/ ID_mut_UART;
/*
osMutexDef ( Mutex_PWM );
*/
osMutexDef ( mut_UART );

/*----------------------------------------------------------------------------
 * Task ID & DEF - MAILS BOXS
 *---------------------------------------------------------------------------*/

osMailQId ID_SOUND2PLAY, ID_ETAT_MOT, ID_Lumiere2DFPlayer, ID_RB_JOYSTICK, ID_RB_BUTTON;
/*
osMailQDef(RFID2DFPlayer, 1, uint8_t); // obj une boite au lettre la plus petite possible 
osMailQDef(ETAT_MOT, 1, uint8_t);
osMailQDef(Lumiere, 1, uint8_t);
*/
osMailQDef(rb_joystick, 8, Manette);
osMailQDef(sound2play, 50, uint8_t);

/*----------------------------------------------------------------------------
 * main: initialize and start the system
 *---------------------------------------------------------------------------*/
 
int main (void) {
	
	osKernelInitialize ();
	
	
	/********Init for debbuging********/
	#ifdef DEBUG
		GLCD_Initialize();
		GLCD_ClearScreen();
		GLCD_SetFont(&GLCD_Font_16x24);
		GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
	#endif
	/********Normal Init********/
	
	//DF Player init
	DFPlayer_init();

	//Init Bluetooth Reception
	RB_init(RB_event);
	
	//Init RFID
	RFID_init(RFID_event);
	
	//Motor init
	init_servo_moteur();
	init_moteur();
	moteur_set_direction(0);
	servo_moteur_set_duty(0.075);
	
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
	
	//Create task
	ID_DFPlayer = osThreadCreate ( osThread ( TaskDFPlayer ), NULL);
	ID_RFID = osThreadCreate ( osThread ( TaskReadRFID ), NULL);
	ID_MOTEUR = osThreadCreate ( osThread ( TaskMoteur ), NULL);
	ID_RECEPTIONBT = osThreadCreate ( osThread ( TaskReceptionBT ), NULL);
	
	//Create mutex
	ID_mut_GLCD = osMutexCreate( osMutex( mut_GLCD ));
	ID_mut_UART = osMutexCreate( osMutex( mut_UART ));
	
	//Create Mail Boxes
	ID_RB_JOYSTICK = osMailCreate(osMailQ(rb_joystick), NULL);
	ID_SOUND2PLAY = osMailCreate(osMailQ(sound2play), NULL);
	
  osKernelStart ();
	
	osDelay(osWaitForever);
	
	return 0;
}

/*----------------------------------------------------------------------------
 * Task - DFPlayer
 *---------------------------------------------------------------------------*/

void TaskDFPlayer ( void const * argument ){
	
	osEvent EV_SON;
	uint8_t *Sound2Play;
		
	while (1){

		EV_SON = osMailGet(ID_SOUND2PLAY, osWaitForever);
		Sound2Play = EV_SON.value.p;
		
		switch(*Sound2Play){
			case 1:
				DFPlayer_play_in_folder(0x02, 0x03);
					break;
			
		}
		
		#ifdef DEBUG
		
			osMutexWait(ID_mut_GLCD, osWaitForever);
			GLCD_DrawString(1,128,"son");
			osMutexRelease(ID_mut_GLCD);
		
		#endif
		
		/*
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
		}*/
		osMailFree(ID_SOUND2PLAY, Sound2Play);
	}
}

/*----------------------------------------------------------------------------
 * Task MOTEUR
 *---------------------------------------------------------------------------*/

void TaskMoteur ( void const * argumsent ) {
	
	Manette *manette;
	osEvent mailJoystick;
	float motor_duty_cycle = 0;
	char texte[30];
	
	while (1){
		
		mailJoystick = osMailGet(ID_RB_JOYSTICK, osWaitForever);
		manette = mailJoystick.value.p;
		
		//To much magic number...
		if(manette->jy  < 144){
				moteur_set_direction(1);
				motor_duty_cycle = (1 - ((manette->jy+111)/255.0));
		}else if(manette->jy > 192){
				moteur_set_direction(0);
				motor_duty_cycle = 2*(manette->jy-192)/255.0;
		}else{
				motor_duty_cycle = 0;
		}
		
		moteur_set_duty(motor_duty_cycle);
		
		servo_moteur_set_duty(0.075 + ((127 - (manette->jx-15))/255.0)*0.025);
		
		osMailFree(ID_RB_JOYSTICK, manette);

	}
	
}
/*----------------------------------------------------------------------------
 * Task RECEPTIONBT - CB  ( peut etre penser à flush le buffer au démarrage ) 
 *---------------------------------------------------------------------------*/

void TaskReceptionBT (void const * argument ){
	
	Manette *manette, manette_prev;
	uint8_t *son;
	
	char texte[30];

  while (1) {
		
		#ifdef DEBUG
		
			osMutexWait(ID_mut_GLCD, osWaitForever);
			GLCD_DrawString(100,92,"bt");
			osMutexRelease(ID_mut_GLCD);
		
		#endif
		
		//Lecture continue pour limiter la latence ??
		//osSignalWait(0x01, osWaitForever);
		
		manette = osMailAlloc(ID_RB_JOYSTICK, 100);
		son = osMailAlloc(ID_SOUND2PLAY, 100);
		
		
		//Acquisition
		if(RB_get_data(manette) < 0){ //Si l'acquisition des données s'est mal passé
			//Anti sacades
			manette->jx = manette_prev.jx;
			manette->jy = manette_prev.jy;
			manette->b = manette_prev.b;
		}
		
		//Affichage pour debug
		#ifdef DEBUG
		
			osMutexWait(ID_mut_GLCD, osWaitForever);
			sprintf(texte,"Jx: %3d Jy: %3d",manette->jx, manette->jy);
			GLCD_DrawString(1,1,texte);
			sprintf(texte,"B: %3d", manette->b);
			GLCD_DrawString(1,32,texte);
			osMutexRelease(ID_mut_GLCD);
		
		#endif
		
		//Traitement
		
		//Son
		if(manette->b == 0x02 && manette_prev.b != manette->b){
				*son = 1;				
		}else{
				*son = 0;
		}
		
		//On sauvegarde la position précédente pour éviter les sacades 
		manette_prev.jx = manette->jx;
		manette_prev.jy = manette->jy;
		manette_prev.b = manette->b;
		
		
		//Envoie du son à jouer
		if(*son == 1){
			osMailPut(ID_SOUND2PLAY, son);
		}else{
			osMailFree(ID_SOUND2PLAY, son);
		}
		
		//Envoie coordonnée du joystick
		osMailPut(ID_RB_JOYSTICK, manette);

		//osSignalClear(ID_RECEPTIONBT, 0x01);
	}
	
}
/*----------------------------------------------------------------------------
 * Task READRFID - CB 
 *---------------------------------------------------------------------------*/

void TaskReadRFID (void const * argument ){
	
	//Sémaphore ou mutext à mettre
	uint8_t *son;
	char buff_badge[65];
	char badge_ouverture[15] = {2,48,56,48,48,56,67,50,51,69,57,52,69,3, '\0'};
	char texte[30];
	
  while (1) {
		
		#ifdef DEBUG
		
			osMutexWait(ID_mut_GLCD, osWaitForever);
			GLCD_DrawString(1,92,"rfid");
			osMutexRelease(ID_mut_GLCD);
		
		#endif
		
		son = osMailAlloc(ID_SOUND2PLAY, osWaitForever);
		
		RFID_read(buff_badge);
		
		#ifdef DEBUG
		
			osMutexWait(ID_mut_GLCD, osWaitForever);
			sprintf(texte,"b:%s",buff_badge);
			GLCD_DrawString(0,64,texte);
			osMutexRelease(ID_mut_GLCD);
		
		#endif
		
		if(strcmp(buff_badge, badge_ouverture)==0){
			*son = 1;
			osMailPut(ID_SOUND2PLAY, son);
		}else{
			osMailFree(ID_SOUND2PLAY, son);
		}
		
		osSignalClear(ID_RFID, 0x02);
		
	}
	
}
/*----------------------------------------------------------------------------
 * UART Event function
 *---------------------------------------------------------------------------*/
void RB_event(uint32_t event){
	
//	if(event & ARM_USART_EVENT_RECEIVE_COMPLETE){ //Pourquoi les if marche pas bordel ???!?!?
			osSignalSet(ID_RECEPTIONBT, 0x01);
//	}

}
void RFID_event(uint32_t event){
	
//	if(event & ARM_USART_EVENT_RECEIVE_COMPLETE){
			osSignalSet(ID_RFID, 0x02);
//	}
	
}