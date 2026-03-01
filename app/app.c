/**
 * @file app.c
 * @brief Programme de lecture et d'affichage des trames UDP 100 ms via le driver DRV.
 *
 * Ce programme initialise le driver DRV, lit périodiquement une trame UDP
 * toutes les 100 ms et affiche son contenu.
 */

#include "app.h"

void decoder_trame_udp(v_uint8_t *udpFrame) {
    set_numero_de_trame(udpFrame[0]); // 1 octet


    v_uint32_t km = ((v_uint32_t)udpFrame[1] << 24) | // 4 octets, Big Endian
                    ((v_uint32_t)udpFrame[2] << 16) |
                    ((v_uint32_t)udpFrame[3] << 8)  |
                    ((v_uint32_t)udpFrame[4]);
    set_kilometrage(km);

    set_vitesse(udpFrame[5]); // 1 octet
    set_probleme_chassis(udpFrame[6]); // 1 octet
    set_probleme_moteur(udpFrame[7]); // 1 octet
    set_niveau_reservoir(udpFrame[8]); // 1 octet

    v_uint32_t rpm = ((v_uint32_t)udpFrame[9] << 24) | // 4 octets, Big Endian
                     ((v_uint32_t)udpFrame[10] << 16) |
                     ((v_uint32_t)udpFrame[11] << 8)  |
                     ((v_uint32_t)udpFrame[12]);
    set_regime_trmin(rpm);

    set_probleme_batterie(udpFrame[13]); // 1 octet
    set_CRC8(udpFrame[14]); // 1 octet
}


void encoder_trame_udp(v_uint8_t* udpFrame) {
    for (int i = 0; i < DRV_UDP_200MS_FRAME_SIZE; i++) {
        udpFrame[i] = 0;
    }

    // Octet 0 (MSB)
    // Bit 7: Voyant feux de position
    if (get_commande_feux_position()) udpFrame[0] |= (1 << 7);
    // Bit 6: Voyant feux de croisement
    if (get_commande_feux_croisement()) udpFrame[0] |= (1 << 6);
    // Bit 5: Voyant feux de route
    if (get_commande_feux_route()) udpFrame[0] |= (1 << 5);
    // Bit 4: Voyant essence (<= 5%)
    // 5% de 40L = 2L
    if (get_niveau_reservoir() <= 2) udpFrame[0] |= (1 << 4);
    // Bit 3: Voyant défaut moteur
    if (get_probleme_moteur() != ENUM_PROBLEME_MOTEUR_T_AUCUN) udpFrame[0] |= (1 << 3);
    // Bit 2: Voyant pression pneus
    if (get_probleme_chassis() & ENUM_PROBLEME_CHASSIS_T_PRESSION_PNEUS) udpFrame[0] |= (1 << 2);
    // Bit 1: Inutilisé
    // Bit 0: Voyant batterie déchargée
    if (get_probleme_batterie() & ENUM_PROBLEME_BATTERIE_T_DECHARGEE) udpFrame[0] |= (1 << 0);

    // Octet 1
    // Bit 7: Voyant Warnings
    if (get_commande_warning()) udpFrame[1] |= (1 << 7);
    // Bit 6: Voyant Panne batterie
    if (get_probleme_batterie() & ENUM_PROBLEME_BATTERIE_T_PANNE) udpFrame[1] |= (1 << 6);
    // Bit 5: Voyant Temp. LDR
    if (get_probleme_moteur() & ENUM_PROBLEME_MOTEUR_T_TEMPERATURE_LDR) udpFrame[1] |= (1 << 5);
    // Bit 4: Voyant pression moteur
    if (get_probleme_moteur() & ENUM_PROBLEME_MOTEUR_T_DEFAUT_PRESSION) udpFrame[1] |= (1 << 4);
    // Bit 3: Voyant surchauffe huile
    if (get_probleme_moteur() & ENUM_PROBLEME_MOTEUR_T_SURCHAUFFE_HUILE) udpFrame[1] |= (1 << 3);
    // Bit 2: Voyant défaillance freins
    if (get_probleme_chassis() & ENUM_PROBLEME_CHASSIS_T_DEFAILLANCE_FREINS) udpFrame[1] |= (1 << 2);
    // Bit 1: Activation Essuie glaces
    if (get_commande_essuie_glace()) udpFrame[1] |= (1 << 1);
    // Bit 0: Activation Lave glace
    if (get_commande_lave_glace()) udpFrame[1] |= (1 << 0);

    // Octets 2-5: Kilométrage , Big Endian 
    v_uint32_t km = get_kilometrage();
    udpFrame[2] = (km >> 24) & 0xFF;
    udpFrame[3] = (km >> 16) & 0xFF;
    udpFrame[4] = (km >> 8) & 0xFF;
    udpFrame[5] = km & 0xFF;

    // Octet 6: Vitesse
    udpFrame[6] = get_vitesse();

    // Octet 7: Niveau réservoir , en pourcent %
    v_uint8_t niveau = get_niveau_reservoir();
    if (niveau > 40) niveau = 40;
    udpFrame[7] = (niveau * 100) / 40;

    // Octets 8-9: RPM , Big Endian
    v_uint32_t rpm = get_regime_trmin();
    if (rpm >= 1000) {
        rpm /= 10;
    }
    udpFrame[8] = (rpm >> 8) & 0xFF;
    udpFrame[9] = rpm & 0xFF;
}


