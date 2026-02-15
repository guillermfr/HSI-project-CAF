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


void encoder_trame_udp(uint8_t* udpFrame) {
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

/**
 * @brief Point d'entrée du programme.
 *
 * @param argc Nombre d'arguments de la ligne de commande
 * @param argv Tableau des arguments de la ligne de commande
 *
 * @return int Code de retour du programme
 */
int main(void) {
    // TODO: changer le nom des variables pour que ce soit en français
    v_int32_t drvHandle = 0;
    v_uint8_t udpFrame[DRV_UDP_100MS_FRAME_SIZE];
    serial_frame_t serialData[DRV_MAX_FRAMES];
    v_uint32_t serialDataLen = 0;
    v_int32_t errorCode = 0;

    drvHandle = drv_open();

    if(drvHandle == DRV_ERROR) {
        printf("Erreur : impossible d'ouvrir le driver (DRV_ERROR)\n");
        return -1;
    }
    else if(drvHandle == DRV_VER_MISMATCH) {
        printf("Erreur : incompatibilité de version du driver (DRV_VER_MISMATCH)\n");
        return -1;
    }

    while(1) {
        // TODO: revoir ordre des lectures
        // LECTURE UDP
        errorCode = drv_read_udp_100ms(drvHandle, udpFrame);

        if(errorCode == DRV_SUCCESS) {
            // Décodage de la trame reçue
            decoder_trame_udp(udpFrame);
        }
        else if(errorCode == DRV_ERROR) {
            printf("Erreur lors de la lecture de la trame UDP\n");
        }

        errorCode = drv_read_ser(drvHandle, serialData, &serialDataLen);

        if (errorCode == DRV_SUCCESS && serialDataLen > 0) {
            for(v_uint32_t i = 0; i < serialDataLen; i++) {
                decoder_trame_serie(serialData[i].frame[0]);
            }
        }


        // Envoi au tableau de bord
        v_uint8_t udpFrameToSend[DRV_UDP_200MS_FRAME_SIZE] = {0};
        encoder_trame_udp(udpFrameToSend);

        drv_write_udp_200ms(drvHandle, udpFrameToSend);

    }
    
    return 0;
}