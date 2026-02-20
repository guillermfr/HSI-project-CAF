//TODO: doxygen

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "fsm_feux_classiques.h"

time_t g_allumes_start_ms = 0;

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
    EV_INIT = 5,

    EV_ERR = 255
} fsm_event_t;

/* Callback functions called on transitions */

static int callback_initialisation(void) {
    printf("[FSM] -> INITIALISATION\n");
    return 0;
}

static int callback_allumer_feux(void) {
    printf("[FSM] -> ALLUMER FEUX\n");
    g_allumes_start_ms = time(NULL); 
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
    {ST_ALLUMES, EV_CMD_1, NULL, ST_ALLUMES},

    /*ACQUITTES*/
    {ST_ACQUITTES, EV_CMD_0, &callback_eteindre_feux, ST_ETEINTS},
    {ST_ACQUITTES, EV_CMD_1, &callback_acquitter, ST_ACQUITTES},

    /*ERREUR*/
    { ST_ANY, EV_ERR, &callback_erreur, ST_TERM}
};

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

int get_next_event(int current_state)
{

    if(current_state == ST_INIT) {
        return EV_INIT;
    }

    boolean_t commande_feux_position = get_commande_feux_position();
    boolean_t commande_feux_croisement = get_commande_feux_croisement();
    boolean_t commande_feux_route = get_commande_feux_route();

    boolean_t commande_feux_any = (commande_feux_position == 1)
                                 || (commande_feux_croisement == 1)
                                 || (commande_feux_route == 1);
    
    if(current_state == ST_ERREUR) {
        return EV_NONE;
    }

    if(commande_feux_any == 0) {
        return EV_CMD_0;
    }

    if(commande_feux_any == 1) {

        if(current_state == ST_ALLUMES) {

            if(get_aquittement() == 1) {
                return EV_ACQUITTEMENT_RECU;
            }

            double elapsed = difftime(time(NULL), g_allumes_start_ms);
            if (elapsed >= 1u) {
                return EV_ACQUITTEMENT_EXPIRE;
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
