#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include <stdio.h>                   // Device header
#include "LPC17xx.h"                    // Device header
#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include "string.h" 
#include "GLCD_Config.h"
#include "Board_GLCD.h"
#include "SSP_LPC17xx.h"
#include "GLCD_Fonts.h"


char badge[14]={2,48,56,48,48,56,67,50,51,69,57,52,69,3};// les données du bon badge

extern ARM_DRIVER_USART Driver_USART2;

void Init_UART1(void){
	Driver_USART2.Initialize(NULL);
	Driver_USART2.PowerControl(ARM_POWER_FULL);
	Driver_USART2.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							9600);
	Driver_USART2.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART2.Control(ARM_USART_CONTROL_RX,1);

}	
char buff[16] = "";

// Fonction pour lire les données du badge
void lire()
	{	
	Driver_USART2.Receive(buff,14); // la fonction remplira jusqu'à 16 cases
	while (Driver_USART2.GetRxCount() <14 ) ; // on attend que 16 cases soient pleine	
	}

int main (void){

	char tab[20]; // tableau pour le badge qu'on lit
	Init_UART1();
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);

	while (1){
		// Vérification si le badge est correct
		lire();

		//affiche les données sur le lcd
		sprintf(tab,"rfid = %s",buff);	
		GLCD_DrawString(1,24,tab);
		
		if(strcmp(badge,buff)==0) // comparaison des deux tableaux
		{
		GLCD_DrawString(0, 2*24, "Bon badge"); // si les badges sont similaires
		}
		else
		{
		GLCD_DrawString(0, 2*24, "Mauvais badge");// si non
		}
	}	
	return 0;
}

