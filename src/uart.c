#include "common.h"

typedef struct {
	char buf[UART_BUF_MAX_SIZE];
	uint32_t size;
} UART_BUF;

enum {
	RX_STATE_IDLE=0,
	RX_STATE_RECEIVING,
	RX_STATE_COMPLETE
};

UART_BUF g_tx_backend[2];
UART_BUF *g_ptx_buf_dma=&g_tx_backend[0];
UART_BUF *g_ptx_buf_user=&g_tx_backend[1];

UART_BUF g_rx_backend[2];
UART_BUF *g_prx_buf_dma=&g_rx_backend[0];
UART_BUF *g_prx_buf_user=&g_rx_backend[1];

uint8_t g_rx_state=RX_STATE_IDLE;

ALWAYS_INLINE void wait_until_tx_idle(void) {
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
}

ALWAYS_INLINE void swap_tx_buf(void) {
	UART_BUF *ptmp=g_ptx_buf_user;

	g_ptx_buf_user=g_ptx_buf_dma;
	g_ptx_buf_dma=ptmp;
}

ALWAYS_INLINE void swap_rx_buf(void) {
	UART_BUF *ptmp=g_prx_buf_user;

	g_prx_buf_user=g_prx_buf_dma;
	g_prx_buf_dma=ptmp;
}

ALWAYS_INLINE void flush(void) {
	if(!g_ptx_buf_user->size)
		return;

	DMA_InitTypeDef DMA_init_data;

	wait_until_tx_idle();
	swap_tx_buf();

	DMA_init_data.DMA_PeripheralBaseAddr = (uint32_t) &USART1->DR;
	DMA_init_data.DMA_MemoryBaseAddr = (uint32_t) g_ptx_buf_dma;
	DMA_init_data.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_init_data.DMA_BufferSize = g_ptx_buf_dma->size;
	DMA_init_data.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_init_data.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_init_data.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_init_data.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_init_data.DMA_Mode = DMA_Mode_Normal;
	DMA_init_data.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_init_data.DMA_M2M = DMA_M2M_Disable;

	DMA_DeInit(DMA1_Channel4);
	DMA_Init(DMA1_Channel4, &DMA_init_data);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel4, ENABLE);
}

void DMA1_Channel4_IRQHandler(void) {
	if(DMA_GetITStatus(DMA1_IT_TC4)==SET) {
		DMA_ClearITPendingBit(DMA1_IT_TC4);
		g_ptx_buf_dma->size=0;
	}
}

void DMA1_Channel5_IRQHandler(void) {
	if(DMA_GetITStatus(DMA1_IT_TC5)==SET) {
		DMA_ClearITPendingBit(DMA1_IT_TC5);
		g_prx_buf_dma->size=UART_BUF_MAX_SIZE;
		g_rx_state=RX_STATE_COMPLETE;
	}
}

void USART1_IRQHandler(void) {
	if(USART_GetITStatus(USART1, USART_IT_RXNE)==SET) {
		DMA_InitTypeDef DMA_init_data;

		DMA_init_data.DMA_PeripheralBaseAddr = (uint32_t) &USART1->DR;
		DMA_init_data.DMA_MemoryBaseAddr = (uint32_t) &g_rx_backend[0];
		DMA_init_data.DMA_DIR = DMA_DIR_PeripheralSRC;
		DMA_init_data.DMA_BufferSize = UART_BUF_MAX_SIZE;
		DMA_init_data.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_init_data.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_init_data.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
		DMA_init_data.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
		DMA_init_data.DMA_Mode = DMA_Mode_Normal;
		DMA_init_data.DMA_Priority = DMA_Priority_VeryHigh;
		DMA_init_data.DMA_M2M = DMA_M2M_Disable;

		DMA_DeInit(DMA1_Channel5);
		DMA_Init(DMA1_Channel5, &DMA_init_data);
		DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
		DMA_Cmd(DMA1_Channel5, ENABLE);

		g_rx_state=RX_STATE_RECEIVING;
	}
}

int32_t UART_puts(const char *str) {
	if(!str || !strlen(str))
		return 0;

	uint32_t i;
	int32_t ret=0;

	for(i=0;str[i];) {
		if(g_ptx_buf_user->size>=UART_BUF_MAX_SIZE)
			flush();

		g_ptx_buf_user->buf[g_ptx_buf_user->size]=str[i];
		g_ptx_buf_user->size++;
		i++;
		ret++;
	}
	flush();

	return ret;
}

int32_t UART_gets(const char **pstr) {
	static char buf[UART_BUF_MAX_SIZE+1];
	uint32_t i=0;

	switch(g_rx_state) {
		case RX_STATE_RECEIVING: {
			uint32_t left;

			DMA_Cmd(DMA1_Channel5, DISABLE);
			left=UART_BUF_MAX_SIZE-DMA_GetCurrDataCounter(DMA1_Channel5);
			if(left>0) {
				swap_rx_buf();
				g_prx_buf_user->size=left;
				memcpy(&buf[i], g_prx_buf_user->buf, g_prx_buf_user->size*sizeof(buf[0]));
				i=g_prx_buf_user->size;
				g_prx_buf_user->size=0;
			}
			g_rx_state=RX_STATE_IDLE;
			break;
		}

		case RX_STATE_COMPLETE:
			swap_rx_buf();
			memcpy(&buf[i], g_prx_buf_user->buf, g_prx_buf_user->size*sizeof(buf[0]));
			i=g_prx_buf_user->size;
			g_prx_buf_user->size=0;
			g_rx_state=RX_STATE_IDLE;
			break;

		default:
			;
	}

	buf[i]='\0';
	*pstr=buf;
	return i;
}
