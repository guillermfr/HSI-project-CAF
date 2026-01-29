/**
 * @file define.h
 * @brief Définition des defines pour les domaines des variables
 * @date {{ date }}
 */

#ifndef DEFINE_H 
#define DEFINE_H

//TODO: mettre des noms spéciaux pour les booléens
//TODO: ajouter rebouclage pour le numéro de trame

{% for item in donnees -%}
    {%- set limites = item.domaine.split('..') -%}
/**
 * @brief Domaine de {{ item.nom }} : {{ item.commentaire }}
 */
#define {{ item.nom|upper }}_MIN ({{ limites[0] }})
#define {{ item.nom|upper }}_MAX ({{ limites[1] }})

{% endfor %}

#endif DEFINE_H