/**
 * @file define.h
 * @brief Définition des defines pour les domaines des variables
 * @date {{ date }}
 */

#ifndef DEFINE_H 
#define DEFINE_H

//TODO: mettre des noms spéciaux pour les booléens

{% for item in donnees -%}
    {%- set limites = item.domaine.split('..') -%}
/**
 * @brief Domaine de {{ item.nom }} : {{ item.commentaire }}
 */
#define {{ item.nom|upper }}_MIN ({{ limites[0] }})
#define {{ item.nom|upper }}_MAX ({{ limites[1] }})

{% endfor %}
/**
 * @brief Valeur représentant une commande activée
 */
#define CMD_ACTIVEE 1u

/**
 * @brief Valeur représentant une commande éteinte
 */
#define CMD_ETEINTE 0u

/**
 * @brief Durée maximale pour l'acquittement dans la FSM des feux classiques
 */
#define ACQ_FEUX_CLASSIQUES 1.0

/**
 * @brief Durée maximale pour l'acquittement dans la FSM des clignotants et warnings
 */
#define ACQ_FEUX_CLIGNOTANTS_WARNINGS 1.0

/**
 * @brief Durée maximale pour le timer dans la FSM des essuie-glaces et du lave glace 
 */
#define TIMER_ESSUIE_GLACES_LAVE_GLACE 2.0

#endif /* DEFINE_H */