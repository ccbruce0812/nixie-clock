#include "common.h"

const static uint8_t dot_map[NIXIE_DIGIT_COUNT]={0, 1, 2, 3};

static uint8_t g_digit;
static uint8_t g_number[NIXIE_DIGIT_COUNT];

void NIXIE_init(void) {
	RCC_APB2PeriphClockCmd(
		RCC_APB2Periph_GPIOA |
		RCC_APB2Periph_SPI1, ENABLE);

	GPIO_InitTypeDef GPIO_init;

	GPIO_init.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOA, &GPIO_init);

	GPIO_init.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(GPIOA, &GPIO_init);

	SPI_InitTypeDef SPI_init;

	SPI_init.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_init.SPI_Mode = SPI_Mode_Master;
	SPI_init.SPI_DataSize = SPI_DataSize_8b;
	SPI_init.SPI_CPOL = SPI_CPOL_Low;
	SPI_init.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_init.SPI_NSS = SPI_NSS_Soft;
	SPI_init.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_init.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_init.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_init);

	SPI_Cmd(SPI1, ENABLE);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

    NIXIE_reset();
    NIXIE_on();
}

void NIXIE_reset(void) {
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    TICK_usleep(1);
    GPIO_SetBits(GPIOA, GPIO_Pin_2);

    g_digit=0;
    memset(g_number, 0, sizeof(g_number));
}

void NIXIE_update(uint8_t digit, uint8_t number) {
	ASSERT(digit<NIXIE_DIGIT_COUNT && (number&0x0f)<=NIXIE_DARK, "Bad argument.\n");

	g_number[digit]=number;
}

ALWAYS_INLINE void blanking(void) {
	uint16_t data=0x7ff0;
	uint8_t *pdata=(uint8_t *)&data;
	int16_t i;

	GPIO_ResetBits(GPIOA, GPIO_Pin_4);

	for(i=sizeof(data)-1;i>=0;i--) {
		SPI_I2S_SendData(SPI1, pdata[i]);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	}
	TICK_usleep(1);

	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);

	TICK_usleep(100);
}


void NIXIE_refresh(void) {
	uint16_t data=0;
	uint8_t *pdata=(uint8_t *)&data;
	int16_t shift=NIXIE_DIGIT_COUNT, i;

	if(g_number[g_digit]&0x0f==NIXIE_DARK)
		data|=(0x1<<g_digit);
	else {
		shift+=((g_number[g_digit]&0x0f)+1);
		data|=(0x1<<g_digit);
		data|=(0x1<<shift);
	}

	if(g_number[g_digit]&0xf0)
		data|=(0x1<<NIXIE_DIGIT_COUNT);

	blanking();

	GPIO_ResetBits(GPIOA, GPIO_Pin_4);

	for(i=sizeof(data)-1;i>=0;i--) {
		SPI_I2S_SendData(SPI1, pdata[i]);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	}
	TICK_usleep(1);

	GPIO_SetBits(GPIOA, GPIO_Pin_4);
	GPIO_ResetBits(GPIOA, GPIO_Pin_4);

	TICK_usleep(40);

	g_digit=(g_digit+1)%NIXIE_DIGIT_COUNT;
}
