#ifndef _NIXIE_PWR_H_
#define _NIXIE_PWR_H_

inline static __attribute__((always_inline)) void NIXIE_PWR_init(void) {
	RCC_APB2PeriphClockCmd(
		RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_init;

	GPIO_init.GPIO_Pin = GPIO_Pin_11;
	GPIO_init.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_init.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_init);
}

inline static __attribute__((always_inline))  void NIXIE_PWR_on(void) {
	GPIO_SetBits(GPIOB, GPIO_Pin_11);
}

inline static __attribute__((always_inline))  void NIXIE_PWR_off(void) {
	GPIO_ResetBits(GPIOB, GPIO_Pin_11);
}

#endif
