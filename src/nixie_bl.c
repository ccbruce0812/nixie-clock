#include "common.h"

typedef struct {
	int32_t toggle;
	int32_t disable[NIXIE_BL_COUNT];
	NIXIE_BL_PIXEL pixel[2][NIXIE_BL_COUNT];
} NIXIE_BL_INDICATOR_DATA;

typedef struct {
	int8_t color[3];
	int8_t round;
	NIXIE_BL_PIXEL pixel[NIXIE_BL_COUNT];
} NIXIE_BL_PAT0_DATA;

static NIXIE_BL_PAT0_DATA g_pat0_data;
static NIXIE_BL_INDICATOR_DATA g_indicator_data;

void NIXIE_BL_INDICATOR_init(const NIXIE_BL_PIXEL pixel[2][NIXIE_BL_COUNT]) {
	int32_t i;

	g_indicator_data.toggle=0;
	for(i=0;i<NIXIE_BL_COUNT;i++)
		g_indicator_data.disable[i]=-1;
	memcpy(g_indicator_data.pixel, pixel, sizeof(g_indicator_data.pixel));
}

void NIXIE_BL_INDICATOR_enable(int32_t idx) {
	g_indicator_data.disable[idx]=FALSE;
}

void NIXIE_BL_INDICATOR_disable(int32_t idx) {
	g_indicator_data.disable[idx]=TRUE;
}

void NIXIE_BL_INDICATOR_toggle(void) {
	g_indicator_data.toggle=(g_indicator_data.toggle+1)%2;
}

ALWAYS_INLINE void update(void) {
    int32_t i;
    NIXIE_BL_PIXEL pixel[NIXIE_BL_COUNT];

    for(i=0;i<NIXIE_BL_COUNT;i++) {
    	if(g_indicator_data.disable[i])
    		memcpy(&pixel[i], &g_pat0_data.pixel[i], sizeof(pixel));
    	else
    		memcpy(&pixel[i], &g_indicator_data.pixel[g_indicator_data.toggle][i], sizeof(pixel));
    }

    NIXIE_BL_update(pixel);
}

void NIXIE_BL_PAT0_init(void) {
    g_pat0_data.color[0]=0;
    g_pat0_data.color[1]=0;
    g_pat0_data.color[2]=31;
    g_pat0_data.round=0;

    int32_t i;

    for(i=1;i<NIXIE_BL_COUNT;i++) {
    	g_pat0_data.pixel[i].prefix=1;
    	g_pat0_data.pixel[i].red=0;
    	g_pat0_data.pixel[i].green=0;
    	g_pat0_data.pixel[i].blue=0;
    }

    g_pat0_data.pixel[0].prefix=1;
    g_pat0_data.pixel[0].red=g_pat0_data.color[0];
    g_pat0_data.pixel[0].green=g_pat0_data.color[1];
    g_pat0_data.pixel[0].blue=g_pat0_data.color[2];

    update();
}

void NEXIE_BL_PAT0_update(void) {
	int8_t i;

	for(i=3; i>=1; i--)
		g_pat0_data.pixel[i]=g_pat0_data.pixel[i-1];

    const int8_t delta[]={1, -1}, max[]={31, 0}, idx[]={0, 2, 1, 0, 2, 1};

    if(g_pat0_data.color[idx[g_pat0_data.round]]!=max[g_pat0_data.round%2])
        g_pat0_data.color[idx[g_pat0_data.round]]+=delta[g_pat0_data.round%2];
    else {
    	g_pat0_data.round=(g_pat0_data.round+1)%6;
    	g_pat0_data.color[idx[g_pat0_data.round]]+=delta[g_pat0_data.round%2];
    }

	g_pat0_data.pixel[0].prefix=1;
	g_pat0_data.pixel[0].red=(uint8_t)g_pat0_data.color[0];
	g_pat0_data.pixel[0].green=(uint8_t)g_pat0_data.color[1];
	g_pat0_data.pixel[0].blue=(uint8_t)g_pat0_data.color[2];

    update();
}
