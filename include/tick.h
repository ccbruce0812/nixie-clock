#ifndef _TICK_H_
#define _TICK_H_

void TICK_init(void);
void TICK_msleep(uint32_t msec);
uint32_t TICK_now(void);

ALWAYS_INLINE void TICK_usleep(uint32_t usecs) {
	uint32_t base=SystemCoreClock/1000000;
	uint32_t i;

	for(i=0;i<base*usecs;i++)
		__NOP();
}

#endif
