#include "logger.h"
#include "STC15Fxxxx.H"
#include "timer.h"

static CTimer heart_beat_timer;
uint8_t heart_beat_logger_byte = BYTE_UNINIT_HEART_BEAT;

void log_byte(unsigned  char dat)
{
//	return ;
	SBUF = dat;
	while(!TI);	    
 	TI=0;
}

void heart_beat_logger_init()
{
	timer_set_period(&heart_beat_timer, 500);//40s
  timer_reset(&heart_beat_timer);
}

void heart_beat_logger_run()
{
	if(timer_is_timeup(&heart_beat_timer))
	{
		timer_reset(&heart_beat_timer);
		log_byte(heart_beat_logger_byte);
	}
}

//end of file
