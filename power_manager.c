#include "power_manager.h"
#include "timer.h"
#include "GPIO.h"
#include "key_monitor.h"
#include "logic_out.h"

/* either KEY_HY_OFF or KEY_LAUNCH can shutdown the robot */
static uint8_t virtual_shutdown_key = 0xFF;

/* either KEY_HY_ON or KEY_LAUNCH can launch the robot */
static uint8_t virtual_launch_key = 0xFF;

/* enable shutdown detect */
static uint8_t is_shutdown_key_enable = 0;

/* enable launch detect */
static uint8_t is_launch_key_enable = 1;

/* shutdown state timer */
CTimer shutdownTimer;
static uint8_t is_logic_shuttingdown = 0;

enum BRD_STATE_TYPE
{
	BS_LAUNCH_PRESSING,
	BS_WORKING,
	BS_SHUTDOWN_PRESSING,
	BS_SHUTTING_DOWN,
	BS_STANDBY/*,
	BS_RELEASE_BRAKE*/
};

/**
  * @brief  confirm who wake up the power manager MCU
  * @param  None
	* @retval 0: nothing happend 
	*					1: launch pressed 
	*					2: release brake
  * @note   call this function after powerup as soon as possible
  */
int confirm_wake_type()
{
    if(PRESSED == key_get_raw_data(KEY_LAUNCH))
    {
			virtual_shutdown_key = KEY_LAUNCH;
			virtual_launch_key = KEY_LAUNCH;
			launch_board();
			return 1;
    }
		else if(PRESSED == key_get_raw_data(KEY_HY_ON))
    {
        virtual_shutdown_key = KEY_HY_OFF;
        virtual_launch_key = KEY_HY_ON;
//        launch_board();
				return 1;
    }
//		else if(PRESSED == key_get_raw_data(KEY_BRAKE))
//    {
//        virtual_shutdown_key = 0xFF;
//        virtual_launch_key = 0xFF;
//        disable_battery();
//				shutdown_board();
//				return 2;
//    }
		/* when battery key is not used */
		/* when DC socket is used */
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
			if(1 == confirm_wake_type())
			{
				launch_board();
				
				/* forwarding virtual shutdown key to PC */
				pc_en_line_low();
				
				/* switch state */
				board_state = BS_LAUNCH_PRESSING;
			}
//			/* brake release */
//			else if(2 == confirm_wake_type())
//			{
//				board_state = BS_RELEASE_BRAKE;
//			}
			break;
		}
		case (BS_LAUNCH_PRESSING):
		{
			if(RELEASED == key_get_data(virtual_launch_key))
			{
				/* forwarding virtual shutdown key to PC */
				pc_en_line_high();

				/* switch state */
				board_state = BS_WORKING;
			}
			break;
		}
		case (BS_WORKING):
		{
			if(PRESSED == key_get_data(virtual_shutdown_key))
			{
				/* start shutdown timer */
				timer_set_period(&shutdownTimer, 40000);//40s
        timer_reset(&shutdownTimer);
				
				/* forwarding virtual shutdown key to PC */
				pc_en_line_low();
				
				/* switch state */
				board_state = BS_SHUTDOWN_PRESSING;
			}
			
			/* shutdown from PC desktop */
			if(RELEASED == key_get_data(KEY_IS_PC_LAUNCH))
			{
				shutdown_board();
				disable_battery();
				board_state = BS_STANDBY;
			}
			break;
		}
		
		case (BS_SHUTDOWN_PRESSING):
		{
			/* shutdown key long press */
			if(key_is_long_pressed(virtual_shutdown_key))
			{
				shutdown_board();
				disable_battery();
			}
			/* shutdown key short press */
			else if(RELEASED == key_get_data(virtual_shutdown_key))
			{
				/* forwarding virtual shutdown key to PC */
				pc_en_line_high();
				
				if(RELEASED == key_get_data(KEY_IS_PC_LAUNCH))
				{ /* means PC is shutted down */
					shutdown_board();
					disable_battery();
					board_state = BS_STANDBY;
				}else //IS_PC_LAUNCH
				{
					board_state = BS_SHUTTING_DOWN;
				}
			}
			break;
		}
		case (BS_SHUTTING_DOWN):
		{
			/* shutdown key long press is still effective*/
			if(key_is_long_pressed(virtual_shutdown_key))
			{
				shutdown_board();
				disable_battery();
			}
			
			if(timer_is_timeup(&shutdownTimer) || RELEASED == key_get_data(KEY_IS_PC_LAUNCH))
			{
				shutdown_board();
				disable_battery();
				board_state = BS_STANDBY;
			}
			break;
		}
		
//		case (BS_RELEASE_BRAKE):
//		{
//			if(RELEASED == key_get_data(KEY_BRAKE))
//			{
//				disable_battery();
//				board_state = BS_STANDBY;
//			}
//			break;
//		}
		
		default:
			break;
	}
	
}
//end of file
