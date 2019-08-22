#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "diag/Trace.h"

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "misc.h"

#define COUNTOF(arr)	(sizeof(arr)/sizeof(arr[0]))
#define ALWAYS_INLINE	static inline __attribute__((always_inline))
#define TRUE			(-1)
#define FALSE			(0)

static inline void __msg(const char *loc, const char *fmt, ...) {
	va_list args;
	char str[1024], line[1024];

	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);

	sprintf(line, "[%s] %s", loc, str);
	trace_printf(line);
}

#define MSG(...) __msg(__func__, __VA_ARGS__)

#ifndef NDEBUG
	#define ASSERT(expr, msg) \
		if(!(expr)) { \
			MSG(msg); \
			assert(0); \
		}
#else
	#define ASSERT(expr, msg)
#endif

#include "fsm.h"
#include "fsm_inst.h"

#include "calendar.h"

#include "tick.h"
#include "rtc.h"
#include "ir.h"
#include "nixie.h"
#include "nixie_pwr.h"
#include "nixie_bl.h"
#include "nixie_svr.h"
#include "bt.h"

#include "uart.h"

#endif
