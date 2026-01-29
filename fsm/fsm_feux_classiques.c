//TODO: doxygen

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "fsm_feux_classiques.h"

/* States */
typedef enum {
    ST_ANY = -1,                            /* Any state */
    ST_INIT = 0,                            /* Init state */
    ST_ETEINTS = 1,
    ST_ALLUMES = 2,
    ST_ACQUITTES = 3,
    ST_ERREUR = 4,
    ST_TERM = 255                           /* Final state */
} fsm_state_t;

/* Events */
typedef enum {
    EV_ANY = -1,                            /* Any event */
    EV_NONE = 0,                            /* No event */
    EV_CMD_0 = 1,
    EV_CMD_1 = 2,
    EV_ACQUITTEMENT_RECU = 3,
    EV_ACQUITTEMENT_EXPIRE= 4,
    EV_INIT = 5
} fsm_event_t;

/* Callback functions called on transitions */

static int callback_initialisation(void) {
    printf("[FSM] -> INITIALISATION\n");
    return 0;
}

static int callback_allumer_feux(void) {
    printf("[FSM] -> ALLUMER FEUX\n");
    return 0;
}

static int callback_acquitter(void) {
    printf("[FSM] -> ACQUITTER\n");
    return 0;
}

static int callback_eteindre_feux(void) {
    printf("[FSM] -> ETEINDRE FEUX\n");
    return 0;
}

static int callback_erreur(void) {
    printf("[FSM] -> ERREUR\n");
    return 0;
}

/* Transition structure */
typedef struct {
    fsm_state_t state;
    fsm_event_t event;
    int (*callback)(void);
    int next_state;
} tTransition;

/* Transition table */
tTransition trans[] = {
    {ST_INIT, EV_INIT, &callback_initialisation, ST_ETEINTS},

    /* ETEINTS */
    {ST_ETEINTS, EV_CMD_1, &callback_allumer_feux, ST_ALLUMES},
    {ST_ETEINTS, EV_CMD_0, &callback_eteindre_feux, ST_ETEINTS},

    /* ALLUMES */
    {ST_ALLUMES, EV_CMD_0, &callback_eteindre_feux, ST_ETEINTS},
    {ST_ALLUMES, EV_ACQUITTEMENT_RECU, &callback_acquitter, ST_ACQUITTES},
    {ST_ALLUMES, EV_ACQUITTEMENT_EXPIRE, &callback_erreur, ST_ERREUR },
    {ST_ALLUMES, EV_CMD_1, &callback_allumer_feux, ST_ALLUMES},

    /*ACQUITTES*/
    {ST_ACQUITTES, EV_CMD_0, &callback_eteindre_feux, ST_ETEINTS},
    {ST_ACQUITTES, EV_CMD_1, &callback_acquitter, ST_ACQUITTES},

    // TODO: voir si on garde
    // { ST_ANY, EV_ERR, &FsmError, ST_TERM}
};


#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

int get_next_event(int current_state)
{
    int event = EV_NONE;

    /* Here, you can get the parameters of your FSM */

    /* Build all the events */

    /* Example code : 
    if (PARAM1 == ...) {
        event = EV_EVENT1
    }
    else if (PARAM2 == ... && PARAM3 == ...) {
        event = EV_EVENT2
    }
    ...
    */
    return event;
}



/* ========= Construction événement (selon cmd/ack/timeout) ========= */
static fsm_event_t get_next_event(fsm_state_t state)
{
    const int cmd = read_cmd();
    const int ack = read_ack();

    /* cmd=0 prioritaire : on coupe tout */
    if (cmd == 0) {
        return EV_CMD_0;
    }

    /* cmd=1 */
    if (cmd == 1) {
        /* en ALLUMES on attend l'ack, sinon on reste stable */
        if (state == ST_ALLUMES) {
            if (ack) {
                return EV_ACK_RECU;
            }
            /* timeout 1s */
            uint32_t elapsed = now_ms() - g_allumes_start_ms;
            if (elapsed >= 1000u) {
                return EV_ACK_TIMEOUT;
            }
        }
        return EV_CMD_1;
    }

    return EV_NONE;
}



int main(void)
{
    int i = 0;
    int ret = 0; 
    int event = EV_NONE;
    int state = ST_INIT;
    
    /* While FSM hasn't reach end state */
    while (state != ST_TERM) {
        
        /* Get event */
        event = get_next_event(state);
        
        /* For each transitions */
        for (i = 0; i < TRANS_COUNT; i++) {
            /* If State is current state OR The transition applies to all states ...*/
            if ((state == trans[i].state) || (ST_ANY == trans[i].state)) {
                /* If event is the transition event OR the event applies to all */
                if ((event == trans[i].event) || (EV_ANY == trans[i].event)) {
                    /* Apply the new state */
                    state = trans[i].next_state;
                    if (trans[i].callback != NULL) {
                        /* Call the state function */
                        ret = (trans[i].callback)();
                    }
                    break;
                }
            }
        }
    }

    return ret;
}
