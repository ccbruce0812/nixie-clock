#ifndef _NIXIE_H_
#define _NIXIE_H_

#define NIXIE_DIGIT_COUNT	(4)

#define NIXIE_DOT	(0xf0)
#define NIXIE_DARK	(10)

void NIXIE_init(void);
void NIXIE_reset(void);
void NIXIE_update(uint8_t digit, uint8_t number);
void NIXIE_refresh(void);

ALWAYS_INLINE void NIXIE_on(void) {
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

ALWAYS_INLINE void NIXIE_off(void) {
    GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

#endif
