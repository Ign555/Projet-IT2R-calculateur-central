#include "Lib_UART.h"
#include "Driver_USART.h"               // ::CMSIS Driver:USART
#include <stdlib.h>

 /*----------------------------------------------------------------------------
  * Driver UART Extern
  *--------------------------------------------------------------------------- */

extern ARM_DRIVER_USART Driver_USART0;
extern ARM_DRIVER_USART Driver_USART1;
extern ARM_DRIVER_USART Driver_USART2;

/*----------------------------------------------------------------------------
 * Fonction D'Init des ports 0 - 1 - 2 de l' UART
 *--------------------------------------------------------------------------- */

void Init_UART ( void ) {
	
	Driver_USART0.Initialize(NULL);
	Driver_USART0.PowerControl(ARM_POWER_FULL);
	
	Driver_USART0.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							9600);
	
	Driver_USART0.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART0.Control(ARM_USART_CONTROL_RX,1);
	
	
	Driver_USART1.Initialize(NULL);
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	
	Driver_USART1.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							9600);
	
	Driver_USART1.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART1.Control(ARM_USART_CONTROL_RX,1);
	
	Driver_USART2.Initialize(NULL);
	Driver_USART2.PowerControl(ARM_POWER_FULL);
	
	Driver_USART2.Control(	ARM_USART_MODE_ASYNCHRONOUS |
							ARM_USART_DATA_BITS_8		|
							ARM_USART_STOP_BITS_1		|
							ARM_USART_PARITY_NONE		|
							ARM_USART_FLOW_CONTROL_NONE,
							19200);
	
	Driver_USART2.Control(ARM_USART_CONTROL_TX,1);
	Driver_USART2.Control(ARM_USART_CONTROL_RX,1);
	
}

/*----------------------------------------------------------------------------
 * Fonction liant de micro au DFPlayer
 *--------------------------------------------------------------------------- */

void LinkDFPlayer ( void ){
	
	unsigned char packet[10] = {0x7E,0xFF, 0x06, 0x3F, 0x00, 0x00, 0x00, 0xFE, 0xBC, 0xEF};
	
	while( 1 == Driver_USART0.GetStatus().tx_busy );
	Driver_USART0.Send(packet,10);
}

/*----------------------------------------------------------------------------
 * Fonction d'Envoie de Commande au DFPlayer
 *--------------------------------------------------------------------------- */

void Send_DFPlayer_Command ( unsigned char command, unsigned const char param1, unsigned const char param2 ){
	
	
	unsigned char chkb1; 																																		//checksum high
	unsigned char chkb2; 																																		//checksum low
	int checksum; 																																					//checksum pour calculer
	
  unsigned char packet[10]; 																															//trame à envoyer
	packet[0] = 0x7E; 																																			//start 
	packet[1] = 0xFF; 																																			//version
	packet[2] = 0x06; 																																			//taille de la data (toujours 06)
	packet[3] = command; 																																		//commande
	packet[4] = 0x00; 																																			//feedback : 00 = pas de feedback
	packet[5] = param1; 																																		//data high
	packet[6] = param2; 																																		//data low
	
	checksum = 0 - packet[1]- packet[2]- packet[3]- packet[4]- packet[5]- packet[6]; 				//calcul du checksum
	
	//Octet 1 et 2 de checksum
	chkb1 = (checksum >> 8) & 0xFF; 																												//r?partion du checksum en int sur 2 chars, ici le char high
	chkb2 = checksum & 0xFF; 																																//checksum partie low

	packet[7] = chkb1;
	packet[8] = chkb2;
	packet[9] = 0xEF; 																																			//bit de stop
  while( 1 == Driver_USART0.GetStatus().tx_busy ); 																					// attente buffer TX vide
	Driver_USART0.Send(packet,10); 																													//envoie du packet
	
}

/*----------------------------------------------------------------------------
 * Fonction de Delais
 *--------------------------------------------------------------------------- */

void tempo ( int ms ) {
	
	int j;
	for (j=0; j<(16667*ms);j++);

}


/*----------------------------------------------------------------------------
 * Fonction de Lecture du RFID avec Retour de Tableau
 *--------------------------------------------------------------------------- */

char *Read_RFID ( void ) {
	char *buff = (char *)malloc(14);
	
	Driver_USART1.Receive(buff,14); // la fonction remplira jusqu'à 16 cases
	while ( 14 < Driver_USART1.GetRxCount() ) ;
	
	return buff;
}
