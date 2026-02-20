/**
 * \file        fsm.c
 * \author      Alexis Daley
 * \version     0.4
 * \date        08 otober 2023
 * \brief       This is a template file to create a Finite State Machine.
 * \details
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/* States */
typedef enum {
    ST_ANY = -1,
    ST_INIT = 0,

    ST_ETEINTS = 1,
    ST_ESSUIE_GLACE_ACTIVES = 2,
    ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES = 3,
    ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES = 4,

    ST_TERM = 255
} fsm_state_t;

/* Events */
typedef enum {
    EV_ANY = -1,
    EV_NONE = 0,

    EV_CMD_EG_0 = 1,
    EV_CMD_LG_0 = 2,

    EV_CMD_EG_1 = 3,
    EV_CMD_LG_1 = 4,

    EV_TEMPS_INFERIEUR_2 = 5,
    EV_TEMPS_SUPERIEUR_2 = 6,
    EV_INIT = 7,

    EV_ERR = 255
} fsm_event_t;

/* Callback functions called on transitions */

static int callback_initialisation(void) {
    printf("[FSM] -> INITIALISATION : essuie-glace et lave-glace éteints\n");
    return 0;
}

static int callback_allumer_essuie_glace(void) {
    printf("[FSM] -> ESSUIE-GLACE ACTIVÉ\n");
    return 0;
}

static int callback_eteindre_tout(void) {
    printf("[FSM] -> ESSUIE-GLACE ETEINDRE\n");
    return 0;
}

static int callback_allumer_essuie_glace_lave_glace(void) {
    printf("[FSM] -> ESSUIE-GLACE + LAVE-GLACE ACTIVÉS\n");
    return 0;
}

static int callback_eteindre_timer_essuie_glace_lave_glace(void) {
    printf("[FSM] -> ARRÊT TEMPORISÉ : essuie-glace maintenu après lave-glace\n");
    return 0;
}


/* Transition structure */
typedef struct {
    fsm_state_t state;
    fsm_event_t event;
    int (*callback)(void);
    int next_state;
} tTransition;

/* ================== Transition table (diagramme) ================== */
tTransition trans[] = {

    /* Initialisation */
    {ST_INIT, EV_INIT, &callback_initialisation, ST_ETEINTS},

    /* ---- TOUT ETEINTS ---- */
    { ST_ETEINTS, EV_CMD_LG_1, &callback_allumer_essuie_glace_lave_glace, ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    { ST_ETEINTS, EV_CMD_EG_1, &callback_allumer_essuie_glace,ST_ESSUIE_GLACE_ACTIVES },
    { ST_ETEINTS, EV_CMD_EG_0 & EV_CMD_LG_0, &callback_eteindre_tout, ST_ETEINTS },

    /* ---- ESSUIE-GLACES ACTIVES ---- */
    {ST_ESSUIE_GLACE_ACTIVES, EV_CMD_LG_1, &callback_allumer_essuie_glace_lave_glace, ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    {ST_ESSUIE_GLACE_ACTIVES, EV_CMD_EG_0, &callback_eteindre_tout,ST_ETEINTS },
    {ST_ESSUIE_GLACE_ACTIVES, EV_CMD_EG_1, &callback_allumer_essuie_glace, ST_ESSUIE_GLACE_ACTIVES },

    /* ---- LAVE + ESSUIE ACTIVES ---- */
    { ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_CMD_LG_1, &callback_allumer_essuie_glace_lave_glace, ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    { ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_CMD_LG_0, &callback_eteindre_timer_essuie_glace_lave_glace, ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },

    /* ---- TIMER EG&LG ETEINTS ---- */
    { ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_CMD_LG_1, &callback_allumer_essuie_glace_lave_glace, ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    { ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_TEMPS_INFERIEUR_2, &callback_eteindre_timer_essuie_glace_lave_glace, ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    { ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_TEMPS_SUPERIEUR_2, &callback_eteindre_timer_essuie_glace_lave_glace,              ST_ETEINTS },

    // TODO: voir si on met etat erreur
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
