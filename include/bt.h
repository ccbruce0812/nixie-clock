#ifndef _BT_H_
#define _BT_H_

void BT_init(void);

ALWAYS_INLINE void BT_mute_set(int32_t flag) {
	if(flag)
		GPIO_SetBits(GPIOB, GPIO_Pin_10);
	else
		GPIO_ResetBits(GPIOB, GPIO_Pin_10);
}

ALWAYS_INLINE int32_t BT_mute_get(void) {
	if(GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_10))
		return TRUE;
	return FALSE;
}

ALWAYS_INLINE void BT_reset_set(int32_t flag) {
	if(flag)
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
	else
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

ALWAYS_INLINE int32_t BT_reset_get(void) {
	if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1))
		return FALSE;
	return TRUE;
}

void BT_pause(void);
void BT_increase(void);
void BT_decrease(void);
void BT_next(void);
void BT_previous(void);

void BT_restore(void);

#endif
