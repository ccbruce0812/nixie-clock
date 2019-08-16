#ifndef _NIXIE_BL_H_
#define _NIXIE_BL_H_

#define NIXIE_BL_COUNT	(4)

typedef union {
	struct {
		uint16_t red	:	5;
		uint16_t green	:	5;
		uint16_t blue	:	5;
		uint16_t prefix	:	1;
	};
	uint16_t data;
} NIXIE_BL_PIXEL;

ALWAYS_INLINE void NIXIE_BL_init(void) {
	RCC_APB2PeriphClockCmd(
		RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_init;

	GPIO_init.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOB, &GPIO_init);
}

ALWAYS_INLINE void NIXIE_BL_update(const NIXIE_BL_PIXEL pixels[NIXIE_BL_COUNT]) {
	int16_t i, j;

	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	for(i=0;i<32;i++) {
		GPIO_SetBits(GPIOB, GPIO_Pin_13);
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	}

	for(i=0;i<NIXIE_BL_COUNT;i++) {
		uint16_t mask=0x8000;

		for(j=0;j<16;j++) {
			if(!j || (pixels[i].data&mask))
				GPIO_SetBits(GPIOB, GPIO_Pin_15);
			else
				GPIO_ResetBits(GPIOB, GPIO_Pin_15);
			GPIO_SetBits(GPIOB, GPIO_Pin_13);
			GPIO_ResetBits(GPIOB, GPIO_Pin_13);
			mask>>=1;
    	}
	}

	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
	for(i=0;i<NIXIE_BL_COUNT;i++) {
		GPIO_SetBits(GPIOB, GPIO_Pin_13);
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
	}
}

void NIXIE_BL_INDICATOR_init(const NIXIE_BL_PIXEL pixel[2][NIXIE_BL_COUNT]);
void NIXIE_BL_INDICATOR_enable(int32_t idx);
void NIXIE_BL_INDICATOR_disable(int32_t idx);
void NIXIE_BL_INDICATOR_toggle(void);

void NIXIE_BL_PAT0_init(void);
void NEXIE_BL_PAT0_update(void);

#endif
