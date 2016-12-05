
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

#define BATT_EN_PIN				GPIO_Pin_2
#define BATT_EN_GPIO			GPIO_P1

#define BRD_nEN_PIN				GPIO_Pin_4
#define BRD_nEN_GPIO			GPIO_P1

#define PC_EN_PIN				GPIO_Pin_6
#define PC_EN_GPIO				GPIO_P3

#define launch_board()		GPIO_ResetBits(BRD_nEN_GPIO, BRD_nEN_PIN)
#define shutdown_board()	GPIO_SetBits(BRD_nEN_GPIO, BRD_nEN_PIN)

#define enable_battery()	GPIO_SetBits(BATT_EN_GPIO, BATT_EN_PIN)
#define disable_battery()	GPIO_ResetBits(BATT_EN_GPIO, BATT_EN_PIN)


/************************ IO Configuration ****************************/
void	GPIO_config(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;		

//---------------Output Port------------------
	
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

/************************ ��ʱ������ ****************************/
void	Timer_config(void)
{
	TIM_InitTypeDef		TIM_InitStructure;					//�ṹ����
	
	TIM_InitStructure.TIM_Mode      = TIM_16BitAutoReload;	//ָ������ģʽ,   TIM_16BitAutoReload,TIM_16Bit,TIM_8BitAutoReload,TIM_16BitAutoReloadNoMask
	TIM_InitStructure.TIM_Polity    = PolityLow;			//ָ���ж����ȼ�, PolityHigh,PolityLow
	TIM_InitStructure.TIM_Interrupt = ENABLE;				//�ж��Ƿ�����,   ENABLE��DISABLE. (ע��: Timer2�̶�Ϊ16λ�Զ���װ, �жϹ̶�Ϊ�����ȼ�)
	TIM_InitStructure.TIM_ClkSource = TIM_CLOCK_12T;		//ָ��ʱ��Դ,     TIM_CLOCK_1T,TIM_CLOCK_12T,TIM_CLOCK_Ext
	TIM_InitStructure.TIM_ClkOut    = DISABLE;				//�Ƿ�������������, ENABLE��DISABLE
	TIM_InitStructure.TIM_Value     = 65536UL - (MAIN_Fosc / (1000*12));		//��ֵ
	TIM_InitStructure.TIM_Run       = ENABLE;				//�Ƿ���ʼ����������ʱ��, ENABLE��DISABLE
	Timer_Inilize(Timer2,&TIM_InitStructure);				//��ʼ��Timer2	  Timer0,Timer1,Timer2

}

CTimer testTimer;

void main(void)
{

	GPIO_config();
	Timer_config();
	key_init();
	EA = 1;
	launch_board();
	enable_battery();
//	GPIO_SetBits(BRD_nEN_GPIO, BRD_nEN_PIN);	
	
	timer_set_period(&testTimer, 500);
	while (1)
	{
		key_monitor_run();
		
		if(key_is_long_pressed(KEY_LAUNCH))
		{
			shutdown_board();
			disable_battery();
		}
		else	if(PRESSED == key_get_data(KEY_LAUNCH))
		{
			launch_board();
			enable_battery();
		}
		
	}
}



