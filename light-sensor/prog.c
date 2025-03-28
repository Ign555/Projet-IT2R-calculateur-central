#include "Board_GLCD.h"
#include "Board_ADC.h"
#include "GLCD_Config.h"
#include "stdio.h"  


extern GLCD_FONT GLCD_Font_6x8;
extern GLCD_FONT GLCD_Font_16x24;

int main (void)
{
	int AD_last;
	int status;
	char out[32];
	GLCD_Initialize();
	GLCD_SetFont(&GLCD_Font_16x24);
	GLCD_ClearScreen();
	ADC_Initialize ();
	
	while(1)
	{
		ADC_StartConversion();
		GLCD_DrawString(0, 24, "conversion en cours");
		while(ADC_ConversionDone());
		GLCD_DrawString(0, 24, "conversion finie      ");
		AD_last = ADC_GetValue();  
    sprintf(out, "ADC: %04d", AD_last); 
    GLCD_DrawString(0, 2*24, out);

	}
}
