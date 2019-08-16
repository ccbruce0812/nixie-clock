#include "common.h"

const FSM_STATE_ITEM state_table[13]={
    {
        .id=STATE_START,
        .action_table=(FSM_ACTION_ITEM[]) {
            {.input=INPUT_INITIALIZE,		.action=on_initialize},
            {.input=FSM_ACTION_BAD_INPUT}
        }
    }, {
		.id=STATE_HHMM_MODE,
		.action_table=(FSM_ACTION_ITEM[]) {
			{.input=INPUT_MUTE,				.action=on_mute},
			{.input=INPUT_NEXT,				.action=on_next},
			{.input=INPUT_PREVIOUS,			.action=on_previous},
			{.input=INPUT_INCREASE,			.action=on_increase},
			{.input=INPUT_DECREASE,			.action=on_decrease},
			{.input=INPUT_DISPLAY,			.action=on_display},
			{.input=INPUT_SET,				.action=on_set},
			{.input=INPUT_NIXIE_UPDATE,		.action=on_nixie_update},
			{.input=INPUT_NIXIE_BL_UPDATE,	.action=on_nixie_bl_update},
			{.input=FSM_ACTION_BAD_INPUT}
		}
	}, {
		.id=STATE_SS_MODE,
		.action_table=(FSM_ACTION_ITEM[]) {
			{.input=INPUT_MUTE,				.action=on_mute},
			{.input=INPUT_NEXT,				.action=on_next},
			{.input=INPUT_PREVIOUS,			.action=on_previous},
			{.input=INPUT_INCREASE,			.action=on_increase},
			{.input=INPUT_DECREASE,			.action=on_decrease},
			{.input=INPUT_DISPLAY,			.action=on_display},
			{.input=INPUT_SET,				.action=on_set},
			{.input=INPUT_NIXIE_UPDATE,		.action=on_nixie_update},
			{.input=INPUT_NIXIE_BL_UPDATE,	.action=on_nixie_bl_update},
			{.input=FSM_ACTION_BAD_INPUT}
		}
	}, {
		.id=STATE_MD_MODE,
		.action_table=(FSM_ACTION_ITEM[]) {
			{.input=INPUT_MUTE,				.action=on_mute},
			{.input=INPUT_NEXT,				.action=on_next},
			{.input=INPUT_PREVIOUS,			.action=on_previous},
			{.input=INPUT_INCREASE,			.action=on_increase},
			{.input=INPUT_DECREASE,			.action=on_decrease},
			{.input=INPUT_DISPLAY,			.action=on_display},
			{.input=INPUT_SET,				.action=on_set},
			{.input=INPUT_NIXIE_UPDATE,		.action=on_nixie_update},
			{.input=INPUT_NIXIE_BL_UPDATE,	.action=on_nixie_bl_update},
			{.input=FSM_ACTION_BAD_INPUT}
		}
	}, {
		.id=STATE_Y_MODE,
		.action_table=(FSM_ACTION_ITEM[]) {
			{.input=INPUT_MUTE,				.action=on_mute},
			{.input=INPUT_NEXT,				.action=on_next},
			{.input=INPUT_PREVIOUS,			.action=on_previous},
			{.input=INPUT_INCREASE,			.action=on_increase},
			{.input=INPUT_DECREASE,			.action=on_decrease},
			{.input=INPUT_DISPLAY,			.action=on_display},
			{.input=INPUT_SET,				.action=on_set},
			{.input=INPUT_NIXIE_UPDATE,		.action=on_nixie_update},
			{.input=INPUT_NIXIE_BL_UPDATE,	.action=on_nixie_bl_update},
			{.input=FSM_ACTION_BAD_INPUT}
		}
	}, {
		.id=STATE_SET_MM_MODE,
		.action_table=(FSM_ACTION_ITEM[]) {
			{.input=INPUT_MUTE,				.action=on_mute},
			{.input=INPUT_NEXT,				.action=on_next},
			{.input=INPUT_PREVIOUS,			.action=on_previous},
			{.input=INPUT_INCREASE,			.action=on_increase},
			{.input=INPUT_DECREASE,			.action=on_decrease},
			{.input=INPUT_DISPLAY,			.action=on_display},
			{.input=INPUT_SET,				.action=on_set},
			{.input=INPUT_NIXIE_UPDATE,		.action=on_nixie_update},
			{.input=INPUT_NIXIE_BL_UPDATE,	.action=on_nixie_bl_update},
			{.input=FSM_ACTION_BAD_INPUT}
		}
	}, {
		.id=STATE_SET_HH_MODE,
		.action_table=(FSM_ACTION_ITEM[]) {
			{.input=INPUT_MUTE,				.action=on_mute},
			{.input=INPUT_NEXT,				.action=on_next},
			{.input=INPUT_PREVIOUS,			.action=on_previous},
			{.input=INPUT_INCREASE,			.action=on_increase},
			{.input=INPUT_DECREASE,			.action=on_decrease},
			{.input=INPUT_DISPLAY,			.action=on_display},
			{.input=INPUT_SET,				.action=on_set},
			{.input=INPUT_NIXIE_UPDATE,		.action=on_nixie_update},
			{.input=INPUT_NIXIE_BL_UPDATE,	.action=on_nixie_bl_update},
			{.input=FSM_ACTION_BAD_INPUT}
		}
	}, {
		.id=STATE_SET_D_MODE,
		.action_table=(FSM_ACTION_ITEM[]) {
			{.input=INPUT_MUTE,				.action=on_mute},
			{.input=INPUT_NEXT,				.action=on_next},
			{.input=INPUT_PREVIOUS,			.action=on_previous},
			{.input=INPUT_INCREASE,			.action=on_increase},
			{.input=INPUT_DECREASE,			.action=on_decrease},
			{.input=INPUT_DISPLAY,			.action=on_display},
			{.input=INPUT_SET,				.action=on_set},
			{.input=INPUT_NIXIE_UPDATE,		.action=on_nixie_update},
			{.input=INPUT_NIXIE_BL_UPDATE,	.action=on_nixie_bl_update},
			{.input=FSM_ACTION_BAD_INPUT}
		}
	}, {
		.id=STATE_SET_M_MODE,
		.action_table=(FSM_ACTION_ITEM[]) {
			{.input=INPUT_MUTE,				.action=on_mute},
			{.input=INPUT_NEXT,				.action=on_next},
			{.input=INPUT_PREVIOUS,			.action=on_previous},
			{.input=INPUT_INCREASE,			.action=on_increase},
			{.input=INPUT_DECREASE,			.action=on_decrease},
			{.input=INPUT_DISPLAY,			.action=on_display},
			{.input=INPUT_SET,				.action=on_set},
			{.input=INPUT_NIXIE_UPDATE,		.action=on_nixie_update},
			{.input=INPUT_NIXIE_BL_UPDATE,	.action=on_nixie_bl_update},
			{.input=FSM_ACTION_BAD_INPUT}
		}
	}, {
		.id=STATE_SET_Y_MODE,
		.action_table=(FSM_ACTION_ITEM[]) {
			{.input=INPUT_MUTE,				.action=on_mute},
			{.input=INPUT_NEXT,				.action=on_next},
			{.input=INPUT_PREVIOUS,			.action=on_previous},
			{.input=INPUT_INCREASE,			.action=on_increase},
			{.input=INPUT_DECREASE,			.action=on_decrease},
			{.input=INPUT_DISPLAY,			.action=on_display},
			{.input=INPUT_SET,				.action=on_set},
			{.input=INPUT_NIXIE_UPDATE,		.action=on_nixie_update},
			{.input=INPUT_NIXIE_BL_UPDATE,	.action=on_nixie_bl_update},
			{.input=FSM_ACTION_BAD_INPUT}
		}
	}, {
        .id=FSM_STATE_BAD_ID
    }
};
