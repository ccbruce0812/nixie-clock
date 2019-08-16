#ifndef _FSM_H_
#define _FSM_H_

#define FSM_STATE_BAD_ID                (-1)
#define FSM_ACTION_BAD_INPUT            (-1)

#define FSM_ERROR_UNACCEPTABLE_INPUT    (-1)
#define FSM_ERROR_BAD_STATE             (-2)
#define FSM_ERROR_BAD_NEXT_STATE        (-3)

struct _FSM;

typedef int32_t (*FSM_ACTION)(const struct _FSM *pfsm, int32_t input, void *parg);

typedef struct {
    int32_t input;
    FSM_ACTION action;
} FSM_ACTION_ITEM, *FSM_ACTION_TABLE;

typedef struct {
    int32_t id;
    FSM_ACTION_TABLE action_table;
} FSM_STATE_ITEM, *FSM_STATE_TABLE;

typedef struct _FSM {
    int32_t state;
    FSM_STATE_TABLE state_table;
    void *puser_data;
} FSM;

ALWAYS_INLINE int32_t FSM_init(FSM *pfsm, int32_t init_state, const FSM_STATE_TABLE state_table, void *puser_data) {
    ASSERT(pfsm && state_table, "Bad argument.\n");

    int32_t ret=0, i;

    for(i=0;state_table[i].id!=FSM_STATE_BAD_ID && state_table[i].id!=init_state;i++);
    if(state_table[i].id!=FSM_STATE_BAD_ID) {
        pfsm->state=init_state;
        pfsm->state_table=state_table;
        pfsm->puser_data=puser_data;
    } else
        ret=FSM_ERROR_BAD_STATE;

    return ret;
}

ALWAYS_INLINE int32_t FSM_run(FSM *pfsm, int32_t input, void *parg) {
    ASSERT(pfsm, "Bad argument.\n");

    int32_t ret=0, i;

    ASSERT(pfsm->state_table, "Null state table.\n");

    int32_t state=pfsm->state;
    FSM_STATE_TABLE state_table=pfsm->state_table;

    for(i=0;state_table[i].id!=FSM_STATE_BAD_ID && state_table[i].id!=state;i++);
    if(state_table[i].id!=FSM_STATE_BAD_ID) {
        ASSERT(state_table[i].action_table, "Null action table.\n");

        FSM_ACTION_TABLE action_table=state_table[i].action_table;

        for(i=0;action_table[i].input!=FSM_ACTION_BAD_INPUT && action_table[i].input!=input;i++);
        if(action_table[i].input!=FSM_ACTION_BAD_INPUT) {
            ASSERT(action_table[i].action, "Null action.\n");

            FSM_ACTION action=action_table[i].action;

            ret=action(pfsm, input, parg);
            for(i=0;state_table[i].id!=FSM_STATE_BAD_ID && state_table[i].id!=ret;i++);
            if(state_table[i].id!=FSM_STATE_BAD_ID)
                pfsm->state=ret;
            else
                ret=FSM_ERROR_BAD_NEXT_STATE;
        } else
            ret=FSM_ERROR_UNACCEPTABLE_INPUT;
    } else
        ret=FSM_ERROR_BAD_STATE;

    return ret;
}

#endif
