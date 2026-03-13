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

int get_next_event(int current_state);
void fsm_essuie_glace_lave_glace();

#endif 