#include <LPC17XX.h>
#include "dfplayermini.h"

	void Temporisation_ms(int a){ //fonction de temporisation en ms
		int j;
		for (j=0; j<(16667*a);j++){
		}
	}

int main (void){

	DFPlayer_init();
	
	DFPlayer_send_command(0x3F,0x00,0x00); //Initialise le DFPlayer
	Temporisation_ms(1000);
	DFPlayer_send_command(0x03,0x01,0x01); //Joue un son mais ça marche pas, à revoir
	Temporisation_ms(5000);
	DFPlayer_send_command(0x01,0x00,0x00); //Next
	Temporisation_ms(5000);
	DFPlayer_send_command(0x01,0x00,0x00); //Next
	
	

	while (1){

	}	
	return 0;
}
