#ifndef _RTC_H_
#define _RTC_H_

void RTC_init(void);

ALWAYS_INLINE uint32_t RTC_now(uint8_t *pms) {
	if(pms)
		*pms=RTC_GetDivider()*100/32768;

	return RTC_GetCounter();
}

ALWAYS_INLINE void RTC_set(uint32_t ts) {
	return RTC_SetCounter(ts);
}

#endif
