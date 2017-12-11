#include "STC15Fxxxx.H"
#include "watchdog.h"

//3 bits prescal factor from 0x01 to 0x07
#define PRESCAL_FACTOR 0x04

void init_watchdog()
{
	WDT_CONTR = (0x30 | PRESCAL_FACTOR);
}

void service_watchdog()
{
	WDT_CONTR = (0x30 | PRESCAL_FACTOR);
}
