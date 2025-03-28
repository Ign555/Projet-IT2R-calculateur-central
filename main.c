// Utilisation Event UART en emission-reception

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions


#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "LPC17xx.h"                    // Device header
#include "cmsis_os.h"                   // CMSIS RTOS header file
#include "stdio.h"

#include "reception_bluetooth.h"

extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

void Thread_T (void const *argument);                             // thread function
osThreadId tid_Thread_T;                                          // thread id
osThreadDef (Thread_T, osPriorityNormal, 1, 0);                   // thread object

void Thread_R (void const *argument);                             // thread function
osThreadId tid_Thread_R;                                          // thread id
osThreadDef (Thread_R, osPriorityNormal, 1, 0);                   // thread object



//fonction de CB lancee si Event T ou R
void event_UART(uint32_t event)
{
	switch (event) {
		
		case ARM_USART_EVENT_RECEIVE_COMPLETE : 	osSignalSet(tid_Thread_R, 0x01);
																							break;
		
		case ARM_USART_EVENT_SEND_COMPLETE  : 	osSignalSet(tid_Thread_T, 0x01);
																							break;
		
		default : break;
	}
}


int main (void){
	
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	// initialize peripherals here
	RB_init(event_UART);
	
	
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	GLCD_SetForegroundColor(GLCD_COLOR_BLACK);
//	GLCD_DrawRectangle(x,100,5,5);
	
	tid_Thread_T = osThreadCreate (osThread(Thread_T), NULL);
	tid_Thread_R = osThreadCreate (osThread(Thread_R), NULL);
	
	osKernelStart ();                         // start thread execution 
	
	osDelay(osWaitForever);
	
	return 0;
}

void Thread_T (void const *argument) {

  while (1) {
		osSignalWait(0x01, osWaitForever);		// sommeil fin emission
		osDelay(1000);
  }
}


void Thread_R (void const *argument) {

	char tab[9], texte[30];
	char jx, jy, b;
  while (1) {
		RB_get_data(&jx, &jy, &b);
		sprintf(texte,"Jx: %3d",jx);
		GLCD_DrawString(1,1,texte);
		sprintf(texte,"Jy: %3d",jy);
		GLCD_DrawString(1,100,texte);		
		sprintf(texte,"B: %3d",b);
		GLCD_DrawString(1,200,texte);			
	}
}





