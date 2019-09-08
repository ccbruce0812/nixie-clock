#include "common.h"

#define NIXIE_UPDATING_PERIOD_1_SEC			(1000)
#define NIXIE_UPDATING_PERIOD_10_MSEC		(10)
#define NIXIE_REFRESHING_PERIOD				(1)
#define NIXIE_BL_UPDATING_PERIOD			(50)
#define NIXIE_BL_INDICATOR_TOGGLING_PERIOD	(500)
#define NIXIE_SVR_RUNNING_PERIOD			(60*60*1000)
#define IR_CHECKING_PERIOD					(1)
#define IR_INTERVAL							(500)
#define BT_RESTORING_PERIOD					(50)

typedef struct {
	/*
	 * 0: NIXIE_UPDATING_PERIOD
	 * 1: NIXIE_REFRESHING_PERIOD
	 * 2: NIXIE_BL_UPDATING_PERIOD
	 * 3: NIXIE_BL_INDICATOR_TOGGLING_PERIOD
	 * 4: NIXIE_SVR_RUNNING PERIOD
	 * 5: IR_CHECKING_PERIOD
	 * 6: IR_INTERVAL
	 * 7: BT_RESTORING_PERIOD
	 */
	uint32_t last_ts[8];
	uint32_t period[8];

	uint16_t new_y;
	uint8_t new_m;
	uint8_t new_d;
	uint8_t new_hh;
	uint8_t new_mm;
	uint8_t new_ss;
} CONTEXT;

static const NIXIE_BL_PIXEL g_y_toggle[2][NIXIE_BL_COUNT]={
    {
        {.prefix=1, .red=0, .green=0, .blue=0},
        {.prefix=1, .red=0, .green=0, .blue=0},
        {.prefix=1, .red=0, .green=0, .blue=0},
        {.prefix=1, .red=0, .green=0, .blue=0}
    }, {
        {.prefix=1, .red=31, .green=31, .blue=31},
        {.prefix=1, .red=31, .green=31, .blue=31},
        {.prefix=1, .red=31, .green=31, .blue=31},
        {.prefix=1, .red=31, .green=31, .blue=31}
    }
};

static const NIXIE_BL_PIXEL g_md_toggle[2][NIXIE_BL_COUNT]={
    {
        {.prefix=1, .red=0, .green=0, .blue=0},
        {.prefix=1, .red=0, .green=0, .blue=0},
        {.prefix=1, .red=0, .green=0, .blue=0},
        {.prefix=1, .red=0, .green=0, .blue=0}
    }, {
        {.prefix=1, .red=31, .green=31, .blue=0},
        {.prefix=1, .red=31, .green=31, .blue=0},
        {.prefix=1, .red=0, .green=31, .blue=31},
        {.prefix=1, .red=0, .green=31, .blue=31}
    }
};

static const NIXIE_BL_PIXEL g_hhmm_toggle[2][NIXIE_BL_COUNT]={
    {
        {.prefix=1, .red=0, .green=0, .blue=0},
        {.prefix=1, .red=0, .green=0, .blue=0},
        {.prefix=1, .red=0, .green=0, .blue=0},
        {.prefix=1, .red=0, .green=0, .blue=0}
    }, {
        {.prefix=1, .red=31, .green=0, .blue=31},
        {.prefix=1, .red=31, .green=0, .blue=31},
        {.prefix=1, .red=0, .green=0, .blue=31},
        {.prefix=1, .red=0, .green=0, .blue=31}
    }
};

