/**
 * @file fsm_essuie-glaces_lave_glace.h
 * @brief Interface de la machine à états des essuie-glaces et du lave glace.
 *
 * Ce fichier déclare les fonctions nécessaires au pilotage de la FSM des essuie-glaces et du lave glace.
 */

#ifndef FSM_ELG_H_
#define FSM_ELG_H_

#include "../lib/output/types.h"
#include "../lib/output/donnees.h"
#include "../lib/output/define.h"

/**
 * @brief Détermine le prochain événement à traiter.
 *
 * Lit les entrées de commande et retourne l'événement correspondant en fonction de l'état actuel.
 * L'ordre des conditions est important dans la logique de la FSM.
 *
 * @param current_state État courant de la FSM.
 * @return Evénement à traiter.
 */
int get_next_event(int current_state);

#endif 