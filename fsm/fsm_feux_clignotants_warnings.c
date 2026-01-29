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
    ST_ANY = -1,                 /* Any state */

    ST_ETEINTS = 0,              /* Eteints */
    ST_ACTIVES_ALLUMES = 1,          /* Activés & Allumés */
    ST_ACQUITTES_ALLUME = 2,           /* Acquittés (voyant allumé) */
    ST_ACTIVE_ETEINTS = 3,          /* Activés & Eteints */
    ST_ACQUITTES_ETEINT = 4,           /* Acquittés (voyant éteint) */
    ST_ERREUR = 5,               /* Erreur (atteints à jamais) */

    ST_TERM = 255
} fsm_state_t;




/* Events */
typedef enum {
    EV_ANY = -1,                            /* Any event */
    EV_NONE = 0,                            /* No event */
    
    EV_CMD_0 = 1,                /* cmd == 0 */
    EV_CMD_1 = 2,                /* cmd == 1 */

    EV_ACQUITTEMENT_RECU = 3,
    EV_ACQUITTEMENT_EXPIRE= 4,
    EV_INIT = 5,
    EV_TEMPS_1SEC = 6    
   
    
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

static int callback_clignotement_allumer(void){
    printf("[FSM] -> ALLUMER FEUX EN PHASE DE CLIGNOTEMENT\n");
    return 0;
}

static int callback_clignotement_eteindre(void){
    printf("[FSM] -> ETEINDRE FEUX EN PHASE DE CLIGNOTEMENT\n");
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
    /* Initialisation */
    {ST_ETEINTS, EV_INIT, &callback_initialisation, ST_ETEINTS},
    
    /* Eteints */
    {ST_ETEINTS, EV_CMD_1, &callback_allumer_feux, ST_ACTIVES_ALLUMES},
    {ST_ETEINTS, EV_CMD_0, &callback_eteindre_feux, ST_ETEINTS},



    /* ---- Activés et Allumés ---- */
    {ST_ACTIVES_ALLUMES,EV_CMD_0,&callback_eteindre_feux,ST_ETEINTS},
    {ST_ACTIVES_ALLUMES,EV_ACQUITTEMENT_EXPIRE,&callback_erreur,ST_ERREUR},
    {ST_ACTIVES_ALLUMES,EV_ACQUITTEMENT_RECU,&callback_clignotement_allumer,ST_ACTIVES_ALLUMES},
    {ST_ACTIVES_ALLUMES, EV_CMD_1, &callback_allumer_feux, ST_ACTIVES_ALLUMES},
    
  
    /* ---- ACQUITTES (ALLUME) ---- */
    { ST_ACQUITTES_ALLUME,EV_CMD_0,&callback_eteindre_feux,ST_ETEINTS },
    { ST_ACQUITTES_ALLUME,EV_TEMPS_1SEC,&callback_clignotement_eteindre,ST_ACTIVE_ETEINTS},  /* après 1 sec -> Actives & Eteints */
    { ST_ACQUITTES_ALLUME,EV_CMD_1,&callback_acquitter,ST_ACQUITTES_ALLUME},   /* self-loop */



    /* ---- ACTIVES & ETEINTS ---- */
     { ST_ACTIVE_ETEINTS, EV_CMD_0,      &callback_eteindre_feux,    ST_ETEINTS },
    { ST_ACTIVE_ETEINTS, EV_ACQUITTEMENT_RECU,  &callback_clignotement_eteindre, ST_ACQUITTES_ETEINT },
    { ST_ACTIVE_ETEINTS, EV_ACQUITTEMENT_EXPIRE, &callback_erreur,     ST_ERREUR },
    { ST_ACTIVE_ETEINTS, EV_CMD_1,      &callback_clignotement_eteindre,  ST_ACTIVE_ETEINTS },  /* self-loop */

    /* ---- ACQUITTES (ETEINT) ---- */
     { ST_ETEINTS,  EV_CMD_0,       &callback_eteindre_feux,    ST_ETEINTS },
    { ST_ETEINTS,  EV_TEMPS_1SEC,    &callback_clignotement_allumer,ST_ACTIVES_ALLUMES },  /* après 1 sec -> Actives & Allumés */
    { ST_ETEINTS,  EV_CMD_1,       &callback_acquitter,  ST_ETEINTS },   /* self-loop */
    
    /* ---- ERREUR (atteints à jamais) ---- */
     { ST_ERREUR,      EV_ANY,         NULL,                ST_ERREUR },
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
   // int state = ST_INIT;
    
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
