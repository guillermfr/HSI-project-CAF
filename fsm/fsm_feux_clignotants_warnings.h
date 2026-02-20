/**
 * @file fsm_feux_clignotants_warnings.h
 * @brief Interface de la machine à états des clignotants et warning.
 *
 * Ce fichier déclare les fonctions nécessaires au pilotage de la FSM des clignotants et warnings.
 */

#ifndef FSM_CW_H_
#define FSM_CW_H_

#include "../lib/output/types.h"
#include "../lib/output/donnees.h"

static int callback_initialisation(void);

static int callback_enter_actives_allumes(void);

static int callback_enter_actives_eteints(void);

static int callback_enter_acquittes_allume(void);

static int callback_enter_acquittes_eteint(void);

static int callback_eteindre_feux(void);

static int callback_erreur(void);

/**
 * @brief Détermine le prochain événement à traiter.
 *
 * Lit les entrées de commande et retourne l'événement correspondant en fonction de l'état actuel.
 *
 * @param current_state État courant de la FSM.
 * @return Evénement à traiter.
 */
int get_next_event(int current_state);

#endif