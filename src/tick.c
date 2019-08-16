#include "common.h"

#define ONE_KHZ	(1000)

volatile uint32_t g_sleep_countdown=0;
volatile uint32_t g_now_ts=0;

void SysTick_Handler(void) {
	if(g_sleep_countdown!=0) {
		g_sleep_countdown--;
	}

	g_now_ts++;
}

void TICK_init(void) {
  SysTick_Config(SystemCoreClock/ONE_KHZ);
}

void TICK_msleep(uint32_t msec) {
  g_sleep_countdown=msec;

  while(g_sleep_countdown);
}

uint32_t TICK_now(void) {
	return g_now_ts;
}
