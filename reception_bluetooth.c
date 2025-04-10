#include "reception_bluetooth.h"

void Init_UART1(void (event_UART)(uint32_t event)){
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
void RB_init(void (event_UART)(uint32_t event)){ 
	Init_UART1(event_UART);
	NVIC_SetPriority(UART1_IRQn,2);
}
int RB_get_data(Manette *manette){
	
	uint8_t tab[9];
	
	Driver_USART1.Receive(tab,8);		// A mettre ds boucle pour recevoir 
	osSignalWait(0x01, osWaitForever);	// sommeil attente reception
	
	if(tab[0] == 's' && tab[7] == 'e'){
		manette->jx = tab[2];
		manette->jy = tab[4];
		manette->b = tab[6];
		return 0;
	}else{
		return -1;
	}
	
}