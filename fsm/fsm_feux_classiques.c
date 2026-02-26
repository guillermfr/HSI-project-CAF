/**
 * @file fsm_feux_classiques.c
 * @brief Programme de la final state machine pour les feux classiques.
 *
 * Ce programme gère la final state machine pour les 3 types de feu qui doivent être gérés.
 * Il reprend le schéma présent dans l'ennoncé.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "fsm_feux_classiques.h"

time_t g_allumes_debut_ms = 0;

/**
 * @brief Etats possibles de la machine d'états des feux.
 */
typedef enum {
    ST_ANY = -1,
    ST_INIT = 0,

    ST_ETEINTS = 1,
    ST_ALLUMES = 2,
    ST_ACQUITTES = 3,
    ST_ERREUR = 4,

    ST_TERM = 255
} fsm_state_t;

/**
 * @brief Evenements possibles de la machine d'états des feux.
 */
typedef enum {
    EV_ANY = -1,
    EV_NONE = 0,

    EV_CMD_0 = 1,
    EV_CMD_1 = 2,
    EV_ACQUITTEMENT_RECU = 3,
    EV_ACQUITTEMENT_EXPIRE= 4,
    EV_INIT = 5,

    EV_ERR = 255
} fsm_event_t;

/**
 * @brief Callback appelé lors de la transition d'initialisation de la FSM.
 */
static int callback_initialisation(void) {
    printf("[FSM] -> INITIALISATION\n");
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où les feux passent de l'état éteints à allumés.
 */
static int callback_allumer_feux(void) {
    printf("[FSM] -> ALLUMER FEUX\n");
    g_allumes_debut_ms = time(NULL); 
    return 0;
}

/**
 * @brief Callback appelé lors de la transition après avoir reçu l'acquittement.
 */
static int callback_acquitter(void) {
    printf("[FSM] -> ACQUITTER\n");
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où les feux passent de l'état allumés à éteints.
 */
static int callback_eteindre_feux(void) {
    printf("[FSM] -> ETEINDRE FEUX\n");
    return 0;
}

/**
 * @brief Callback appelé lorsqu'il y a une erreur.
 */
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
    {ST_INIT, EV_INIT, &callback_initialisation, ST_ETEINTS},

    /* ETEINTS */
    {ST_ETEINTS, EV_CMD_1, &callback_allumer_feux, ST_ALLUMES},
    {ST_ETEINTS, EV_CMD_0, &callback_eteindre_feux, ST_ETEINTS},

    /* ALLUMES */
    {ST_ALLUMES, EV_CMD_0, &callback_eteindre_feux, ST_ETEINTS},
    {ST_ALLUMES, EV_ACQUITTEMENT_RECU, &callback_acquitter, ST_ACQUITTES},
    {ST_ALLUMES, EV_ACQUITTEMENT_EXPIRE, &callback_erreur, ST_ERREUR },
    {ST_ALLUMES, EV_CMD_1, NULL, ST_ALLUMES},

    /* ACQUITTES */
    {ST_ACQUITTES, EV_CMD_0, &callback_eteindre_feux, ST_ETEINTS},
    {ST_ACQUITTES, EV_CMD_1, &callback_acquitter, ST_ACQUITTES},

    /* ERREUR */
    { ST_ANY, EV_ERR, &callback_erreur, ST_TERM}
};

#define TRANS_COUNT (sizeof(trans)/sizeof(*trans))

/**
 * @brief Détermine le prochain événement à traiter.
 *
 * Lit les entrées de commande et retourne l'événement correspondant en fonction de l'état actuel.
 * L'ordre des conditions est important dans la logique de la FSM.
 *
 * @param current_state État courant de la FSM.
 * @return Evénement à traiter.
 */
int get_next_event(int current_state)
{

    if(current_state == ST_INIT) {
        return EV_INIT;
    }

    if(current_state == ST_ERREUR) {
        return EV_NONE;
    }

    boolean_t commande_feux_position = get_commande_feux_position();
    boolean_t commande_feux_croisement = get_commande_feux_croisement();
    boolean_t commande_feux_route = get_commande_feux_route();

    boolean_t commande_feux_any = (commande_feux_position == CMD_ACTIVEE)
                                 || (commande_feux_croisement == CMD_ACTIVEE)
                                 || (commande_feux_route == CMD_ACTIVEE);

    if(commande_feux_any == CMD_ETEINTE) {
        return EV_CMD_0;
    }

    if(commande_feux_any == CMD_ACTIVEE) {

        if(current_state == ST_ALLUMES) {

            if(get_acquittement_fsm_feux_classiques == CMD_ACTIVEE) {
                return EV_ACQUITTEMENT_RECU;
            }

            double elapsed = difftime(time(NULL), g_allumes_debut_ms);
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
}
