/**
 * @file fsm_feux_clignotants_warnings.c
 * @brief Programme de la finite state machine pour les clignotants et warnings.
 *
 * Ce programme gère la finite state machine pour le clignotant gauche, le clignotant droit et les warnings.
 * Il reprend le schéma présent dans l'énoncé.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "fsm_feux_clignotants_warnings.h"

#include <time.h>

/**
 * @brief Etats possibles de la machine d'états des clignotants et warnings.
 */
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

/**
 * @brief Evenements possibles de la machine d'états des clignotants et warnings.
 */
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

/**
 * @brief Callback appelé lors de la transition d'initialisation de la FSM.
 */
static int callback_initialisation(void) {
    printf("[FSM] -> INITIALISATION\n");
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où les clignotants passent de l'état éteints à activés et allumés.
 * 
 * Ce callback permet de récupérer le temps au moment de la transition afin de le réutiliser plus tard lors de la vérification de la durée de l'acquittement.
 * 
 */
static int callback_enter_actives_allumes(void) {
    printf("[FSM] -> ACTIVES_ALLUMES (feux ON, attente ACK)\n");
    set_timer_clignotants_warning_acquittement(time(NULL));
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où les clignotants passent de l'état éteints à activés et allumés.
 * 
 * Ce callback permet de récupérer le temps au moment de la transition afin de le réutiliser plus tard lors de la vérification de la durée de l'acquittement.
 * 
 */
static int callback_enter_actives_eteints(void) {
    printf("[FSM] -> ACTIVES_ETEINTS (feux OFF, attente ACK)\n");
    set_timer_clignotants_warning_acquittement(time(NULL));
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où les clignotants passent de l'état activés et allumés à acquittés et allumés.
 * 
 * Ce callback permet de récupérer le temps au moment de la transition afin de le réutiliser plus tard lors de la vérification de la durée de l'acquittement.
 * 
 */
static int callback_enter_acquittes_allume(void) {
    printf("[FSM] -> ACQUITTES_ALLUME (clignotement ON)\n");
    set_timer_clignotants_warning_changement(time(NULL));
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où les clignotants passent de l'état activés et éteints à l'état acquittés et éteints.
 * 
 * Ce callback permet de récupérer le temps au moment de la transition afin de le réutiliser plus tard lors de la vérification de la durée de l'acquittement.
 * 
 */
static int callback_enter_acquittes_eteint(void) {
    printf("[FSM] -> ACQUITTES_ETEINT (clignotement OFF)\n");
    set_timer_clignotants_warning_changement(time(NULL));
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où les clignotants passent à l'état éteints.
 */
static int callback_eteindre_feux(void) {
    printf("[FSM] -> ETEINTS\n");
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

/**
 * @brief Détermine le prochain événement à traiter.
 *
 * Lit les entrées de commande et retourne l'événement correspondant en fonction de l'état actuel.
 * L'ordre des conditions est important dans la logique de la FSM.
 *
 * @param current_state État courant de la FSM.
 * @param id_message_clignotants_warning Type de clignotant à gérer par la FSM.
 * @return Evénement à traiter.
 */
int get_next_event_clignotants_warning(int current_state, enum_id_message_feu_t id_message_clignotants_warning)
{

    if(current_state == ST_INIT) {
        return EV_INIT;
    }

    if(current_state == ST_ERREUR) {
        return EV_NONE;
    }

    boolean_t commande_clignotants_warning = 0;

    if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_CLIGNOTANT_DROIT) {
        commande_clignotants_warning = get_commande_clignotant_droit();
    }
    else if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_CLIGNOTANT_GAUCHE) {
        commande_clignotants_warning = get_commande_clignotant_gauche();
    }
    else if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_WARNING) {
        commande_clignotants_warning = get_commande_warning();
    }

    if(commande_clignotants_warning == CMD_ETEINTE) {
        return EV_CMD_0;
    }

    switch (current_state) {

        case ST_ACTIVES_ALLUMES:
        case ST_ACTIVES_ETEINTS: {

            if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_CLIGNOTANT_DROIT && get_acq_clignotant_droit() == CMD_ACTIVEE) {
                return EV_ACQUITTEMENT_RECU;
            }

            if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_CLIGNOTANT_GAUCHE && get_acq_clignotant_gauche() == CMD_ACTIVEE) {
                return EV_ACQUITTEMENT_RECU;
            }

             if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_WARNING && get_acq_warning() == CMD_ACTIVEE) {
                return EV_ACQUITTEMENT_RECU;
            }

            const double elapsed = difftime(time(NULL), get_timer_clignotants_warning_acquittement());
            if(elapsed >= ACQ_FEUX_CLIGNOTANTS_WARNINGS) {
                return EV_ACQUITTEMENT_EXPIRE;
            }

            return EV_CMD_1;

        }
            
        case ST_ACQUITTES_ALLUME:
        case ST_ACQUITTES_ETEINT: {

            const double elapsed = difftime(time(NULL), get_timer_clignotants_warning_changement());
            if(elapsed >= ACQ_FEUX_CLIGNOTANTS_WARNINGS) {
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

/**
 * @brief Fonction de la FSM des clignotants et du warning.
 * Cette fonction doit être appelée à chaque cycle de la boucle principale du programme.
 * 
 * @param id_message_clignotants_warning Type de clignotants ou warning à gérer par la FSM.
 * @return void
 */
void fsm_feux_clignotant_warning(enum_id_message_feu_t id_message_clignotants_warning)
{
    long unsigned int i = 0;
    
    int event = EV_NONE;
    int state = ST_ANY;

    if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_CLIGNOTANT_DROIT) {
        state = get_etat_fsm_clignotants_droit();
    }
    else if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_CLIGNOTANT_GAUCHE) {
        state = get_etat_fsm_clignotants_gauche();
    }
    else if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_WARNING) {
        state = get_etat_fsm_warning();
    }
    
    /* Get event */
    event = get_next_event_clignotants_warning(state, id_message_clignotants_warning);

    /* For each transitions */
    for (i = 0; i < TRANS_COUNT; i++) {
        /* If State is current state OR The transition applies to all states ...*/
        if ((state == trans[i].state) || (ST_ANY == trans[i].state)) {
            /* If event is the transition event OR the event applies to all */
            if ((event == trans[i].event) || (EV_ANY == trans[i].event)) {
                /* Apply the new state */
                state = trans[i].next_state;

                if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_CLIGNOTANT_DROIT) {
                    set_etat_fsm_clignotants_droit(state);
                }
                else if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_CLIGNOTANT_GAUCHE) {
                    set_etat_fsm_clignotants_gauche(state);
                }
                else if(id_message_clignotants_warning == ENUM_ID_MESSAGE_FEU_T_MSG_WARNING) {
                    set_etat_fsm_warning(state);
                }
                
                if (trans[i].callback != NULL) {
                    /* Call the state function */
                    (void) trans[i].callback();
                }
                break;
            }
        }
    }

  
}
