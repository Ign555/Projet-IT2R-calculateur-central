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

osThreadDef ( TaskReadRFID, osPriorityNormal, 1, 0);
osThreadDef ( TaskDFPlayer, osPriorityNormal, 1, 0);
osThreadDef ( TaskReceptionBT, osPriorityNormal, 4, 0);
osThreadDef ( TaskMoteur, osPriorityNormal, 1, 0);

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
osMailQDef(rb_joystick, 50, JoystickPosition);
osMailQDef(sound2play, 8, uint8_t);

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
	
	uint8_t * Sound2Play, * Reception_Manette, * Reception_Moteur, * Reception_Lumiere;
	
	osEvent EV_RFID, EV_Manette, EV_Moteur, EV_Lumiere;
	uint8_t sound;
		
	while (1){

		EV_RFID = osMailGet(ID_SOUND2PLAY, osWaitForever);
		Sound2Play = EV_RFID.value.p;
		
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
	
	JoystickPosition *jp;
	osEvent mailJoystick;
	float motor_duty_cycle = 0;
	char tab[9], texte[30];
	
	while (1){
		
		mailJoystick = osMailGet(ID_RB_JOYSTICK, osWaitForever);
		jp = mailJoystick.value.p;
		
		//To much magic number...
		if(jp->y  < 144){
				moteur_set_direction(1);
				motor_duty_cycle = (1 - ((jp->y+111)/255.0));
		}else if(jp->y > 192){
				moteur_set_direction(0);
				motor_duty_cycle = 0.5*jp->y/255.0;
		}else{
				motor_duty_cycle = 0;
		}
		
		moteur_set_duty(motor_duty_cycle);
		
		servo_moteur_set_duty(0.075 + ((127 - (jp->x-15))/255.0)*0.025);
		
		osMailFree(ID_RB_JOYSTICK, jp);

	}
	
}
/*----------------------------------------------------------------------------
 * Task RECEPTIONBT - CB  ( peut etre penser à flush le buffer au démarrage ) 
 *---------------------------------------------------------------------------*/

void TaskReceptionBT (void const * argument ){
	
	//Sémaphore ou mutext à mettre
	JoystickPosition *jp, jp_prev;
	
	char tab[9], texte[30];
	uint8_t b;
	
  while (1) {
		
		#ifdef DEBUG
		
			osMutexWait(ID_mut_GLCD, osWaitForever);
			GLCD_DrawString(100,92,"bt");
			osMutexRelease(ID_mut_GLCD);
		
		#endif
		
		osSignalWait(0x01, osWaitForever);
		
		jp = osMailAlloc(ID_RB_JOYSTICK, 100);
		
		osMutexWait(ID_mut_UART, osWaitForever);
		if(RB_get_data(&jp->x, &jp->y, &b) < 0){
			//Anti sacades
			jp->x = jp_prev.x;
			jp->y = jp_prev.y;
		}
		osMutexRelease(ID_mut_UART);
		
		#ifdef DEBUG
		
			osMutexWait(ID_mut_GLCD, osWaitForever);
			sprintf(texte,"Jx: %3d Jy: %3d",jp->x, jp->y);
			GLCD_DrawString(1,1,texte);
			sprintf(texte,"B: %3d", b);
			GLCD_DrawString(1,32,texte);
			osMutexRelease(ID_mut_GLCD);
		
		#endif
		
		jp_prev.x = jp->x;
		jp_prev.y = jp->y;
		
		osMailPut(ID_RB_JOYSTICK, jp);
		
	}
	
}
/*----------------------------------------------------------------------------
 * Task READRFID - CB 
 *---------------------------------------------------------------------------*/

void TaskReadRFID (void const * argument ){
	
	//Sémaphore ou mutext à mettre
	uint8_t *son;
	char buff_badge[14];
	char badge_ouverture[14] = {2,48,56,48,48,56,67,50,51,69,57,52,69,3};
	char texte[30];
	
  while (1) {
		
		#ifdef DEBUG
		
			osMutexWait(ID_mut_GLCD, osWaitForever);
			GLCD_DrawString(1,92,"rfid");
			osMutexRelease(ID_mut_GLCD);
		
		#endif
		
		osSignalWait(0x02, osWaitForever);
		
		son = osMailAlloc(ID_SOUND2PLAY, 100);
		
		osMutexWait(ID_mut_UART, osWaitForever);
		RFID_read(buff_badge);
		osMutexRelease(ID_mut_UART);
		
		#ifdef DEBUG
		
			osMutexWait(ID_mut_GLCD, osWaitForever);
			sprintf(texte,"badge : %s",buff_badge);
			GLCD_DrawString(1,64,texte);
			osMutexRelease(ID_mut_GLCD);
		
		#endif
		
		if(strcmp(buff_badge, badge_ouverture)==0){
			*son = 1;
			osMailPut(ID_SOUND2PLAY, son);
		}else{
			*son = 0;
			osMailPut(ID_SOUND2PLAY, son);
		}
		
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