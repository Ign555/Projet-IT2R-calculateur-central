#include <LPC17XX.h>
#include "Driver_USART.h"
#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
#include "GLCD_Config.h"                // Keil.MCB1700::Board Support:Graphic LCD
#include "stdio.h"

extern ARM_DRIVER_USART Driver_USART1;
extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

void Init_UART1(void){
	Driver_USART1.Initialize(NULL);
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

int main(void){
	
	
	
	char tab[50];
	char data_nunchuck[3];
	
	Init_UART1();
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);
	
	while (1){
		Driver_USART1.Receive(data_nunchuck,3);
		sprintf(tab,"Jx: %3d",data_nunchuck[0]);	// du blabla
		GLCD_DrawString(100,100,tab);	// colonne puis ligne en pixel
		sprintf(tab,"Jy: %3d",data_nunchuck[1]);	// du blabla
		GLCD_DrawString(100,150,tab);	// colonne puis ligne en pixel
		sprintf(tab,"Button: %3d",data_nunchuck[2]);	// du blabla
		GLCD_DrawString(100,200,tab);	// colonne puis ligne en pixel
	}
	return 0;
}