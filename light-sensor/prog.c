#include "Board_ADC.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "Board_LED.h"
#include "cmsis_os.h"

void light_Task(void *argument);
void humidity_Task(void *argument);

osThreadId ID_light, ID_humidity;

osThreadDef( light_Task, osPriorityNormal, 1, 0);
osThreadDef( humidity_Task, osPriorityNormal, 1, 0);

osMailQId ID_Light2Lumiere, ID_Humidiy2IHM;

osMailQDef(Ligh2Lumiere, 16, char);
osMailQDef(Humidity2IHM, 16, int);


int main(void) {
    HAL_Init();
    LED_Initialize();
    ADC_Initialize();
    osKernelInitialize();
	  ID_light = osThreadCreate ( osThread ( light_Task), NULL);
    ID_humidity = osThreadCreate ( osThread ( humidity_Task), NULL);
    osKernelStart();
}


void light_Task(void *argument) {
	 int * Envoie_l ;
	 int AD_last_l;
   LED_Initialize();
   ADC_Initialize ();
   while(1) {
	
	   ADC_StartConversion();
	   while(ADC_ConversionDone());
	   AD_last_l = ADC_GetValue(); 
		 
		 if (AD_last_l <= 2000){
			 Envoie_l = osMailAlloc(ID_Light2Lumiere, osWaitForever);
			 * Envoie_l = AD_last_l; // à modifier 
			 osMailPut(ID_Light2Lumiere, Envoie_l);		 
		 }
	}
}
void humidity_Task(void *argument) {
	 int * Envoie_h ;
	 int AD_last_h;
   LED_Initialize();
   ADC_Initialize ();
   while(1) {
		 
		 ADC_StartConversion();
		 while(ADC_ConversionDone());
		 AD_last_h = ADC_GetValue();
		 
		 if (AD_last_h <= 200){
	 	  	Envoie_h = osMailAlloc(ID_Humidiy2IHM, osWaitForever);
		   	* Envoie_h = AD_last_h; // à modifier 
		  	osMailPut(ID_Humidiy2IHM, Envoie_h);		 
		 }
	}
}
