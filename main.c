              // Device header
#include "pwm_moteur.h"
#include "GPIO_LPC17xx.h"


int main(void)
{
	init_servo_moteur();
	init_moteur();
	
	servo_moteur_set_duty(0.075);
	moteur_set_duty(0.7);
	

	while(1)
{
moteur_set_direction(0);

}
return(0);

}

































/*void initTimer0(int prescaler, int MR)
{
LPC_SC->PCONP = LPC_SC->PCONP | 0x00C00006;
	
LPC_TIM0->PR = prescaler;      //pr�scaler
LPC_TIM0->MR0 = MR;            //MR
LPC_TIM0->MCR = LPC_TIM0->MCR | (3<<0); //remise � zero du compteur + interruption

LPC_TIM0->TCR =1;	// lancer le Timer

NVIC_SetPriority(	TIMER0_IRQn,0);      //mettre une priorit�
NVIC_EnableIRQ(TIMER0_IRQn);          // active les int�rruptions	

}
void TIMER0_IRQHandler(void)
{
LPC_TIM0->IR = (1<<0); //baisse le drapeau 
////////.............� completer..............................////////////	
}




void servomot(void)
{ //freq 50 Hz 
  //r�solution 8-10 bit
	//Pin 3.25
	
	
	
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7 | 0x4002 1010
   	

	
}*/
	
	