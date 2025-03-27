#include "reception_bluetooth.h"

int RB_process_data(char *tab, uint8_t *jx, uint8_t *jy, uint8_t *b){
	
	if(tab[0] == 's' && tab[7] == 'e'){
		*jx = tab[2];
		*jy = tab[4];
		*b = tab[6];
		return 0;
	}else{
		return -1;
	}
	
}