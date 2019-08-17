#ifndef _CALENDAR_H_
#define _CALENDAR_H_

ALWAYS_INLINE int32_t CALENDAR_is_leap_year(uint16_t y) {
	if(y%4)
		return FALSE;

	if(y%100)
		return TRUE;

	if(y%400)
		return FALSE;

	return TRUE;
}

void CALENDAR_datetime_from_timestamp(uint32_t ts, uint16_t *py, uint8_t *pm, uint8_t *pd, uint8_t *phh, uint8_t *pmm, uint8_t *pss);
uint32_t CALENDAR_timestamp_from_datetime(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss);

#endif
