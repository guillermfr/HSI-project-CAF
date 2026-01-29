/**
 * @file donnees.h
 * @brief Définition des types et énumérations
 * @date {{ date }}
 */

#ifndef DONNEES_H
#define DONNEES_H

#include "types.h"

/**
 * @brief structure du BCGV
 * @details Contient les informations nécessaires pour la gestion du BCGV
 */
typedef struct donnees_bcgv_t {
    {% for item in donnees %}
        {{ item.type }} {{ item.nom }}; /**< {{ item.commentaire }} */
    {% endfor %}
            
} donnees_bcgv_t;


//Getters
{% for item in donnees %}
    /**
    * @brief Getter pour {{ item.nom }}
    * @details Type: {{ item.type }} | Dimension: {{ item.dimension }} | Valeur d'initialisation : {{ item.init}}
    */
    {{ item.type }} get_{{ item.nom }}();
{% endfor %}

//Setters
{% for item in donnees %}
    /**
    * @brief Setter pour {{ item.nom }}
    * @details Type: {{ item.type }} | Dimension: {{ item.dimension }} | Valeur d'initialisation : {{ item.init}}
    */
    void set_{{ item.nom }}({{ item.type }} value);
{% endfor %}



#endif /* DONNEES_H */