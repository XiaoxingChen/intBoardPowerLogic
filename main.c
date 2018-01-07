#include	"config.h"
#include	"timer.h"
#include	"GPIO.h"
#include "key_monitor.h"
#include "STC15Fxxxx.H"
#include "logic_out.h"
#include "power_manager.h"
#include "heart_led.h"
#include "watchdog.h"
#include "logger.h"

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
	Timer_Inilize(Timer0,&TIM_InitStructure);				
}

void UART_init(void)//115200bps, 22.1184MHz
{		  
	SCON = 0x50;		//8bit
	AUXR |= 0x01;		//uart1 use timer2
	AUXR |= 0x04;		//timer2 1T
	T2L = 0xD0;			//
	T2H = 0xFF;		
	AUXR |= 0x10;		//launch timer2
} 

void main(void)
{
	logic_pin_config();
	confirm_wake_type();
	Timer_config();
	key_init();
	UART_init();
//	initial_heart_led();
	init_watchdog();
	EA = 1;
	enable_battery();	
	heart_beat_logger_init();

	while (1)
	{
		key_monitor_run();
		power_manager_run();
//		heart_led_run();
		heart_beat_logger_run();
		service_watchdog();
	}
}
