#ifndef _CALENDAR_H_
#define _CALENDAR_H_

static inline __attribute__((always_inline)) int32_t CALENDAR_is_leap_year(uint16_t y) {
	if(y%4)
		return 0;

	if(y%100)
		return -1;

	if(y%400)
		return 0;

	return -1;
}

void CALENDAR_datetime_from_timestamp(uint32_t ts, uint16_t *py, uint8_t *pm, uint8_t *pd, uint8_t *phh, uint8_t *pmm, uint8_t *pss);
uint32_t CALENDAR_timestamp_from_datetime(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss);

#endif