void decoder_trame_serie(v_uint8_t serialFrame) {
    set_commande_warning((serialFrame >> 7) & 0x01); // 1 bit
    set_commande_feux_position((serialFrame >> 6) & 0x01); // 1 bit
    set_commande_feux_croisement((serialFrame >> 5) & 0x01); // 1 bit
    set_commande_feux_route((serialFrame >> 4) & 0x01); // 1 bit
    set_commande_clignotant_droit((serialFrame >> 3) & 0x01); // 1 bit
    set_commande_clignotant_gauche((serialFrame >> 2) & 0x01); // 1 bit
    set_commande_essuie_glace((serialFrame >> 1) & 0x01); // 1 bit
    set_commande_lave_glace((serialFrame >> 0) & 0x01); // 1 bit
}


void envoyer_trame_serie(v_int32_t identifiant_driver) {
    serial_frame_t frames[5];
    v_uint32_t nb_frames = 0;
    
    const v_int32_t SER_SERIE = 11;

    // 1. Feux de position
    frames[nb_frames].serNum = SER_SERIE;
    frames[nb_frames].frameSize = 2;
    frames[nb_frames].frame[0] = 0x01;
    frames[nb_frames].frame[1] = get_commande_feux_position() ? 0x01 : 0x00;
    nb_frames++;

    // 2. Feux de croisement
    frames[nb_frames].serNum = SER_SERIE;
    frames[nb_frames].frameSize = 2;
    frames[nb_frames].frame[0] = 0x02;
    frames[nb_frames].frame[1] = get_commande_feux_croisement() ? 0x01 : 0x00;
    nb_frames++;

    // 3. Feux de route
    frames[nb_frames].serNum = SER_SERIE;
    frames[nb_frames].frameSize = 2;
    frames[nb_frames].frame[0] = 0x03;
    frames[nb_frames].frame[1] = get_commande_feux_route() ? 0x01 : 0x00;
    nb_frames++;

    // 4. Clignotant droit
    frames[nb_frames].serNum = SER_SERIE;
    frames[nb_frames].frameSize = 2;
    frames[nb_frames].frame[0] = 0x04;
    frames[nb_frames].frame[1] = get_commande_clignotant_droit() ? 0x01 : 0x00;
    nb_frames++;

    // 5. Clignotant gauche
    frames[nb_frames].serNum = SER_SERIE;
    frames[nb_frames].frameSize = 2;
    frames[nb_frames].frame[0] = 0x05;
    frames[nb_frames].frame[1] = get_commande_clignotant_gauche() ? 0x01 : 0x00;
    nb_frames++;

    drv_write_ser(identifiant_driver, frames, nb_frames);
}

void verifier_numero_de_trame(v_uint8_t numeroRecu, v_uint8_t *pTrameAttendue) {
    if (numeroRecu == 0) {
        *pTrameAttendue = 1;
        return;
    }

    // Vérification par rapport à la valeur pointée
    if (numeroRecu != *pTrameAttendue) {
        printf("[LOG] Problème séquence UDP : Reçu %d, Attendu %d\n", 
                numeroRecu, *pTrameAttendue);
    }
    *pTrameAttendue = (numeroRecu % 100) + 1;
}

/**
 * @brief Point d'entrée du programme.
 *
 * @param argc Nombre d'arguments de la ligne de commande
 * @param argv Tableau des arguments de la ligne de commande
 *
 * @return int Code de retour du programme
 */
int main(void) {
    v_int32_t identifiant_driver = 0;
    v_uint8_t trame_udp[DRV_UDP_100MS_FRAME_SIZE];
    serial_frame_t donnees_serie[DRV_MAX_FRAMES];
    v_uint32_t nb_trames_serie = 0;
    v_int32_t code_erreur = 0;
    v_uint8_t trame_udp_a_envoyer[DRV_UDP_200MS_FRAME_SIZE] = {0};
    v_uint8_t prochaine_trame_attendue = 1;
    v_int32_t compteur_cycle = 0;

    // Initialisation
    identifiant_driver = drv_open();

    if(identifiant_driver == DRV_ERROR) {
        printf("Erreur : impossible d'ouvrir le driver (DRV_ERROR)\n");
        return -1;
    }
    else if(identifiant_driver == DRV_VER_MISMATCH) {
        printf("Erreur : incompatibilité de version du driver (DRV_VER_MISMATCH)\n");
        return -1;
    }

    while(1) {
        // Lecture de la trame UDP 100 ms
        code_erreur = drv_read_udp_100ms(identifiant_driver, trame_udp);

        if(code_erreur == DRV_SUCCESS) {
            // Vérification et log si saut de trame
            verifier_numero_de_trame(trame_udp[0], &prochaine_trame_attendue);
            
            // Décodage des données reçues
            decoder_trame_udp(trame_udp);
        }


        // Lecture de la trame série 500 ms
        if(compteur_cycle >= 5) {
            code_erreur = drv_read_ser(identifiant_driver, donnees_serie, &nb_trames_serie);

            if (code_erreur == DRV_SUCCESS && nb_trames_serie > 0) {
                for(v_uint32_t i = 0; i < nb_trames_serie; i++) {
                    decoder_trame_serie(donnees_serie[i].frame[0]);
                }
            }
        }

        // TODO : MACHINE D'ÉTAT


        // Encodage et Envoi de la trame UDP
        encoder_trame_udp(trame_udp_a_envoyer);
        drv_write_udp_200ms(identifiant_driver, trame_udp_a_envoyer);

        // Encodage et Ecriture de la trame série
        if (compteur_cycle >= 5) {
            envoyer_trame_serie(identifiant_driver);
            compteur_cycle = 0;
        }
        compteur_cycle++;
    }
    // Fermeture du driver
    drv_close(identifiant_driver);
    return 0;
}