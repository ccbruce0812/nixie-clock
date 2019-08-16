#ifndef _IR_H_
#define _IR_H_

#define IR_TOLORANCE		(0.1)

#define IR_SIGNATURE_PULSE	(9000)
#define IR_SIGNATURE_PERIOD	(13500)

#define IR_ZERO_PULSE		(560)
#define IR_ZERO_PERIOD		(1120)

#define IR_ONE_PULSE		(560)
#define IR_ONE_PERIOD		(2250)

#define IR_REPEAT_PULSE		(9000)
#define IR_REPEAT_PERIOD	(11250)

#define IR_STOP_PULSE		(560)
#define IR_STOP_PERIOD_LO	(31500)
#define IR_STOP_PERIOD_HI	(49500)

void IR_init(void);

uint32_t IR_pop(uint8_t *pdata, uint32_t size);

#endif
