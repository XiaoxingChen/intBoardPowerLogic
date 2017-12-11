#ifndef __HEART_LED_H
#define __HEART_LED_H

#define HEART_LED_PIN			GPIO_Pin_1
#define HEART_LED_GPIO		GPIO_P3
#define HEART_LED_PERIOD	6000

void initial_heart_led(void);
void heart_led_run();

#endif