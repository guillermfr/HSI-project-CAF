#ifndef APP_H
#define APP_H

#include <stdio.h>
#include <stdlib.h>

#include "../driver/drv_api.h"
#include "../lib/output/define.h"
#include "../lib/output/donnees.h"
#include "../lib/output/types.h"


/**
 * @brief Décode la trame UDP reçue du MUX et met à jour les données applicatives.
 * 
 * @param udpFrame Le buffer contenant la trame UDP (15 octets).
 */
void decoder_trame_udp(v_uint8_t *udpFrame);


/**
 * @brief Décode la trame série reçue du Comodo et affiche les commandes.
 * 
 * @param serialFrame L'octet de données de la trame série.
 */
void decoder_trame_serie(v_uint8_t serialFrame);


/**
 * @brief Encode les données applicatives dans une trame UDP à envoyer au MUX.
 * 
 * @param udpFrame Le buffer de 10 octets à remplir avec la trame UDP à envoyer.
 */
void encoder_trame_udp(v_uint8_t* udpFrame);

/**
 * @brief Vérifie que le numéro de trame reçu correspond à celui attendu.
 * 
 * @param numeroRecu Le numéro de trame reçu.
 * @param pTrameAttendue Pointeur vers le numéro de trame attendu (mis à jour si la vérification est réussie).
 */
void verifier_numero_de_trame(v_uint8_t numeroRecu, v_uint8_t *pTrameAttendue);

#endif /* APP_H */