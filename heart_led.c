#include "heart_led.h"
#include "GPIO.h"

void initial_heart_led(void)
{
	GPIO_InitTypeDef	GPIO_InitStructure;		
	
	GPIO_InitStructure.Pin  = HEART_LED_PIN;		
	GPIO_InitStructure.Mode = GPIO_OUT_PP;		
	GPIO_Inilize(HEART_LED_GPIO, &GPIO_InitStructure);
}

void heart_led_run()
{
	static uint32_t cnt = 0;
	static uint8_t led_on = 1;
	if(++cnt > HEART_LED_PERIOD)
	{
		cnt = 0;
		led_on = !led_on;
	}
	
	if(led_on)
		GPIO_SetBits(HEART_LED_GPIO, HEART_LED_PIN);
	else
		GPIO_ResetBits(HEART_LED_GPIO, HEART_LED_PIN);
}