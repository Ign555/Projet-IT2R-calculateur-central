#include <LPC17xx.h>                    // Device header

#ifndef PWM_MOTEUR
#define PWM_MOTEUR

#define __50HZ_FRQ_MR__ 99999.0

//void TIMER1_IRQHandler(void);

void init_servo_moteur();
void init_moteur();

void moteur_set_duty(float duty);
void servo_moteur_set_duty(float duty);

void TIMER0_IRQHandler(void);

#endif

/*****************************************************************
*
*10% -> Gauche
*7.5% -> Milieu
*5% -> Droite
*
*
******************************************************************/