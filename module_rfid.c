#include "module_rfid.h"

char bon_badge[14]={2,48,56,48,48,56,67,50,51,69,57,52,69,3};// les données du bon badge

void Init_UART2(void){
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

void RFID_init(){
		Init_UART2();
}
void RFID_read(char *buff){
	Driver_USART2.Receive(buff,14); // la fonction remplira jusqu'à 16 cases
	while (Driver_USART2.GetRxCount() <14 ) ; // on attend que 16 cases soient pleine	
}