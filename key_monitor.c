#include "key_monitor.h"
#include "timer.h"
#include "GPIO.h"

struct key_struct
{
	const uint8_t KEY_GPIO_X_;
	const uint8_t KEY_GPIO_PIN_;
	const uint8_t IS_PRESSED; 		//is 0 means pressed or 1 mean pressed. 
																//It depends on hardware connection
	key_state key_state_;
	CTimer timer_;
	
};

key_type key_array[TOTAL_KEY_NUM] = 
{
	{GPIO_P1, GPIO_Pin_3, 1, RELEASED}, //KEY_IS_PC_LAUNCH
	{GPIO_P5, GPIO_Pin_4, 1, RELEASED}, //KEY_LAUNCH
	{GPIO_P5, GPIO_Pin_5, 1, RELEASED}, //KEY_BRAKE
	{GPIO_P1, GPIO_Pin_1, 1, RELEASED}, //KEY_DCIN
	{GPIO_P1, GPIO_Pin_0, 1, RELEASED}, //KEY_HY_ON
	{GPIO_P3, GPIO_Pin_7, 0, RELEASED}  //KEY_HY_OFF
}; 

void key_init()
{
	int i;
	GPIO_InitTypeDef	GPIO_InitStructure;		
	
	for(i = 0; i < TOTAL_KEY_NUM; i++)
	{
		/* key bsp gpio init */
		GPIO_InitStructure.Pin  = key_array[i].KEY_GPIO_PIN_;		
		GPIO_InitStructure.Mode = GPIO_PullUp;		
		GPIO_Inilize(key_array[i].KEY_GPIO_X_, &GPIO_InitStructure);
	}	
}

/**
  * @brief  Get pin raw data without filter
  * @param  key_index 
  * @retval pressed or released
  */
key_state key_get_raw_data(int key_index)
{
	uint8_t key_gpio = key_array[key_index].KEY_GPIO_X_;
	uint8_t key_pin = key_array[key_index].KEY_GPIO_PIN_;

	if(key_index >= TOTAL_KEY_NUM)
		return 0;

	if(GPIO_ReadInputDataBit(key_gpio, key_pin) == key_array[key_index].IS_PRESSED)
	{
		return PRESSED;
	}
	else
	{
		return RELEASED;
	}
}

/**
  * @brief  Get pin raw data after filterd
  * @param  key_index 
  * @retval 
  */
key_state key_get_data(int key_index)
{
	if(key_index >= TOTAL_KEY_NUM)
		return NO_SUCH_KEY;

	return key_array[key_index].key_state_;
}

/**
  * @brief  judge if key is pressed for a definited time
  * @param  key_index 
  * @retval 
  */
uint8_t key_is_long_pressed(int key_index)
{
	uint8_t ret;

	if(key_index >= TOTAL_KEY_NUM)
		return 0;

	ret = (key_array[key_index].key_state_ == PRESSED 
			&& timer_get_time(&key_array[key_index].timer_) > 3000);
	return (ret);
}

/**
  * @brief  run key monitor for one time, update all keys' state
  * @param  None
  * @retval None
  */
void key_monitor_run()
{
	int i;
	uint8_t temp_state = 0;
	for(i = 0; i < TOTAL_KEY_NUM; i++)
	{
		/* RELEASED ===> PRESSING */
		if(RELEASED == key_array[i].key_state_ 
			&& PRESSED == key_get_raw_data(i))
		{
			timer_set_period(&key_array[i].timer_, 10);
			timer_reset(&key_array[i].timer_);
			key_array[i].key_state_ = PRESSING;
		}
		
		/* RELEASED <== PRESSING ==> PRESSED */
		else if(PRESSING == key_array[i].key_state_
			&& timer_is_timeup(&key_array[i].timer_))
		{
			if(PRESSED == key_get_raw_data(i))
			{
				key_array[i].key_state_ = PRESSED;
				timer_set_period(&key_array[i].timer_, 60000);
				timer_reset(&key_array[i].timer_);
			}
			else 
			{
				key_array[i].key_state_ = RELEASED;
			}			
		}
		
		/* PRESSED ==> RELEASING */
		else if(PRESSED == key_array[i].key_state_
			&& RELEASED == key_get_raw_data(i))
		{
			timer_set_period(&key_array[i].timer_, 10);
			timer_reset(&key_array[i].timer_);
			key_array[i].key_state_ = RELEASING;
		}
		
		/* PRESSED <== RELEASING ==> RELEASED */
		else if(RELEASING == key_array[i].key_state_
			&& timer_is_timeup(&key_array[i].timer_))
		{
			if(PRESSED == key_get_raw_data(i))
			{
				key_array[i].key_state_ = PRESSED;
			}
			else 
			{
				key_array[i].key_state_ = RELEASED;
			}
		}
		
	}	
}

//end of file
