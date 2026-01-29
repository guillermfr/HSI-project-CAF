/**
 * @file main.c
 * @brief Programme de lecture et d'affichage des trames UDP 100 ms via le driver DRV.
 *
 * Ce programme initialise le driver DRV, lit périodiquement une trame UDP
 * toutes les 100 ms et affiche son contenu.
 */

#include "app.h"

/**
 * @brief Point d'entrée du programme.
 *
 * @param argc Nombre d'arguments de la ligne de commande
 * @param argv Tableau des arguments de la ligne de commande
 *
 * @return int Code de retour du programme
 */
int main(void) {
    v_int32_t drvHandle = 0;
    v_uint8_t udpFrame[DRV_UDP_100MS_FRAME_SIZE];
    v_int8_t errorCode = 0;

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
        errorCode = drv_read_udp_100ms(drvHandle, udpFrame);

        if(errorCode == DRV_SUCCESS) {
            for(v_uint8_t i = 0; i < DRV_UDP_100MS_FRAME_SIZE; i++) {
                printf("Index : %u\tValeur : %u\n", i, udpFrame[i]);
            }
            printf("\n\n");
        }
        else if(errorCode == DRV_ERROR) {
            printf("Erreur lors de la lecture de la trame UDP\n");
        }
    }
    
    return 0;
}