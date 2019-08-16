#ifndef _NIXIE_H_
#define _NIXIE_H_

#define NIXIE_COUNT	(4)
#define NIXIE_DOT	(10)
#define NIXIE_DARK	(11)

void NIXIE_init(void);
void NIXIE_reset(void);
void NIXIE_update(uint8_t digit, uint8_t number);
void NIXIE_refresh(void);

inline static __attribute__((always_inline)) void NIXIE_on(void) {
    GPIO_ResetBits(GPIOA, GPIO_Pin_3);
}

inline static __attribute__((always_inline)) void NIXIE_off(void) {
    GPIO_SetBits(GPIOA, GPIO_Pin_3);
}

#endif
