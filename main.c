// Utilisation Event UART en emission-reception

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions

#include "Driver_USART.h"               // ::CMSIS Driver:USART
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

extern ARM_DRIVER_USART Driver_USART1;

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

void Init_UART(void){
	Driver_USART1.Initialize(event_UART);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							115200);
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
}

int main (void){
	
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	// initialize peripherals here
	Init_UART();
	NVIC_SetPriority(UART1_IRQn,2);
	
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
		Driver_USART1.Receive(tab,8);		// A mettre ds boucle pour recevoir 
		osSignalWait(0x01, osWaitForever);	// sommeil attente reception
		
		if(tab[0] == 's' && tab[7] == 'e'){
		sprintf(texte,"Jx: %3d",tab[2]);
		GLCD_DrawString(1,1,texte);
		sprintf(texte,"Jy: %3d",tab[4]);
		GLCD_DrawString(1,100,texte);		
		sprintf(texte,"B: %3d",tab[6]);
		GLCD_DrawString(1,200,texte);			
		}
	}
}





