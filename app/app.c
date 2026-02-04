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
            printf("\n");
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
    }
    
    return 0;
}