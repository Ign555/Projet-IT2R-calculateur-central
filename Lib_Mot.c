#include "LPC17xx.h"                    // Device header
#include "Lib_Mot.h"

float servo_duty = 0.5 ;

/*----------------------------------------------------------------------------
 * Fonction D'Init des Moteurs
 *---------------------------------------------------------------------------*/

void Mot_Initialize ( void ) {
	
	LPC_SC->PCONP = LPC_SC->PCONP | 0x00000040;   // enable PWM1
	LPC_PWM1->PR = 0;  // prescaler
	LPC_PWM1->MR0 = 1249;    // MR0+1=100   la période de la PWM vaut 1ms
	
	LPC_PWM1->MCR = LPC_PWM1->MCR | 0x00000002; // Compteur relancé quand MR0 repasse à 0
	LPC_PWM1->LER = LPC_PWM1->LER | 0x0000000F;  // ceci donne le droit de modifier dynamiquement la valeur du rapport cyclique
																						 // bit 0 = MR0    bit 1 MR1 bit2 MR2 bit3 = MR3
	LPC_PWM1->PCR = LPC_PWM1->PCR | 0x00000e00;  // autorise les sortie PWM1/2/3 bits 9, 10, 11
	LPC_PINCON->PINSEL7 = LPC_PINCON->PINSEL7| 0x000C0000;
	
}

/*----------------------------------------------------------------------------
 * Fonction D'Init du Servo Moteur
 *---------------------------------------------------------------------------*/

void Servo_Mot_Initialize ( void ) {
	
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

/*----------------------------------------------------------------------------
 * Modification Raport cyclique Moteur ( Vitesse )
 *---------------------------------------------------------------------------*/

void Mot_Set_Duty( float duty ) {
	
	LPC_PWM1->MR2 = (int)((float)(LPC_PWM1->MR0 + 1)*duty);

}

/*----------------------------------------------------------------------------
 * Modification Raport cyclique Servo ( Angle )
 *---------------------------------------------------------------------------*/

void Servo_Mot_Set_Duty( float duty ) {
	
	servo_duty = duty;

}

/*----------------------------------------------------------------------------
 * Fonction Gestion PWM Servo_Mot
 *---------------------------------------------------------------------------*/

void TIMER0_IRQHandler ( void ) {
	
	LPC_TIM0->IR = (1<<0); //baisse le drapeau dû à MR0
	
	if( (int) (__50HZ_FRQ_MR__*servo_duty) == LPC_TIM0->MR0 ){
		
		LPC_TIM0->MR0 = (int)(__50HZ_FRQ_MR__*(1-servo_duty));
		
	}
	
	else{
		
		LPC_TIM0->MR0 = (int)(__50HZ_FRQ_MR__*servo_duty);
		
	}
	
	LPC_TIM0->TCR = 1; 

}
