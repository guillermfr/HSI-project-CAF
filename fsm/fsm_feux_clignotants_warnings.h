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
#include "../lib/output/define.h"

int get_next_event_clignotants_warning(int current_state, enum_id_message_feu_t id_message_clignotants_warning);
void fsm_feux_clignotant_warning(enum_id_message_feu_t id_message_clignotants_warning);

#endif