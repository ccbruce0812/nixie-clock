#include "common.h"

enum {
	PATTERN_SIGNATURE=0,
	PATTERN_ZERO,
	PATTERN_ONE,
	PATTERN_REPEAT,
	PATTERN_STOP,
	PATTERN_MAX
};

uint8_t g_ringbuf[64][4];
uint8_t g_rptr=0, g_wptr=0;
uint8_t g_count=0;

ALWAYS_INLINE int32_t about(uint16_t a, uint16_t b) {
	uint16_t u, l;

	u=a+a*IR_TOLORANCE;
	l=a-a*IR_TOLORANCE;
	if(b>=l && b<u)
		return -1;
	return 0;
}

ALWAYS_INLINE inline uint8_t parse(uint16_t pulse, uint16_t period) {
	if(about(IR_SIGNATURE_PULSE, pulse) &&
	   about(IR_SIGNATURE_PERIOD, period))
		goto signature;

	if(about(IR_ZERO_PULSE, pulse) &&
	   about(IR_ZERO_PERIOD, period))
		goto zero;

	if(about(IR_ONE_PULSE, pulse) &&
	   about(IR_ONE_PERIOD, period))
		goto one;

	if(about(IR_REPEAT_PULSE, pulse) &&
	   about(IR_REPEAT_PERIOD, period))
		goto repeat;

	if(about(IR_STOP_PULSE, pulse) &&
	   period>=(IR_STOP_PERIOD_LO-IR_STOP_PERIOD_LO*IR_TOLORANCE) &&
	   period<=(IR_STOP_PERIOD_HI+IR_STOP_PERIOD_HI*IR_TOLORANCE))
		goto stop;

	return PATTERN_MAX;

signature:
	return PATTERN_SIGNATURE;

zero:
	return PATTERN_ZERO;

one:
	return PATTERN_ONE;

repeat:
	return PATTERN_REPEAT;

stop:
	return PATTERN_STOP;
}

ALWAYS_INLINE void push(uint8_t *pdata) {
	if(!pdata) {
		MSG("Bad argument.\n");
		assert(0);
	}

	if(g_count==64) {
		g_rptr=(g_rptr+1)%64;
		g_count--;
	}

	memcpy(g_ringbuf[g_wptr], pdata, sizeof(uint8_t)*4);
	g_wptr=(g_wptr+1)%64;
	g_count++;
}

void TIM2_IRQHandler(void) {
	static uint8_t data[4]={0}, last_data[4]={0};
	static uint8_t count=0;

	uint32_t tmp, period, pulse;
	uint8_t res;

	tmp=TIM_GetCapture1(TIM2);
	period=1000*tmp/(SystemCoreClock/1000/128);

	tmp=TIM_GetCapture2(TIM2);
	pulse=1000*tmp/(SystemCoreClock/1000/128);

	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

	res=parse(pulse, period);
	switch(res) {
		case PATTERN_ZERO:
		case PATTERN_ONE: {
			uint8_t off=count%8,
					idx=count/8;

			if(res==PATTERN_ONE)
				data[idx]|=(((uint8_t)0x1)<<off);

			if(++count==32) {
				push(data);
				memcpy(last_data, data, sizeof(data));
				count=0;
				memset(data, 0, sizeof(data));
			}
			break;
		}

		case PATTERN_REPEAT:
			push(last_data);
			break;

		case PATTERN_SIGNATURE:
		case PATTERN_STOP:
		default:
			count=0;
			memset(data, 0, sizeof(data));
	}
}

void IR_init(void) {
	GPIO_InitTypeDef gpio_init_data;
	TIM_TimeBaseInitTypeDef tim_timebaseinit_data;
	TIM_ICInitTypeDef tim_icinit_data;
	NVIC_InitTypeDef nvic_init_data;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	gpio_init_data.GPIO_Pin = GPIO_Pin_0;
	gpio_init_data.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_init_data.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio_init_data);

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	tim_timebaseinit_data.TIM_Prescaler=127;
	tim_timebaseinit_data.TIM_CounterMode=TIM_CounterMode_Up;
	tim_timebaseinit_data.TIM_Period=65535;
	tim_timebaseinit_data.TIM_ClockDivision=TIM_CKD_DIV1;
	tim_timebaseinit_data.TIM_RepetitionCounter=0;
	TIM_TimeBaseInit(TIM2, &tim_timebaseinit_data);

	tim_icinit_data.TIM_Channel = TIM_Channel_1;
	tim_icinit_data.TIM_ICPolarity = TIM_ICPolarity_Falling;
	tim_icinit_data.TIM_ICSelection = TIM_ICSelection_DirectTI;
	tim_icinit_data.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	tim_icinit_data.TIM_ICFilter = 0x0;
	TIM_PWMIConfig(TIM2, &tim_icinit_data);

	TIM_SelectInputTrigger(TIM2, TIM_TS_TI1FP1);
	TIM_SelectSlaveMode(TIM2, TIM_SlaveMode_Reset);
	TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);

	TIM_Cmd(TIM2, ENABLE);

	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);

	nvic_init_data.NVIC_IRQChannel = TIM2_IRQn;
	nvic_init_data.NVIC_IRQChannelPreemptionPriority = 0;
	nvic_init_data.NVIC_IRQChannelSubPriority = 1;
	nvic_init_data.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init_data);
}

uint32_t IR_pop(uint8_t *pdata, uint32_t size) {
	if(!pdata) {
		MSG("Bad argument.\n");
		assert(0);
	}

	uint32_t ret=0;

	TIM_ITConfig(TIM2, TIM_IT_CC1, DISABLE);

	if(!g_count)
		goto no_data;

	while(g_count && size) {
		//memcpy(pdata[ret++], g_ringbuf[g_rptr], sizeof(uint8_t)*4);
		pdata[ret++]=g_ringbuf[g_rptr][2];
		g_rptr=(g_rptr+1)%64;
		g_count--;
		size--;
	}

	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	return ret;

no_data:
	TIM_ITConfig(TIM2, TIM_IT_CC1, ENABLE);
	return ret;
}
