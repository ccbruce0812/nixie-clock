#include "common.h"

void RTC_IRQHandler(void) {
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET) {
		RTC_ClearITPendingBit(RTC_IT_SEC);

		RTC_WaitForLastTask();
	}
}