static void update(const CONTEXT *pcontext, int32_t mode) {
	uint16_t y;
	uint8_t m, d, hh, mm, ss, ms;

    switch(mode) {
        case STATE_HHMM_MODE:
            CALENDAR_datetime_from_timestamp(RTC_now(NULL), &y, &m, &d, &hh, &mm, &ss);
			goto hhmm;

        case STATE_SS_MODE:
            CALENDAR_datetime_from_timestamp(RTC_now(&ms), &y, &m, &d, &hh, &mm, &ss);
            goto ssms;

        case STATE_MD_MODE:
        	CALENDAR_datetime_from_timestamp(RTC_now(NULL), &y, &m, &d, &hh, &mm, &ss);
			goto md;

        case STATE_Y_MODE:
        	CALENDAR_datetime_from_timestamp(RTC_now(NULL), &y, &m, &d, &hh, &mm, &ss);
            goto y;

        case STATE_SET_Y_MODE:
            y=pcontext->new_y;
            goto y;

        case STATE_SET_M_MODE:
        case STATE_SET_D_MODE:
            m=pcontext->new_m;
            d=pcontext->new_d;
			goto md;

        case STATE_SET_HH_MODE:
        case STATE_SET_MM_MODE:
            hh=pcontext->new_hh;
            mm=pcontext->new_mm;
			goto hhmm;

        default:
            ASSERT(0, "Bad flow. Check your code.\n");
    }

hhmm:
	NIXIE_update(0, mm%10);
	NIXIE_update(1, mm/10);
	NIXIE_update(2, (hh%10)|(ss%2?NIXIE_DOT:0));
	NIXIE_update(3, (hh/10)|(ss%2?NIXIE_DOT:0));
	return;

ssms:
	NIXIE_update(0, ms%10);
	NIXIE_update(1, ms/10);
	NIXIE_update(2, (ss%10)|(ss%2?NIXIE_DOT:0));
	NIXIE_update(3, (ss/10)|(ss%2?NIXIE_DOT:0));
	return;

md:
	NIXIE_update(0, d%10);
	NIXIE_update(1, d/10);
	NIXIE_update(2, (m%10)|NIXIE_DOT);
	NIXIE_update(3, (m/10)|NIXIE_DOT);
	return;

y:
	NIXIE_update(0, y%10);
	NIXIE_update(1, (y%100)/10);
	NIXIE_update(2, (y%1000)/100);
	NIXIE_update(3, y/1000);
}

int32_t on_initialize(const FSM *pfsm, int32_t input, void *parg) {
	(void)input;
	(void)parg;

	ASSERT(pfsm, "Bad arguments.\n");

	CONTEXT *pcontext=(CONTEXT *)pfsm->puser_data;

	TICK_init();

	RTC_init();
	RTC_set(0);

	NIXIE_PWR_init();
	NIXIE_BL_init();
	NIXIE_init();

	IR_init();

	BT_init();

	NIXIE_PWR_on();
	NIXIE_on();

	uint32_t ts=TICK_now(), i;

	for(i=0;i<COUNTOF(pcontext->last_ts);i++)
		pcontext->last_ts[i]=ts;

	pcontext->period[0]=NIXIE_UPDATING_PERIOD_1_SEC;
	pcontext->period[1]=NIXIE_REFRESHING_PERIOD;
	pcontext->period[2]=NIXIE_BL_UPDATING_PERIOD;
	pcontext->period[3]=NIXIE_BL_INDICATOR_TOGGLING_PERIOD;
	pcontext->period[4]=NIXIE_SVR_RUNNING_PERIOD;
	pcontext->period[5]=IR_CHECKING_PERIOD;
	pcontext->period[6]=IR_INTERVAL;
	pcontext->period[7]=BT_RESTORING_PERIOD;

	NIXIE_BL_PAT0_init();

    NIXIE_BL_INDICATOR_disable(0);
    NIXIE_BL_INDICATOR_disable(1);
    NIXIE_BL_INDICATOR_disable(2);
    NIXIE_BL_INDICATOR_disable(3);

	update(pcontext, STATE_HHMM_MODE);
	return STATE_HHMM_MODE;
}

