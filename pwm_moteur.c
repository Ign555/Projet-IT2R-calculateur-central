#include "pwm_moteur.h"

float servo_duty = 0.5;

void init_moteur(){
	LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040;   // enable PWM1
	LPC_PWM1->PR = 0;  // prescaler
	LPC_PWM1->MR0 = 999;    // MR0+1=100   la période de la PWM vaut 50ms 25000hz 1249
	
	LPC_PWM1->MCR = LPC_PWM1->MCR | 0x00000002; // Compteur relancé quand MR0 repasse à 0
	LPC_PWM1->LER = LPC_PWM1->LER | 0x0000000F;  // ceci donne le droit de modifier dynamiquement la valeur du rapport cyclique
																						 // bit 0 = MR0    bit 1 MR1 bit2 MR2 bit3 = MR3
	LPC_PWM1->PCR = LPC_PWM1->PCR | 0x00000e00;  // autorise les sortie PWM1/2/3 bits 9, 10, 11
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7| 0x000C0000;
	LPC_GPIO0->FIODIR2 = 0x03;
	LPC_GPIO0->FIOPIN2 &= 0xFC;	
	//LPC_GPIO0->FIODIR2 = 0x01;	
	
	LPC_PWM1->TCR = 1;  /*validation de timer  et reset counter */	
}	
void init_servo_moteur()
{
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7| 0x00200000;
	LPC_TIM0->EMR=0x00C0; 
	LPC_SC->PCONP = LPC_SC->PCONP | 0x00000002;   
	LPC_TIM0->PR = 4;  // le registre PR prend la valeur du prescaler
	LPC_TIM0->MR0 = (int)(__50HZ_FRQ_MR__*(1-servo_duty));
	LPC_TIM0->MCR=LPC_TIM0->MCR | 0x00000003;
	LPC_TIM0->TCR = 1;  
	NVIC_SetPriority(TIMER0_IRQn,0); // TIMER0 (IRQ1) : interruption de priorité 0
	NVIC_EnableIRQ(TIMER0_IRQn); // active les interruptions TIMER0
}
void moteur_set_duty(float duty){
	LPC_PWM1->MR2 = (int)((float)(LPC_PWM1->MR0 + 1)*duty);
}
void moteur_set_direction(char dir)
{
		
		if(dir==1){
			LPC_GPIO0->FIOPIN2 = 0x01;
			//LPC_GPIO0->FIOPIN2 &= 0xFD;	

		}else{
			LPC_GPIO0->FIOPIN2 = 0x02;
			//LPC_GPIO0->FIOPIN2 &= 0xFE;	
		}

}	



void servo_moteur_set_duty(float duty){
	servo_duty = duty;
}
void TIMER0_IRQHandler(void) // Fonction qui gère le pwm du servo moteur
{
	LPC_TIM0->IR = (1<<0); //baisse le drapeau dû à MR0
	if(LPC_TIM0->MR0 == (int)(__50HZ_FRQ_MR__*servo_duty)){
		LPC_TIM0->MR0 = (int)(__50HZ_FRQ_MR__*(1-servo_duty));
	}else{
		LPC_TIM0->MR0 = (int)(__50HZ_FRQ_MR__*servo_duty);
	}
	LPC_TIM0->TCR = 1; 

}
