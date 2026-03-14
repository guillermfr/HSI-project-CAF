/**
 * @file fsm_essuie-glaces_lave_glace.c
 * @brief Programme de la finite state machine pour les essuie-glaces et le lave glace.
 *
 * Ce programme gère la finite state machine pour les essuie-glaces et le lave glace.
 * Il reprend le schéma présent dans l'ennoncé.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "fsm_essuie-glaces_lave_glace.h"

static time_t g_timer_post_lg_start_s = 0;

/**
 * @brief Etats possibles de la machine d'états des essuie-glaces et du lave glace.
 */
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

/**
 * @brief Evenements possibles de la machine d'états des essuie-glaces et du lave glace.
 */
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

/**
 * @brief Callback appelé lors de la transition d'initialisation de la FSM.
 */
static int callback_initialisation(void) {
    //printf("[FSM] -> INITIALISATION : essuie-glace et lave-glace éteints\n");
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où seuls les essuie-glaces sont allumés.
 */
static int callback_allumer_essuie_glace(void) {
    //printf("[FSM] -> ESSUIE-GLACE ACTIVÉ\n");
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où seuls les essuie-glaces doivent être éteints.
 */
static int callback_eteindre_tout(void) {
    //printf("[FSM] -> ESSUIE-GLACE ETEINDRE\n");
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où seuls les essuie-glaces et le lave glace sont allumés.
 */
static int callback_allumer_essuie_glace_lave_glace(void) {
    //printf("[FSM] -> ESSUIE-GLACE + LAVE-GLACE ACTIVÉS\n");
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où les essuie-glaces et le lave glace doivent être éteints après le timer.
 */
static int callback_eteindre_timer_essuie_glace_lave_glace(void) {
    //printf("[FSM] -> ARRÊT TEMPORISÉ : essuie-glace maintenu après lave-glace\n");
    return 0;
}

/**
 * @brief Callback appelé lors de la transition où le timer doit être déclenché pour faire durer le lave glace pendant 2 secondes.
 * 
 * Ce callback permet de récupérer le temps au moment de la transition afin de le réutiliser plus tard lors de la vérification de la durée du timer.
 * 
 */
static int callback_demarrer_timer_post_lave_glace(void) {
    //printf("[FSM] -> DEMARRER TIMER POST LAVE-GLACE (2s)\n");
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

/* Transition table */
static tTransition trans[] = {

    /* INITIALISATION */
    {ST_INIT, EV_INIT, &callback_initialisation, ST_ETEINTS},

    /* TOUT ETEINTS */
    { ST_ETEINTS, EV_CMD_LG_1, &callback_allumer_essuie_glace_lave_glace, ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    { ST_ETEINTS, EV_CMD_EG_1, &callback_allumer_essuie_glace,ST_ESSUIE_GLACE_ACTIVES },
    { ST_ETEINTS, EV_CMD_EG_0, NULL, ST_ETEINTS }, 
    { ST_ETEINTS, EV_CMD_LG_0, NULL, ST_ETEINTS },

    /* ESSUIE-GLACES ACTIVES */
    {ST_ESSUIE_GLACE_ACTIVES, EV_CMD_LG_1, &callback_allumer_essuie_glace_lave_glace, ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    {ST_ESSUIE_GLACE_ACTIVES, EV_CMD_EG_0, &callback_eteindre_tout,ST_ETEINTS },
    {ST_ESSUIE_GLACE_ACTIVES, EV_CMD_EG_1, NULL, ST_ESSUIE_GLACE_ACTIVES },

    /* LAVE + ESSUIE ACTIVES */
    { ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_CMD_LG_1, NULL, ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    { ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_CMD_LG_0, callback_demarrer_timer_post_lave_glace, ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },

    /* TIMER EG&LG ETEINTS */
    { ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_CMD_LG_1, &callback_allumer_essuie_glace_lave_glace, ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    
    { ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_TEMPS_INFERIEUR_2, NULL, ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES },
    { ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES, EV_TEMPS_SUPERIEUR_2, &callback_eteindre_timer_essuie_glace_lave_glace, ST_ETEINTS },

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
 * @return Evénement à traiter.
 */
int get_next_event_essuie_glace_lave_glace(int current_state)
{
    if (current_state == ST_INIT) {
        return EV_INIT;
    }

    boolean_t cmd_eg = get_commande_essuie_glace();
    boolean_t cmd_lg = get_commande_lave_glace();

    // Cas spécial pour l'état TIMER
    if (current_state == ST_ESSUIE_GLACE_LAVE_GLACE_ACTIVES) {
        if (cmd_lg == CMD_ACTIVEE) {
            return EV_CMD_LG_1;
        }
        return EV_CMD_LG_0;
    }

    if (cmd_lg == CMD_ACTIVEE) {
        return EV_CMD_LG_1;
    }

    if (current_state == ST_ETEINDRE_TIMER_ESSUIE_GLACE_LAVE_GLACE_ACTIVES) {
        const double elapsed = difftime(time(NULL), g_timer_post_lg_start_s);
        if (elapsed >= TIMER_ESSUIE_GLACES_LAVE_GLACE) {
            return EV_TEMPS_SUPERIEUR_2;
        }
        return EV_TEMPS_INFERIEUR_2;
    }

    if (cmd_eg == CMD_ACTIVEE) {
        return EV_CMD_EG_1;
    }

    return EV_CMD_EG_0;
}

/**
 * @brief Fonction de la FSM des essuie-glace et du lave glace.
 * Cette fonction doit être appelée à chaque cycle de la boucle principale du programme.
 * 
 * @return void
 */
void fsm_essuie_glace_lave_glace()
{
    long unsigned int i = 0;
    
    int event = EV_NONE;
    int state = get_etat_fsm_essuie_glace_lave_glace();
    
    /* Get event */
    event = get_next_event_essuie_glace_lave_glace(state);
    
    /* For each transitions */
    for (i = 0; i < TRANS_COUNT; i++) {
        /* If State is current state OR The transition applies to all states ...*/
        if ((state == trans[i].state) || (ST_ANY == trans[i].state)) {
            /* If event is the transition event OR the event applies to all */
            if ((event == trans[i].event) || (EV_ANY == trans[i].event)) {
                /* Apply the new state */
                state = trans[i].next_state;
                set_etat_fsm_essuie_glace_lave_glace(state);
                if (trans[i].callback != NULL) {
                    /* Call the state function */
                    (void)trans[i].callback();
                }
                break;
            }
        }
    }

   
}