int32_t on_mute(const FSM *pfsm, int32_t input, void *parg) {
	(void)input;
	(void)parg;

    ASSERT(pfsm, "Bad argument.\n");

    int32_t ret=pfsm->state;

    switch(pfsm->state) {
        case STATE_HHMM_MODE:
        case STATE_SS_MODE:
        case STATE_MD_MODE:
        case STATE_Y_MODE: {
        	int32_t state=BT_mute_get();

        	BT_mute_set(!state);
            break;
        }

        default:
            ASSERT(0, "Bad flow. Check your code.\n");
    }

    return ret;
}

int32_t on_pause(const FSM *pfsm, int32_t input, void *parg) {
	(void)input;
	(void)parg;

    ASSERT(pfsm, "Bad argument.\n");

    int32_t ret=pfsm->state;

    switch(pfsm->state) {
        case STATE_HHMM_MODE:
        case STATE_SS_MODE:
        case STATE_MD_MODE:
        case STATE_Y_MODE:
			BT_pause();
            break;

        default:
            ASSERT(0, "Bad flow. Check your code.\n");
    }

    return ret;
}

int32_t on_next(const FSM *pfsm, int32_t input, void *parg) {
	(void)input;
	(void)parg;

	ASSERT(pfsm, "Bad argument.\n");

    int32_t ret=pfsm->state;

    switch(pfsm->state) {
        case STATE_HHMM_MODE:
        case STATE_SS_MODE:
        case STATE_MD_MODE:
        case STATE_Y_MODE:
        	BT_next();
            break;

        default:
            ASSERT(0, "Bad flow. Check your code.\n");
    }

    return ret;
}

int32_t on_previous(const FSM *pfsm, int32_t input, void *parg) {
	(void)input;
	(void)parg;

    ASSERT(pfsm, "Bad argument.\n");

    int32_t ret=pfsm->state;

    switch(pfsm->state) {
        case STATE_HHMM_MODE:
        case STATE_SS_MODE:
        case STATE_MD_MODE:
        case STATE_Y_MODE:
        	BT_previous();
            break;

        default:
            ASSERT(0, "Bad flow. Check your code.\n");
    }

    return ret;
}

int32_t on_increase(const FSM *pfsm, int32_t input, void *parg) {
	(void)input;
	(void)parg;

    ASSERT(pfsm, "Bad argument.\n");

    CONTEXT *pcontext=(CONTEXT *)pfsm->puser_data;
    int32_t ret=pfsm->state;

    switch(pfsm->state) {
        case STATE_HHMM_MODE:
        case STATE_SS_MODE:
        case STATE_MD_MODE:
        case STATE_Y_MODE:
            BT_increase();
            break;

        case STATE_SET_Y_MODE:
            pcontext->new_y++;
            if(pcontext->new_y==2101)
                pcontext->new_y=1970;
            update(pcontext, pfsm->state);
            break;

        case STATE_SET_M_MODE:
            pcontext->new_m++;
            if(pcontext->new_m==13)
                pcontext->new_m=1;
            update(pcontext, pfsm->state);
            break;

        case STATE_SET_D_MODE: {
            uint8_t max;

            if(pcontext->new_m==1 ||
               pcontext->new_m==3 ||
               pcontext->new_m==5 ||
               pcontext->new_m==7 ||
               pcontext->new_m==8 ||
               pcontext->new_m==10 ||
               pcontext->new_m==12)
                max=32;
            else if(pcontext->new_m==2) {
                if(CALENDAR_is_leap_year(pcontext->new_y))
                    max=30;
                else
                    max=29;
            } else
                max=31;

            pcontext->new_d++;
            if(pcontext->new_d==max)
                pcontext->new_d=1;
            update(pcontext, pfsm->state);
            break;
        }

        case STATE_SET_HH_MODE:
            pcontext->new_hh=(pcontext->new_hh+1)%24;
            update(pcontext, pfsm->state);
            break;

        case STATE_SET_MM_MODE:
            pcontext->new_mm=(pcontext->new_mm+1)%60;
            update(pcontext, pfsm->state);
            break;

        default:
            ASSERT(0, "Bad flow. Check your code.\n");
    }

    return ret;
}

