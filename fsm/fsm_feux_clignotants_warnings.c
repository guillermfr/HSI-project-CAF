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
#include "fsm_feux_clignotants_warnings.h"

#include <time.h>

static time_t g_ack_debut_s   = 0;
static time_t g_clignotement_debut_s = 0;

/* States */
typedef enum {
    ST_ANY = -1,
    ST_INIT = 0,

    ST_ETEINTS = 1,
    ST_ACTIVES_ALLUMES = 2,
    ST_ACQUITTES_ALLUME = 3,
    ST_ACTIVES_ETEINTS = 4,
    ST_ACQUITTES_ETEINT = 5,
    ST_ERREUR = 6,

    ST_TERM = 255
} fsm_state_t;

/* Events */
typedef enum {
    EV_ANY = -1,
    EV_NONE = 0,
    
    EV_CMD_0 = 1,
    EV_CMD_1 = 2,

    EV_ACQUITTEMENT_RECU = 3,
    EV_ACQUITTEMENT_EXPIRE= 4,
    EV_INIT = 5,
    EV_TEMPS_1SEC = 6,

    EV_ERR = 255
} fsm_event_t;

static int callback_initialisation(void) {
    printf("[FSM] -> INITIALISATION\n");
    return 0;
}

static int callback_enter_actives_allumes(void) {
    printf("[FSM] -> ACTIVES_ALLUMES (feux ON, attente ACK)\n");
    g_ack_debut_s = time(NULL);

    return 0;
}

static int callback_enter_actives_eteints(void) {
    printf("[FSM] -> ACTIVES_ETEINTS (feux OFF, attente ACK)\n");
    g_ack_debut_s = time(NULL);
    return 0;
}

static int callback_enter_acquittes_allume(void) {
    printf("[FSM] -> ACQUITTES_ALLUME (clignotement ON)\n");
    g_clignotement_debut_s = time(NULL);
    return 0;
}

static int callback_enter_acquittes_eteint(void) {
    printf("[FSM] -> ACQUITTES_ETEINT (clignotement OFF)\n");
    g_clignotement_debut_s = time(NULL);
    return 0;
}

static int callback_eteindre_feux(void) {
    printf("[FSM] -> ETEINTS\n");
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

    /* INITIALISATION */
    { ST_INIT, EV_INIT, callback_initialisation, ST_ETEINTS },

    /* ETEINTS */
    { ST_ETEINTS, EV_CMD_1, callback_enter_actives_allumes, ST_ACTIVES_ALLUMES },
    { ST_ETEINTS, EV_CMD_0, NULL, ST_ETEINTS },

    /* ACTIVES_ALLUMES */
    { ST_ACTIVES_ALLUMES, EV_CMD_0, callback_eteindre_feux, ST_ETEINTS },
    { ST_ACTIVES_ALLUMES, EV_ACQUITTEMENT_RECU, callback_enter_acquittes_allume, ST_ACQUITTES_ALLUME },
    { ST_ACTIVES_ALLUMES, EV_ACQUITTEMENT_EXPIRE, callback_erreur, ST_ERREUR },
    { ST_ACTIVES_ALLUMES, EV_CMD_1, NULL, ST_ACTIVES_ALLUMES },

    /* ACQUITTES_ALLUME */
    { ST_ACQUITTES_ALLUME, EV_CMD_0, callback_eteindre_feux, ST_ETEINTS },
    { ST_ACQUITTES_ALLUME, EV_TEMPS_1SEC, callback_enter_actives_eteints, ST_ACTIVES_ETEINTS },
    { ST_ACQUITTES_ALLUME, EV_CMD_1, NULL, ST_ACQUITTES_ALLUME },

    /* ACTIVES_ETEINTS */
    { ST_ACTIVES_ETEINTS, EV_CMD_0, callback_eteindre_feux, ST_ETEINTS },
    { ST_ACTIVES_ETEINTS, EV_ACQUITTEMENT_RECU, callback_enter_acquittes_eteint, ST_ACQUITTES_ETEINT },
    { ST_ACTIVES_ETEINTS, EV_ACQUITTEMENT_EXPIRE, callback_erreur, ST_ERREUR },
    { ST_ACTIVES_ETEINTS, EV_CMD_1, NULL, ST_ACTIVES_ETEINTS },

    /* ACQUITTES_ETEINT */
    { ST_ACQUITTES_ETEINT, EV_CMD_0, callback_eteindre_feux, ST_ETEINTS },
    { ST_ACQUITTES_ETEINT, EV_TEMPS_1SEC, callback_enter_actives_allumes, ST_ACTIVES_ALLUMES },
    { ST_ACQUITTES_ETEINT, EV_CMD_1, NULL, ST_ACQUITTES_ETEINT },

    /* ERREUR */
    { ST_ERREUR, EV_ERR, NULL, ST_ERREUR },
};

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

int get_next_event(int current_state)
{

    if(current_state == ST_INIT) {
        return EV_INIT;
    }

    if(current_state == ST_ERREUR) {
        return EV_NONE;
    }

    boolean_t commande_warning = get_commande_warning();
    boolean_t commande_clignotant_droit = get_commande_clignotant_droit();
    boolean_t commande_clignotant_gauche = get_commande_clignotant_gauche();

    boolean_t commande_clignotant_any = (commande_warning == 1)
                                    || (commande_clignotant_droit == 1)
                                    || (commande_clignotant_gauche == 1);

    if(commande_clignotant_any == 0) {
        return EV_CMD_0;
    }

    switch (current_state) {

        case ST_ACTIVES_ALLUMES:
        case ST_ACTIVES_ETEINTS: {

            if(get_acquittement_fsm_feux_clignotants_warnings() == 1) {
                return EV_ACQUITTEMENT_RECU;
            }

            const double elapsed = difftime(time(NULL), g_ack_debut_s);
            if(elapsed >= 1.0) {
                return EV_ACQUITTEMENT_EXPIRE;
            }

            return EV_CMD_1;

        }
            
        case ST_ACQUITTES_ALLUME:
        case ST_ACQUITTES_ETEINT: {

            const double elapsed = difftime(time(NULL), g_clignotement_debut_s);
            if(elapsed >= 1.0) {
                return EV_TEMPS_1SEC;
            }

            return EV_CMD_1;

        }

        case ST_ETEINTS:
        default:
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
