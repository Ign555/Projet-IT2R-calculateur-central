 /*----------------------------------------------------------------------------
  * Define de fréquence Moteur
  *---------------------------------------------------------------------------*/
	
#define __50HZ_FRQ_MR__ 99999.0


 /*----------------------------------------------------------------------------
  * Fonction Init Moteur & Servo + Set Duty et Timer
  *---------------------------------------------------------------------------*/
 

void Mot_Initialize ( void );
void Servo_Mot_Initialize ( void );
void Mot_Set_Duty ( float duty );
void Servo_Mot_Set_Duty ( float duty );

void TIMER0_IRQHandler ( void );
