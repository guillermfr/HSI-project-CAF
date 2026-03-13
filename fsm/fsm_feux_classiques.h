/**
 * @file fsm_feux_classiques.h
 * @brief Interface de la machine à états des feux classiques.
 *
 * Ce fichier déclare les fonctions nécessaires au pilotage de la FSM des feux classiques.
 */

#ifndef FSM_C_H_
#define FSM_C_H_

#include "../lib/output/types.h"
#include "../lib/output/donnees.h"
#include "../lib/output/define.h"

int get_next_event_feux_classiques(int current_state, enum_id_message_feu_t id_message_feux);
void fsm_feux_classiques(enum_id_message_feu_t id_message_feux);

#endif