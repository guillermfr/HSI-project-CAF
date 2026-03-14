#include "donnees.h"
#include "define.h"

donnees_bcgv_t dcgv;

//Getters
{% for item in donnees %}
    /**
    * @brief Getter pour {{ item.nom }}
    * @details Type: {{ item.type }} | Dimension: {{ item.dimension }} | Valeur d'initialisation : {{ item.init}}
    */
    {{ item.type }} get_{{ item.nom }}(){
        return dcgv.{{ item.nom }};
    }
{% endfor %}

//Setters
{% for item in donnees -%}
/**
 * @brief Setter pour {{ item.nom }}
 * @details Type: {{ item.type }} | Dimension: {{ item.dimension }} | Valeur d'initialisation : {{ item.init }}
 */
void set_{{ item.nom }}({{ item.type }} value){
    {% if item.type == 'numero_trame_t' -%}
        value = ((value - 1) % 100) + 1;
    {% endif -%}

    if ({{ item.type | upper }}_MIN <= value && {{ item.type | upper }}_MAX >= value) {
        dcgv.{{ item.nom }} = value;
    }
}

{% endfor %}

/**
    * @brief Fonction d'initialisation des données
    * @details Initialise toutes les données avec leurs valeurs par défaut
    */
void init_donnees(){
{% for item in donnees %}
    dcgv.{{ item.nom }} = {{ item.init }};
{% endfor %}
}