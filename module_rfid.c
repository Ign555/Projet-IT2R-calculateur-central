#include "module_rfid.h"

char bon_badge[14]={2,48,56,48,48,56,67,50,51,69,57,52,69,3};// les données du bon badge

void Init_UART2(void (event_RFID)(uint32_t event)){
	Driver_USART2.Initialize(event_RFID);
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
void _RFID_flush(){
		uint8_t dummy;
		while (Driver_USART2.GetStatus().rx_busy == 1) {
				Driver_USART2.Receive(&dummy, 1); // Lire et ignorer les données
		}
}
void RFID_init(void (event_RFID)(uint32_t event)){
		
		Init_UART2(event_RFID);
		NVIC_SetPriority(UART2_IRQn,0);
	
		//Flush RFID UART
		_RFID_flush();
		
}
void RFID_read(char *buff){
	
	int i = 0;
	osSignalWait(0x02, osWaitForever);
	//Driver_USART2.Receive(buff,64); // la fonction remplira jusqu'à 14 cases
	while (Driver_USART2.GetStatus().rx_busy == 1) {
				Driver_USART2.Receive(buff + i, 1); // Lire et ignorer les données
				i++;
		}
	_RFID_flush();
}