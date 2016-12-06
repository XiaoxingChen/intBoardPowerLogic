#include "logic_out.h"
#include "GPIO.h"

void logic_pin_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;		
	
	GPIO_InitStructure.Pin  = BATT_EN_PIN;		
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		
	GPIO_Inilize(BATT_EN_GPIO,&GPIO_InitStructure);	
	
	GPIO_InitStructure.Pin  = BRD_nEN_PIN;		
	GPIO_InitStructure.Mode = GPIO_OUT_OD;		
	GPIO_Inilize(BRD_nEN_GPIO,&GPIO_InitStructure);
	
	GPIO_InitStructure.Pin  = PC_EN_PIN;		
	GPIO_InitStructure.Mode = GPIO_OUT_OD;		
	GPIO_Inilize(PC_EN_GPIO, &GPIO_InitStructure);
}

//end of file
