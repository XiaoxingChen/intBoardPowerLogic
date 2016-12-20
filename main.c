#include	"config.h"
#include	"timer.h"
#include	"GPIO.h"
#include "key_monitor.h"
#include "STC15Fxxxx.H"
#include "logic_out.h"
#include "power_manager.h"


/************************ Timer Config****************************/
void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					
	
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;
	TIM_InitStructure.TIM_Polity    = PolityLow;			
	TIM_InitStructure.TIM_Interrupt = ENABLE;				
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;
	TIM_InitStructure.TIM_ClkOut    = DISABLE;			
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (1000*12));
	TIM_InitStructure.TIM_Run       = ENABLE;				
	Timer_Inilize(Timer2,&TIM_InitStructure);				
}


void main(void)
{
	logic_pin_config();
	confirm_wake_type();
	Timer_config();
	key_init();
	EA = 1;
	launch_board();
	enable_battery();	
	
	while (1)
	{
		key_monitor_run();
		power_manager_run();
		
	}
}



