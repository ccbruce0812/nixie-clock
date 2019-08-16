#ifndef _UART_H_
#define _UART_H_

#define UART_BUF_MAX_SIZE	(64)
#define UART_BAUD_RATE		(115200)
#define UART_WORD_LENGTH	(USART_WordLength_8b)
#define UART_STOP_BITS		(USART_StopBits_1)
#define UART_PARITY			(USART_Parity_No)

static inline __attribute__((always_inline)) void UART_init(void) {
	NVIC_InitTypeDef NVIC_init_data;
	GPIO_InitTypeDef GPIO_init_data;
	USART_InitTypeDef USART_init_data;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

	NVIC_init_data.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_init_data.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_init_data.NVIC_IRQChannelSubPriority = 0;
	NVIC_init_data.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_init_data);

	NVIC_init_data.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_init_data.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_init_data.NVIC_IRQChannelSubPriority = 0;
	NVIC_init_data.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_init_data);

	NVIC_init_data.NVIC_IRQChannel = USART1_IRQn;
	NVIC_init_data.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_init_data.NVIC_IRQChannelSubPriority = 0;
	NVIC_init_data.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_init_data);

	GPIO_init_data.GPIO_Pin = GPIO_Pin_9;
	GPIO_init_data.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init_data.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_init_data);

	GPIO_init_data.GPIO_Pin = GPIO_Pin_10;
	GPIO_init_data.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_init_data);

	USART_init_data.USART_BaudRate = UART_BAUD_RATE;
	USART_init_data.USART_WordLength = UART_WORD_LENGTH;
	USART_init_data.USART_StopBits = UART_STOP_BITS;
	USART_init_data.USART_Parity = UART_PARITY;
	USART_init_data.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_init_data.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_init_data);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_DMACmd(USART1, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

int32_t UART_puts(const char *str);
int32_t UART_gets(const char **pstr);

#endif
