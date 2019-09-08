#include "common.h"

void RTC_IRQHandler(void) {
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
		RTC_ClearITPendingBit(RTC_IT_SEC);

		RTC_WaitForLastTask();
	}
}

ALWAYS_INLINE void nvic_init(void) {
	NVIC_InitTypeDef NVIC_init;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_init.NVIC_IRQChannel = RTC_IRQn;
	NVIC_init.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_init.NVIC_IRQChannelSubPriority = 0;
	NVIC_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_init);
}

ALWAYS_INLINE void rtc_init(void) {
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
}

ALWAYS_INLINE void rtc_init_partial(void) {
	RCC_APB1PeriphClockCmd(
			RCC_APB1Periph_PWR |
			RCC_APB1Periph_BKP, ENABLE);

	PWR_BackupAccessCmd(ENABLE);

	RCC_RTCCLKCmd(ENABLE);

    RTC_WaitForSynchro();

    RTC_ITConfig(RTC_IT_SEC, ENABLE);

    RTC_WaitForLastTask();
}

void RTC_init(void) {
	nvic_init();

	if(BKP_ReadBackupRegister(BKP_DR1)!=0xA5A5) {
		rtc_init();

		RTC_WaitForLastTask();

		RTC_SetCounter(0);

		RTC_WaitForLastTask();

		BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
	} else {
		uint32_t tmp;

		rtc_init_partial();

		tmp=RTC_GetCounter();

		RTC_SetCounter(tmp);
	}
}
