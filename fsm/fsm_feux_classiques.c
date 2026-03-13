/**
 * @file fsm_feux_classiques.c
 * @brief Programme de la finite state machine pour les feux classiques.
 *
 * Ce programme gère la finite state machine pour les 3 types de feu qui doivent être gérés.
 * Il reprend le schéma présent dans l'ennoncé.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "fsm_feux_classiques.h"

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
 * 
 * Ce callback permet de récupérer le temps au moment de la transition afin de le réutiliser plus tard lors de la vérification de la durée de l'acquittement.
 * 
 */
static int callback_allumer_feux(void) {
    printf("[FSM] -> ALLUMER FEUX\n");
    set_timer_feux_acquittement(time(NULL));
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
static tTransition trans[] = {

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
 * @param id_message_feux Type de feux à gérer par la FSM.
 * @return Evénement à traiter.
 */
int get_next_event_feux_classiques(int current_state, enum_id_message_feu_t id_message_feux)
{

    if(current_state == ST_INIT) {
        return EV_INIT;
    }

    if(current_state == ST_ERREUR) {
        return EV_NONE;
    }

    boolean_t commande_feux = 0;

    if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_POSITION) {
        commande_feux = get_commande_feux_position();
    }
    else if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_CROISEMENT) {
        commande_feux = get_commande_feux_croisement();
    }
    else if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_ROUTE) {
        commande_feux = get_commande_feux_route();
    }

    if(commande_feux == CMD_ETEINTE) {
        return EV_CMD_0;
    }

    if(commande_feux == CMD_ACTIVEE) {

        if(current_state == ST_ALLUMES) {

            if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_POSITION && get_acq_feux_position() == CMD_ACTIVEE) {
                return EV_ACQUITTEMENT_RECU;
            }

            if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_CROISEMENT && get_acq_feux_croisement() == CMD_ACTIVEE) {
                return EV_ACQUITTEMENT_RECU;
            }

            if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_ROUTE && get_acq_feux_route() == CMD_ACTIVEE) {
                return EV_ACQUITTEMENT_RECU;
            }

            const double elapsed = difftime(time(NULL), get_timer_feux_acquittement());
            if (elapsed >= ACQ_FEUX_CLASSIQUES) {
                return EV_ACQUITTEMENT_EXPIRE;
            }

        }
    
        return EV_CMD_1;

    }

    return EV_NONE;

}

/**
 * @brief Fonction de la FSM des feux classiques.
 * Cette fonction doit être appelée à chaque cycle de la boucle principale du programme.
 * 
 * @param id_message_feux Type de feux à gérer par la FSM.
 * @return void
 */
void fsm_feux_classiques(enum_id_message_feu_t id_message_feux)
{
    long unsigned int i = 0;
    
    int event = EV_NONE;
    int state = ST_ANY;

    if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_POSITION) {
        state = get_etat_fsm_feux_classiques_position();
    }
    else if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_CROISEMENT) {
        state = get_etat_fsm_feux_classiques_croisement();
    }
    else if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_ROUTE) {
        state = get_etat_fsm_feux_classiques_route();
    }
        
    /* Get event */
    event = get_next_event_feux_classiques(state, id_message_feux);
    
    /* For each transitions */
    for (i = 0; i < TRANS_COUNT; i++) {
        /* If State is current state OR The transition applies to all states ...*/
        if ((state == trans[i].state) || (ST_ANY == trans[i].state)) {
            /* If event is the transition event OR the event applies to all */
            if ((event == trans[i].event) || (EV_ANY == trans[i].event)) {
                /* Apply the new state */
                state = trans[i].next_state;

                if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_POSITION) {
                    set_etat_fsm_feux_classiques_position(state);
                }
                else if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_CROISEMENT) {
                    set_etat_fsm_feux_classiques_croisement(state);
                }
                else if(id_message_feux == ENUM_ID_MESSAGE_FEU_T_MSG_FEU_ROUTE) {
                    set_etat_fsm_feux_classiques_route(state);
                }

                if (trans[i].callback != NULL) {
                    /* Call the state function */
                    (void)trans[i].callback();
                }
                break;
            }
        }
    }

    
}
