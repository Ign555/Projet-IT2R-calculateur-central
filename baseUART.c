#include <stdio.h>                   // Device header
#include "LPC17xx.h"                    // Device header
#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"
#include "string.h" 
#include "GLCD_Config.h"
#include "Board_GLCD.h"
#include "SSP_LPC17xx.h"
#include "GLCD_Fonts.h"
#include "module_rfid.h"

extern char bon_badge[14];




char buff[16] = "";

// Fonction pour lire les données du badge


int main (void){

	char tab[20]; // tableau pour le badge qu'on lit
	RFID_init();
	GLCD_Initialize();
	GLCD_ClearScreen();
	GLCD_SetFont(&GLCD_Font_16x24);

	while (1){
		// Vérification si le badge est correct
		RFID_read(buff);
		
		//affiche les données sur le lcd
		sprintf(tab,"rfid = %s",buff);	
		GLCD_DrawString(1,24,tab);
		
		if(strcmp(bon_badge,buff)==0) // comparaison des deux tableaux
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

