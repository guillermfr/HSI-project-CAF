//TODO: doxygen
//TODO: relire ce qui a été fait

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "fsm_essuie-glaces_lave_glace.h"

static time_t g_timer_post_lg_start_s = 0;
/* States */
typedef enum {
    ST_ANY = -1,
    ST_INIT = 0,

    ST_ETEINTS = 1,
    ST_ESSUIE_GLACE_ACTIVES = 2,
    ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES = 3,
    ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES = 4,
    ST_ERREUR = 5,

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
static int callback_demarrer_timer_post_lave_glace(void) {
    printf("[FSM] -> DEMARRER TIMER POST LAVE-GLACE (2s)\n");
    g_timer_post_lg_start_s = time(NULL);
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
    { ST_ETEINTS, EV_CMD_EG_0, NULL, ST_ETEINTS }, 
    { ST_ETEINTS, EV_CMD_LG_0, NULL, ST_ETEINTS },

    /* ---- ESSUIE-GLACES ACTIVES ---- */
    {ST_ESSUIE_GLACE_ACTIVES, EV_CMD_LG_1, &callback_allumer_essuie_glace_lave_glace, ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    {ST_ESSUIE_GLACE_ACTIVES, EV_CMD_EG_0, &callback_eteindre_tout,ST_ETEINTS },
    {ST_ESSUIE_GLACE_ACTIVES, EV_CMD_EG_1, NULL, ST_ESSUIE_GLACE_ACTIVES },

    /* ---- LAVE + ESSUIE ACTIVES ---- */
    { ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_CMD_LG_1, NULL, ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    { ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_CMD_LG_0, callback_demarrer_timer_post_lave_glace, ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },

    /* ---- TIMER EG&LG ETEINTS ---- */
    { ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_CMD_LG_1, &callback_allumer_essuie_glace_lave_glace, ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    
    { ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_TEMPS_INFERIEUR_2, NULL, ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    { ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_TEMPS_SUPERIEUR_2, &callback_eteindre_timer_essuie_glace_lave_glace,              ST_ETEINTS },

    /* ERREUR */
    { ST_ERREUR, EV_ERR, NULL, ST_ERREUR },
};

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

int get_next_event(int current_state)
{
    if (current_state == ST_INIT) {
        return EV_INIT;
    }

    boolean_t cmd_eg = get_cmd_essuie_glace(); /* 0/1 */
    boolean_t cmd_lg = get_cmd_lave_glace();   /* 0/1 */

    /* Cas spécial : pour déclencher l'entrée dans l'état TIMER via EV_CMD_LG_0 */
    if (current_state == ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES) {
        if (cmd_lg == 1u) {
            return EV_CMD_LG_1;
        }
        return EV_CMD_LG_0; /* cmd_lg retombe à 0 => transition vers TIMER dans ta table */
    }

    /* Priorité au lave-glace */
    if (cmd_lg == 1u) {
        return EV_CMD_LG_1;
    }

    /* Etat TIMER : uniquement le temps (conforme au schéma) */
    if (current_state == ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES) {
        const double elapsed = difftime(time(NULL), g_timer_post_lg_start_s);
        if (elapsed >= 2.0) {
            return EV_TEMPS_SUPERIEUR_2;
        }
        return EV_TEMPS_INFERIEUR_2;
    }

    /* Hors Timer : essuie-glace */
    if (cmd_eg == 1u) {
        return EV_CMD_EG_1;
    }

    /* Tout à 0 */
    return EV_CMD_EG_0;
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
