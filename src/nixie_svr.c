#include "common.h"

const int32_t g_pattern[7][4]={
    {TRUE , FALSE, FALSE, FALSE},
    {TRUE , TRUE , FALSE, FALSE},
    {TRUE , TRUE , TRUE , FALSE},
    {TRUE , TRUE , TRUE , TRUE },
    {FALSE, TRUE , TRUE , TRUE },
    {FALSE, FALSE, TRUE , TRUE },
    {FALSE, FALSE, FALSE, TRUE },
};

void NIXIE_SVR_run(void) {
    uint32_t last_ts[]={TICK_now(), TICK_now()};
    int8_t i, j, k=0;

    for(i=0;i<7;) {
        uint32_t ts=TICK_now();

        for(j=0;j<COUNTOF(last_ts);j++) {
        	if(ts<last_ts[j])
        		last_ts[j]=ts;
        }

        if(ts-last_ts[0]>=10) {
            last_ts[0]=ts;

            for(j=0;j<4;j++) {
                if(g_pattern[i][j])
                    NIXIE_update(j, k);
            }
            k=(k+1)%10;
        }

        NIXIE_refresh();

        if(ts-last_ts[1]>=1000) {
            last_ts[1]=ts;
            i++;
        }
    }
}
