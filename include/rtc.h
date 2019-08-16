#ifndef _RTC_H_
#define _RTC_H_

ALWAYS_INLINE void RTC_init(void) {
	NVIC_InitTypeDef NVIC_init;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_init.NVIC_IRQChannel = RTC_IRQn;
	NVIC_init.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_init.NVIC_IRQChannelSubPriority = 0;
	NVIC_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_init);

	if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET) {
		//Do something after resuming.

		PWR_ClearFlag(PWR_FLAG_SB);

	    RTC_WaitForSynchro();

	    RTC_ITConfig(RTC_IT_SEC, ENABLE);
	    RTC_WaitForLastTask();
	} else {
		RCC_APB1PeriphClockCmd(
				RCC_APB1Periph_PWR |
				RCC_APB1Periph_BKP, ENABLE);

		PWR_BackupAccessCmd(ENABLE);

		BKP_DeInit();

		RCC_LSEConfig(RCC_LSE_ON);
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
		RCC_RTCCLKCmd(ENABLE);

		RTC_WaitForSynchro();
		RTC_WaitForLastTask();

		RTC_ITConfig(RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();

		RTC_SetPrescaler(32767);
		RTC_WaitForLastTask();

		RTC_SetCounter(0);
		RTC_WaitForLastTask();
	}
}

ALWAYS_INLINE uint32_t RTC_now(uint8_t *pms) {
	if(pms)
		*pms=RTC_GetDivider()*100/32768;

	return RTC_GetCounter();
}

ALWAYS_INLINE void RTC_set(uint32_t ts) {
	return RTC_SetCounter(ts);
}

#endif
