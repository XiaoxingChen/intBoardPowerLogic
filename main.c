
/*------------------------------------------------------------------*/
/* --- STC MCU International Limited -------------------------------*/
/* --- STC 1T Series MCU RC Demo -----------------------------------*/
/* --- Mobile: (86)13922805190 -------------------------------------*/
/* --- Fax: 86-0513-55012956,55012947,55012969 ---------------------*/
/* --- Tel: 86-0513-55012928,55012929,55012966 ---------------------*/
/* --- Web: www.GXWMCU.com -----------------------------------------*/
/* --- QQ:  800003751 ----------------------------------------------*/
/* If you want to use the program or the program referenced in the  */
/* article, please specify in which data and procedures from STC    */
/*------------------------------------------------------------------*/

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
		
		
//		if(key_is_long_pressed(KEY_LAUNCH))
//		{
//			shutdown_board();
//			disable_battery();
//		}
//		else	if(PRESSED == key_get_data(KEY_LAUNCH))
//		{
//			launch_board();
//			enable_battery();
//		}
		
	}
}