int32_t on_decrease(const FSM *pfsm, int32_t input, void *parg) {
	(void)input;
	(void)parg;

    ASSERT(pfsm, "Bad argument.\n");

    CONTEXT *pcontext=(CONTEXT *)pfsm->puser_data;
    int32_t ret=pfsm->state;

    switch(pfsm->state) {
        case STATE_HHMM_MODE:
        case STATE_SS_MODE:
        case STATE_MD_MODE:
        case STATE_Y_MODE:
            BT_decrease();
            break;

        case STATE_SET_Y_MODE:
            pcontext->new_y--;
            if(pcontext->new_y==1969)
                pcontext->new_y=2100;
            update(pcontext, pfsm->state);
            break;

        case STATE_SET_M_MODE:
            pcontext->new_m--;
            if(pcontext->new_m==0)
                pcontext->new_m=12;
            update(pcontext, pfsm->state);
            break;

        case STATE_SET_D_MODE: {
            uint8_t max;

            if(pcontext->new_m==1 ||
               pcontext->new_m==3 ||
               pcontext->new_m==5 ||
               pcontext->new_m==7 ||
               pcontext->new_m==8 ||
               pcontext->new_m==10 ||
               pcontext->new_m==12)
                max=31;
            else if(pcontext->new_m==2) {
                if(CALENDAR_is_leap_year(pcontext->new_y))
                    max=29;
                else
                    max=28;
            } else
                max=30;

            pcontext->new_d--;
            if(pcontext->new_d==0)
                pcontext->new_d=max;
            update(pcontext, pfsm->state);
            break;
        }

        case STATE_SET_HH_MODE:
            if(pcontext->new_hh==0)
                pcontext->new_hh=23;
            else
                pcontext->new_hh--;
            update(pcontext, pfsm->state);
            break;

        case STATE_SET_MM_MODE:
            if(pcontext->new_mm==0)
                pcontext->new_mm=59;
            else
                pcontext->new_mm--;
            update(pcontext, pfsm->state);
            break;

        default:
            ASSERT(0, "Bad flow. Check your code.\n");
    }

    return ret;
}

int32_t on_display(const FSM *pfsm, int32_t input, void *parg) {
	(void)input;
	(void)parg;

    ASSERT(pfsm, "Bad argument.\n");

    CONTEXT *pcontext=(CONTEXT *)pfsm->puser_data;
    int32_t ret=STATE_HHMM_MODE;

    switch(pfsm->state) {
        case STATE_HHMM_MODE:
        	pcontext->period[0]=NIXIE_UPDATING_PERIOD_10_MSEC;
        	update(pcontext, STATE_SS_MODE);
            ret=STATE_SS_MODE;
            break;

        case STATE_SS_MODE:
        	pcontext->period[0]=NIXIE_UPDATING_PERIOD_1_SEC;
        	update(pcontext, STATE_MD_MODE);
            ret=STATE_MD_MODE;
            break;

        case STATE_MD_MODE:
        	update(pcontext, STATE_Y_MODE);
            ret=STATE_Y_MODE;
            break;

        case STATE_Y_MODE:
        	update(pcontext, STATE_HHMM_MODE);
            ret=STATE_HHMM_MODE;
            break;

        case STATE_SET_Y_MODE:
        case STATE_SET_M_MODE:
        case STATE_SET_D_MODE:
        case STATE_SET_HH_MODE:
        case STATE_SET_MM_MODE: {
    		if(pcontext->new_m==2) {
    			if(!CALENDAR_is_leap_year(pcontext->new_y)) {
    				if(pcontext->new_d==29)
    					pcontext->new_d=28;
        		}
        	}

            uint32_t ts=CALENDAR_timestamp_from_datetime(pcontext->new_y,
                                                         pcontext->new_m,
                                                         pcontext->new_d,
                                                         pcontext->new_hh,
                                                         pcontext->new_mm,
                                                         pcontext->new_ss);

            RTC_set(ts);
            NIXIE_BL_INDICATOR_disable(0);
            NIXIE_BL_INDICATOR_disable(1);
            NIXIE_BL_INDICATOR_disable(2);
            NIXIE_BL_INDICATOR_disable(3);
            update(pcontext, STATE_HHMM_MODE);
            ret=STATE_HHMM_MODE;
            break;
        }

        default:
            ASSERT(0, "Bad flow. Check your code.\n");
    }

    return ret;
}

