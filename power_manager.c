#include "power_manager.h"
#include "timer.h"
#include "GPIO.h"
#include "key_monitor.h"
#include "logic_out.h"
#include "logger.h"
#include "watchdog.h"

/* either KEY_HY_OFF or KEY_LAUNCH can shutdown the robot */
static uint8_t virtual_shutdown_key = 0xFF;

/* either KEY_HY_ON or KEY_LAUNCH can launch the robot */
static uint8_t virtual_launch_key = 0xFF;

/* shutdown state timer */

#define LONGPRESS_FORCE_SHUTDOWN	0

CTimer shutdownTimer;
static uint8_t is_logic_shuttingdown = 0;

enum BRD_STATE_TYPE
{
	BS_LAUNCH_PRESSING,
	BS_WORKING,
	BS_SHUTDOWN_PRESSING,
	BS_SHUTTING_DOWN,
	BS_STANDBY
};

/**
  * @brief  confirm who wake up the power manager MCU
  * @param  None
	* @retval 0: nothing happend 
	*					1: launch pressed 
  * @note   call this function after powerup as soon as possible
  */
int confirm_wake_type()
{
    if(PRESSED == key_get_raw_data(KEY_DCIN))
    {
			virtual_shutdown_key = KEY_ON_BRD;
			virtual_launch_key = KEY_ON_BRD;
			if(PRESSED == key_get_raw_data(KEY_ON_BRD))
				return 1;
			else
				return 0;
    }
		else if(PRESSED == key_get_raw_data(KEY_LAUNCH))
    {
			virtual_shutdown_key = KEY_LAUNCH;
			virtual_launch_key = KEY_LAUNCH;
			return 1;
    }
		else if(PRESSED == key_get_raw_data(KEY_HY_ON))
    {
			virtual_shutdown_key = KEY_HY_OFF;
			virtual_launch_key = KEY_HY_ON;
			return 1;
    }
		/* when battery key is not used */
		else
    {
			virtual_shutdown_key = 0xFF;
			virtual_launch_key = 0xFF;
			disable_battery();
			shutdown_board();
			return 0;
    }
}

/**
  * @brief  block waiting before shutdown, reserve time for logger
  * @param  None
  * @retval None
  */
void wait_for_logging_before_shutdown()
{
	int i = 0;
	for(i = 0; i < 100; i++)
	{
		delay_ms(50);
		service_watchdog();
		heart_beat_logger_run();
	}
}

/**
  * @brief  run power manager logic
  * @param  None
  * @retval None
  */
void power_manager_run()
{   
	static uint8_t board_state = BS_STANDBY;
	switch(board_state)
	{
		case (BS_STANDBY):
		{
			heart_beat_logger_byte = BYTE_STANDBY_HEART_BEAT;
			
			if(1 == confirm_wake_type())
			{
				log_byte(BYTE_GET_WAKE_TYPE);
				launch_board();
				enable_battery();
				
				/* forwarding virtual shutdown key to PC */
				pc_en_line_low();
				
				/* switch state */
				board_state = BS_LAUNCH_PRESSING;
				
				delay_ms(500);
			}
			break;
		}
		case (BS_LAUNCH_PRESSING):
		{
			heart_beat_logger_byte = BYTE_LAUNCH_PRESSING_HEART_BEAT;
			
			if(RELEASED == key_get_data(virtual_launch_key))
			{
				log_byte(BYTE_LAUNCH_KEY_RELEASED);
				/* forwarding virtual shutdown key to PC */
				pc_en_line_high();

				/* switch state */
				board_state = BS_WORKING;
			}
			break;
		}
		case (BS_WORKING):
		{
			heart_beat_logger_byte = BYTE_WORKING_HEART_BEAT;
			
			if(PRESSED == key_get_data(virtual_shutdown_key))
			{
				log_byte(BYTE_SHUTDOWN_KEY_PRESSED);
				/* start shutdown timer */
				timer_set_period(&shutdownTimer, 40000);//40s
        timer_reset(&shutdownTimer);
				
				/* switch state */
				board_state = BS_SHUTTING_DOWN;
			}
			
			/* shutdown from PC desktop */
			if(RELEASED == key_get_data(KEY_IS_PC_LAUNCH))
			{
				log_byte(BYTE_PC_ACTIVE_SHUTDOWN_DETECT);
				
				wait_for_logging_before_shutdown();
				
				shutdown_board();
				disable_battery();
				if(RELEASED == key_get_data(virtual_shutdown_key))
				{
					log_byte(BYTE_SHUTDOWN_KEY_RELEASED_AFTER_ACTIVE_SHUTDOWN);
					board_state = BS_STANDBY;
				}
			}
			break;
		}
		case (BS_SHUTTING_DOWN):
		{
			static key_state prev_shutdown_key_state = NO_SUCH_KEY;
			key_state curr_shutdown_key_state = key_get_data(virtual_shutdown_key);
			
			heart_beat_logger_byte = BYTE_SHUTTINGDOWN_HEART_BEAT;
			
			/* shutdown key long press is still effective*/
#if LONGPRESS_FORCE_SHUTDOWN
			if(key_is_long_pressed(virtual_shutdown_key))
			{
				wait_for_logging_before_shutdown();
				shutdown_board();
				disable_battery();
			}
#endif
			
			if(RELEASED == curr_shutdown_key_state)
			{
				if(curr_shutdown_key_state != prev_shutdown_key_state)
					log_byte(BYTE_SHUTDOWN_KEY_RELEASED_WHILE_SHUTTINGDOWN);
				
				pc_en_line_high();
			}
			else if(PRESSED == curr_shutdown_key_state)
			{
				if(curr_shutdown_key_state != prev_shutdown_key_state)
					log_byte(BYTE_SHUTDOWN_KEY_PRESSED_WHILE_SHUTTINGDOWN);
				
				pc_en_line_low();
			}
			
			prev_shutdown_key_state = curr_shutdown_key_state;
			
			if(timer_is_timeup(&shutdownTimer) || RELEASED == key_get_data(KEY_IS_PC_LAUNCH))
			{
				if(RELEASED == key_get_data(KEY_IS_PC_LAUNCH))
				{
					log_byte(BYTE_PC_PASSIVE_SHUTDOWN_DETECT);
				}
				else
				{
					log_byte(BYTE_SHUTDOWN_TIMER_TIMEUP);
				}
				
				wait_for_logging_before_shutdown();
				
				shutdown_board();
				disable_battery();
				if(RELEASED == key_get_data(virtual_shutdown_key))
				{
					log_byte(BYTE_SHUTDOWN_KEY_RELEASED_AFTER_PASSIVE_SHUTDOWN);
					
					board_state = BS_STANDBY;
				}
			}
			
			
			break;
		}
		
		default:
			break;
	}
	
}
//end of file
