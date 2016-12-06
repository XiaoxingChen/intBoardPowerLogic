#include "power_manager.h"
#include "timer.h"
#include "GPIO.h"
#include "key_monitor.h"
#include "logic_out.h"

enum wake_type
{
    NORMAL_LUANCH = 1,
    HY_LAUNCH,
    BRAKE_LAUNCH
};

/* either KEY_HY_OFF or KEY_LAUNCH can shutdown the robot */
static uint8_t virtual_shutdown_key = 0xFF;

/* either KEY_HY_ON or KEY_LAUNCH can launch the robot */
static uint8_t virtual_launch_key = 0xFF;

/*  */
static uint8_t is_shutdown_key_enable = 0;

/* shutdown state timer */
CTimer shutdownTimer;
static uint8_t is_logic_shuttingdown = 0;

/**
  * @brief  confirm who wake up the power manager MCU
  * @param  None
  * @retval None
  * @note   call this function after powerup as soon as possible
  */
void confirm_wake_type()
{
    if(PRESSED == key_get_raw_data(KEY_LAUNCH))
    {
        virtual_shutdown_key = KEY_LAUNCH;
        virtual_launch_key = KEY_LAUNCH;
        launch_board();
    }else if(PRESSED == key_get_raw_data(KEY_HY_ON))
    {
        virtual_shutdown_key = KEY_HY_OFF;
        virtual_launch_key = KEY_HY_ON;
        launch_board();
    }else if(PRESSED == key_get_raw_data(KEY_BRAKE))
    {
        virtual_shutdown_key = 0xFF;
        virtual_launch_key = 0xFF;
        disable_battery();
    }else //confirm failed
    {
        /* For the reason of safety, this default option can 
        be changed according to the actual product */
        virtual_shutdown_key = KEY_LAUNCH;
        virtual_launch_key = KEY_LAUNCH;
    }
}

/**
  * @brief  run power manager logic
  * @param  None
  * @retval None
  */
void power_manager_run()
{   
    /* after the releasing of virtual launch key, shutdown key is enabled */
    if(!is_shutdown_key_enable && RELEASED == key_get_data(virtual_launch_key))
    {
        is_shutdown_key_enable = 1;
    }

    /* forwarding shutdown key to PC */
    if(is_shutdown_key_enable)
    {
        if(PRESSED == key_get_data(virtual_shutdown_key))
        {
            pc_en_line_low();
            if(!is_logic_shuttingdown)
            {
                is_logic_shuttingdown = 1;
                timer_set_period(&shutdownTimer, 40000);//40s
                timer_reset(&shutdownTimer);
            }
        }else if(RELEASED == key_get_data(virtual_shutdown_key))
        {
            pc_en_line_high();
        }
    }

    /* shutdown board and battery after PC is shutted down */
    if(timer_is_timeup(&shutdownTimer) || RELEASED == key_get_data(KEY_IS_PC_LAUNCH))
    {
        shutdown_board();
        disable_battery();
    }
		
		/* longpress shutdown logic */
		if(is_logic_shuttingdown && key_is_long_pressed(virtual_shutdown_key))
		{
			shutdown_board();
      disable_battery();
		}
}

//end of file
