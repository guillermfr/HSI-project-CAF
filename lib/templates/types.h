/**
 * @file types.h
 * @brief Définition des types et énumérations
 * @date {{ date }}
 */

#ifndef TYPES_H 
#define TYPES_H

#include <stdint.h>

{% for item in types %}
/**
 * @brief {{ item.commentaire }}
 * @details Type: {{ item.type }} {% if item.domaine %}| Domaine: {{ item.domaine }}{% endif %}
 */
{% if item.type == 'atom' -%}
typedef {{ item.declaration }} {{ item.nom }};

{% elif item.type == 'enum' -%}
typedef enum {
    {%- for enum_item in item.declaration %}
    {{ enum_item.nom }} = {{ enum_item.valeur }}, /**< {{ enum_item.commentaire }} */
    {%- endfor %}
} {{ item.nom }};
{%- endif %}

{% endfor %}

#endif TYPES_H