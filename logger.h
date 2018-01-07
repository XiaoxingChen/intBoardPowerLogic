#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdint.h>

#define BYTE_GET_WAKE_TYPE 					0x01
#define BYTE_LAUNCH_KEY_RELEASED 		0x02
#define BYTE_SHUTDOWN_KEY_PRESSED 	0x03

#define BYTE_PC_ACTIVE_SHUTDOWN_DETECT										0x04
#define BYTE_SHUTDOWN_KEY_RELEASED_AFTER_ACTIVE_SHUTDOWN 	0x05
#define BYTE_SHUTDOWN_KEY_RELEASED_WHILE_SHUTTINGDOWN 		0x06
#define BYTE_SHUTDOWN_KEY_PRESSED_WHILE_SHUTTINGDOWN 			0x07
#define BYTE_PC_PASSIVE_SHUTDOWN_DETECT										0x08
#define BYTE_SHUTDOWN_TIMER_TIMEUP												0x09
#define BYTE_SHUTDOWN_KEY_RELEASED_AFTER_PASSIVE_SHUTDOWN 0x0A

#define BYTE_UNINIT_HEART_BEAT														0x10
#define BYTE_STANDBY_HEART_BEAT														0x11
#define BYTE_LAUNCH_PRESSING_HEART_BEAT										0x12
#define BYTE_WORKING_HEART_BEAT														0x13
#define BYTE_SHUTTINGDOWN_HEART_BEAT											0x14

void log_byte(unsigned  char dat);
void heart_beat_logger_init();
void heart_beat_logger_run();

extern uint8_t heart_beat_logger_byte;

#endif
//end of file
