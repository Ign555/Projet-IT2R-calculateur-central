#include <LPC17XX.h>
#include "dfplayermini.h"

	void Temporisation_ms(int a){ //fonction de temporisation en ms
		int j;
		for (j=0; j<(16667*a);j++){
		}
	}

int main (void){

	DFPlayer_init();
	
	Temporisation_ms(50);
	DFPlayer_set_volume(30);
	DFPlayer_send_command(0x0F,0x02,0x02);
	
	//DFPlayer_next();

	while (1){

	}	
	return 0;
}
