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
	INPUT_INITIALIZE=0,
	INPUT_MUTE=1,
	INPUT_NEXT=2,
	INPUT_PREVIOUS=3,
	INPUT_INCREASE=0x09,
	INPUT_DECREASE=0x1b,
    INPUT_DISPLAY=0x07,
	INPUT_SET=0x08,
	INPUT_NIXIE_UPDATE=4,
	INPUT_NIXIE_BL_UPDATE=5
};

extern const FSM_STATE_ITEM state_table[13];

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
