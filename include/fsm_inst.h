#ifndef _FSM_INST_H_
#define _FSM_INST_H_

enum {
    STATE_START=0,
    STATE_HHMM_MODE,
    STATE_SS_MODE,
	STATE_MD_MODE,
	STATE_Y_MODE,
	STATE_SET_Y_MODE,
	STATE_SET_M_MODE,
	STATE_SET_D_MODE,
	STATE_SET_HH_MODE,
	STATE_SET_MM_MODE
};

enum {
	INPUT_INITIALIZE=		0x0100,
	INPUT_NIXIE_UPDATE=		0x0101,
	INPUT_NIXIE_BL_UPDATE=	0x0102,
	INPUT_MUTE=				0x09,
	INPUT_NEXT=				0x40,
	INPUT_PREVIOUS=			0x44,
	INPUT_INCREASE=			0x15,
	INPUT_DECREASE=			0x07,
    INPUT_DISPLAY=			0x45,
	INPUT_SET=				0x47,
	INPUT_SAVER=			0x46
};

extern const FSM_STATE_ITEM g_state_table[13];

int32_t on_initialize(const FSM *pfsm, int32_t input, void *parg);
int32_t on_mute(const FSM *pfsm, int32_t input, void *parg);
int32_t on_next(const FSM *pfsm, int32_t input, void *parg);
int32_t on_previous(const FSM *pfsm, int32_t input, void *parg);
int32_t on_increase(const FSM *pfsm, int32_t input, void *parg);
int32_t on_decrease(const FSM *pfsm, int32_t input, void *parg);
int32_t on_display(const FSM *pfsm, int32_t input, void *parg);
int32_t on_set(const FSM *pfsm, int32_t input, void *parg);
int32_t on_nixie_update(const FSM *pfsm, int32_t input, void *parg);
int32_t on_nixie_bl_update(const FSM *pfsm, int32_t input, void *parg);

#endif
