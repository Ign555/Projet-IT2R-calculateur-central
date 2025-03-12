
#include <LPC17xx.h>                    // Device header
#include "pwm_moteur.h"


int main(void)
{
	init_servo_moteur();
	init_moteur();
	
	moteur_set_duty(0.9);
	LPC_PWM1->TCR = 1;  /*validation de timer  et reset counter */	
	servo_moteur_set_duty(0.1);
while(1)
{
}
return(0);
}













































/*void initTimer0(int prescaler, int MR)
{
LPC_SC->PCONP = LPC_SC->PCONP | 0x00C00006;
	
LPC_TIM0->PR = prescaler;      //préscaler
LPC_TIM0->MR0 = MR;            //MR
LPC_TIM0->MCR = LPC_TIM0->MCR | (3<<0); //remise à zero du compteur + interruption

LPC_TIM0->TCR =1;	// lancer le Timer

NVIC_SetPriority(	TIMER0_IRQn,0);      //mettre une priorité
NVIC_EnableIRQ(TIMER0_IRQn);          // active les intérruptions	

}
void TIMER0_IRQHandler(void)
{
LPC_TIM0->IR = (1<<0); //baisse le drapeau 
////////.............à completer..............................////////////	
}




void servomot(void)
{ //freq 50 Hz 
  //résolution 8-10 bit
	//Pin 3.25
	
	
	
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7 | 0x4002 1010
   	

	
}*/
	
	