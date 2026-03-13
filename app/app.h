/**
 * @file app.h
 * @brief Déclaration des fonctions de l'application.
 */

#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <stdlib.h>

#include "../driver/drv_api.h"
#include "../lib/output/define.h"
#include "../lib/output/donnees.h"
#include "../lib/output/types.h"

#include "../fsm/fsm_feux_classiques.h"
#include "../fsm/fsm_feux_clignotants_warnings.h"
#include "../fsm/fsm_essuie-glaces_lave_glace.h"

void decoder_trame_udp(v_uint8_t *udpFrame);
void decoder_trame_serie(v_uint8_t serialFrame);
void envoyer_trame_serie(v_int32_t identifiant_driver);
void encoder_trame_udp(v_uint8_t* udpFrame);
void verifier_numero_de_trame(v_uint8_t numeroRecu, v_uint8_t *pTrameAttendue);

#endif /* APP_H */