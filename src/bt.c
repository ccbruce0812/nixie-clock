#include "common.h"

#define PAUSE_PERIOD	(200)
#define INCREASE_PERIOD	(200)
#define DECREASE_PERIOD	(200)
#define NEXT_PERIOD		(200)
#define PREVIOUS_PERIOD	(200)

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;

	uint32_t last_ts;
	uint32_t period;
	int32_t active;
} BT_DATA;

static BT_DATA g_data[]={
	{
		.port=GPIOA,
		.pin=GPIO_Pin_2,
		.last_ts=0,
		.period=PAUSE_PERIOD,
		.active=FALSE
	}, {
		.port=GPIOB,
		.pin=GPIO_Pin_0,
		.last_ts=0,
		.period=INCREASE_PERIOD,
		.active=FALSE
	}, {
		.port=GPIOB,
		.pin=GPIO_Pin_1,
		.last_ts=0,
		.period=DECREASE_PERIOD,
		.active=FALSE
	}, {
		.port=GPIOB,
		.pin=GPIO_Pin_8,
		.last_ts=0,
		.period=NEXT_PERIOD,
		.active=FALSE
	}, {
		.port=GPIOB,
		.pin=GPIO_Pin_9,
		.last_ts=0,
		.period=PREVIOUS_PERIOD,
		.active=FALSE
	}
};

void BT_init(void) {
	RCC_APB2PeriphClockCmd(
		RCC_APB2Periph_GPIOA|
		RCC_APB2Periph_GPIOB,
		ENABLE);

	GPIO_InitTypeDef GPIO_init;

	//Mute (PB10)
	GPIO_init.GPIO_Pin = GPIO_Pin_10;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_init);

	//Reset (PA1)
	GPIO_init.GPIO_Pin = GPIO_Pin_1;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_init);

	//Pause (PA2)
	GPIO_init.GPIO_Pin = GPIO_Pin_2;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_init);

	//Increase (PB0)
	GPIO_init.GPIO_Pin = GPIO_Pin_0;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_init);

	//Decrease (PB1)
	GPIO_init.GPIO_Pin = GPIO_Pin_1;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_init);

	//Next (PB8)
	GPIO_init.GPIO_Pin = GPIO_Pin_8;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_init);

	//Previous (PB9)
	GPIO_init.GPIO_Pin = GPIO_Pin_9;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_init);

	//No reset
	GPIO_SetBits(GPIOA, GPIO_Pin_1);

	//No mute
	GPIO_ResetBits(GPIOB, GPIO_Pin_10);
}

void BT_pause(void) {
	GPIO_SetBits(GPIOA, GPIO_Pin_2);
	g_data[0].last_ts=TICK_now();
	g_data[0].active=TRUE;
}

void BT_increase(void) {
	GPIO_SetBits(GPIOB, GPIO_Pin_0);
	g_data[1].last_ts=TICK_now();
	g_data[1].active=TRUE;
}

void BT_decrease(void) {
	GPIO_SetBits(GPIOB, GPIO_Pin_1);
	g_data[2].last_ts=TICK_now();
	g_data[2].active=TRUE;
}

void BT_next(void) {
	GPIO_SetBits(GPIOB, GPIO_Pin_8);
	g_data[3].last_ts=TICK_now();
	g_data[3].active=TRUE;
}

void BT_previous(void) {
	GPIO_SetBits(GPIOB, GPIO_Pin_9);
	g_data[4].last_ts=TICK_now();
	g_data[4].active=TRUE;
}

void BT_restore(void) {
	uint32_t ts=TICK_now();
	int32_t i;

	for(i=0;i<COUNTOF(g_data);i++) {
		if(g_data[i].active) {
			if(ts<g_data[i].last_ts)
				g_data[i].last_ts=ts;

			if(ts-g_data[i].last_ts>=g_data[i].period) {
				GPIO_ResetBits(g_data[i].port, g_data[i].pin);
				g_data[i].active=FALSE;
			}
		}
	}
}