int32_t on_set(const FSM *pfsm, int32_t input, void *parg) {
	(void)input;
	(void)parg;

    ASSERT(pfsm, "Bad argument.\n");

    CONTEXT *pcontext=(CONTEXT *)pfsm->puser_data;
    int32_t ret=STATE_SET_Y_MODE;

    switch(pfsm->state) {
        case STATE_HHMM_MODE:
        case STATE_SS_MODE:
        case STATE_MD_MODE:
        case STATE_Y_MODE:
            CALENDAR_datetime_from_timestamp(RTC_now(NULL),
                                             &pcontext->new_y,
                                             &pcontext->new_m,
                                             &pcontext->new_d,
                                             &pcontext->new_hh,
                                             &pcontext->new_mm,
                                             &pcontext->new_ss);
            NIXIE_BL_INDICATOR_init(g_y_toggle);
            NIXIE_BL_INDICATOR_enable(0);
            NIXIE_BL_INDICATOR_enable(1);
            NIXIE_BL_INDICATOR_enable(2);
            NIXIE_BL_INDICATOR_enable(3);
            update(pcontext, STATE_SET_Y_MODE);
            ret=STATE_SET_Y_MODE;
            break;

        case STATE_SET_Y_MODE:
        	NIXIE_BL_INDICATOR_init(g_md_toggle);
            NIXIE_BL_INDICATOR_disable(0);
            NIXIE_BL_INDICATOR_disable(1);
            NIXIE_BL_INDICATOR_enable(2);
            NIXIE_BL_INDICATOR_enable(3);
            update(pcontext, STATE_SET_M_MODE);
            ret=STATE_SET_M_MODE;
            break;

        case STATE_SET_M_MODE:
        	NIXIE_BL_INDICATOR_init(g_md_toggle);
            NIXIE_BL_INDICATOR_enable(0);
            NIXIE_BL_INDICATOR_enable(1);
            NIXIE_BL_INDICATOR_disable(2);
            NIXIE_BL_INDICATOR_disable(3);
            update(pcontext, STATE_SET_D_MODE);
            ret=STATE_SET_D_MODE;
            break;

        case STATE_SET_D_MODE:
        	NIXIE_BL_INDICATOR_init(g_hhmm_toggle);
            NIXIE_BL_INDICATOR_disable(0);
            NIXIE_BL_INDICATOR_disable(1);
            NIXIE_BL_INDICATOR_enable(2);
            NIXIE_BL_INDICATOR_enable(3);
            update(pcontext, STATE_SET_HH_MODE);
            ret=STATE_SET_HH_MODE;
            break;

        case STATE_SET_HH_MODE:
        	NIXIE_BL_INDICATOR_init(g_hhmm_toggle);
            NIXIE_BL_INDICATOR_enable(0);
            NIXIE_BL_INDICATOR_enable(1);
            NIXIE_BL_INDICATOR_disable(2);
            NIXIE_BL_INDICATOR_disable(3);
            update(pcontext, STATE_SET_MM_MODE);
            ret=STATE_SET_MM_MODE;
            break;

        case STATE_SET_MM_MODE:
        	NIXIE_BL_INDICATOR_init(g_y_toggle);
            NIXIE_BL_INDICATOR_enable(0);
            NIXIE_BL_INDICATOR_enable(1);
            NIXIE_BL_INDICATOR_enable(2);
            NIXIE_BL_INDICATOR_enable(3);
            update(pcontext, STATE_SET_Y_MODE);
            ret=STATE_SET_Y_MODE;
            break;

        default:
            ASSERT(0, "Bad flow. Check your code.\n");
    }

    return ret;
}

