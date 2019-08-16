#include "common.h"

#define	SECOND	(1)
#define MINUTE	(60*SECOND)
#define HOUR	(60*MINUTE)
#define DAY		(24*HOUR)

const uint8_t days_in_a_month[]={
	31, 28, 31, 30, 31, 30,
	31, 31, 30, 31, 30, 31
};

#define DAYS_IN_A_MONTH(month)		(days_in_a_month[(month)]*DAY)
#define DAYS_IN_A_YEAR(year)		((365+leap_year_complement((year)))*DAY)

const uint16_t leap_years_before_1970=1970/4-1970/100+1970/400;

#define LEAP_YEARS_FROM_1970(year)	(((year)/4-(year)/100+(year)/400)-leap_years_before_1970)

ALWAYS_INLINE uint8_t leap_year_complement(uint16_t y) {
	if(y%4)
		return 0;

	if(y%100)
		return 1;

	if(y%400)
		return 0;

	return 1;
}

#define IS_LEAP_YEAR(year)			leap_year_complement(year)

void CALENDAR_datetime_from_timestamp(uint32_t ts, uint16_t *py, uint8_t *pm, uint8_t *pd, uint8_t *phh, uint8_t *pmm, uint8_t *pss) {
	*py=1970;
	while(1) {
		uint32_t days=DAYS_IN_A_YEAR(*py);

		if(ts>=days) {
			(*py)++;
			ts-=days;
		} else
			break;
	}

	uint32_t lyc;

	*pm=0;
	lyc=leap_year_complement(*py)*DAY;
	while(1) {
		uint32_t days=DAYS_IN_A_MONTH(*pm);

		if(*pm==1)
			days+=lyc;

		if(ts>=days) {
			(*pm)++;
			ts-=days;
		} else
			break;
	}
	(*pm)++;

	*pd=ts/DAY;
	ts%=DAY;
	(*pd)++;

	*phh=ts/HOUR;
	ts%=HOUR;

	*pmm=ts/MINUTE;
	ts%=MINUTE;

	*pss=ts;
}

uint32_t CALENDAR_timestamp_from_datetime(uint16_t y, uint8_t m, uint8_t d, uint8_t hh, uint8_t mm, uint8_t ss) {
	uint32_t ts=(y>1970)?(((y-1970)*365+LEAP_YEARS_FROM_1970(y-1))*DAY):0;
	uint8_t i;

	for(i=0;i<m-1;i++) {
		ts+=DAYS_IN_A_MONTH(i);
		if(i==1)
			ts+=leap_year_complement(y)*DAY;
	}

	ts+=(d-1)*DAY;

	ts+=hh*HOUR;

	ts+=mm*MINUTE;

	ts+=ss;

	return ts;
}