int32_t on_nixie_update(const FSM *pfsm, int32_t input, void *parg) {
	(void)input;
	(void)parg;

    ASSERT(pfsm, "Bad argument.\n");

    CONTEXT *pcontext=(CONTEXT *)pfsm->puser_data;
    int32_t ret=pfsm->state;

    update(pcontext, pfsm->state);

    return ret;
}

int32_t on_nixie_bl_update(const FSM *pfsm, int32_t input, void *parg) {
	(void)input;
	(void)parg;

    ASSERT(pfsm, "Bad argument.\n");

    int32_t ret=pfsm->state;

    NEXIE_BL_PAT0_update();

    return ret;
}

int32_t main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;

	CONTEXT context;
	FSM fsm;

	FSM_init(&fsm, STATE_START, (const FSM_STATE_TABLE)g_state_table, &context);
	FSM_run(&fsm, INPUT_INITIALIZE, NULL);

    while(1) {
    	uint32_t ts=TICK_now();
    	int32_t i;

    	//Timestamp overflow handling.
    	for(i=0;i<COUNTOF(context.last_ts);i++) {
    		if(ts<context.last_ts[i])
    			context.last_ts[i]=ts;
    	}

    	//NIXIE digit updating.
    	if(ts-context.last_ts[0]>=context.period[0]) {
    		context.last_ts[0]=ts;
    		FSM_run(&fsm, INPUT_NIXIE_UPDATE, NULL);
    	}

    	//NIXIE refreshing.
    	if(ts-context.last_ts[1]>=context.period[1]) {
    		context.last_ts[1]=ts;
    		NIXIE_refresh();
    	}

    	//Backlight updating.
    	if(ts-context.last_ts[2]>=context.period[2]) {
    		context.last_ts[2]=ts;
    		FSM_run(&fsm, INPUT_NIXIE_BL_UPDATE, NULL);
    	}

    	//Digit indicator toggling.
    	if(ts-context.last_ts[3]>=context.period[3]) {
    		context.last_ts[3]=ts;
    		NIXIE_BL_INDICATOR_toggle();
    	}

    	//NIXIE saver procedure.
    	if(ts-context.last_ts[4]>=context.period[4]) {
    		context.last_ts[4]=ts;
    		NIXIE_SVR_run();
    	}

    	//Remote control command dispatching.
    	if(ts-context.last_ts[5]>=context.period[5]) {
    		context.last_ts[5]=ts;

        	uint8_t data[8], count=0;

			count=IR_pop(data, 256);
			if(count) {
				int32_t i;

				for(i=0;i<count;i++) {
					if(data[i]==INPUT_MUTE ||
					   data[i]==INPUT_PAUSE ||
					   data[i]==INPUT_NEXT ||
					   data[i]==INPUT_PREVIOUS ||
					   data[i]==INPUT_INCREASE ||
					   data[i]==INPUT_DECREASE ||
					   data[i]==INPUT_DISPLAY ||
					   data[i]==INPUT_SET) {
						if(ts-context.last_ts[6]>=context.period[6]) {
							context.last_ts[6]=ts;
							FSM_run(&fsm, data[i], NULL);
						}
						break;
					} else if(data[i]==INPUT_SAVER) {
						if(ts-context.last_ts[6]>=context.period[6]) {
							context.last_ts[6]=ts;
							NIXIE_SVR_run();
						}
						break;
					} else
						;
				}
			}
    	}

    	//BT restoring
    	if(ts-context.last_ts[7]>=context.period[7]) {
    		context.last_ts[7]=ts;
    		BT_restore();
    	}
    }

	return 0;
}
