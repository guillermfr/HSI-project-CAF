# HSI-project-CAF

### Membre du groupe
- Alif ALI SEKANDER
- François GUILLERM
- Célian MIGNOT

## Premier lancement
Afin de lancer notre projet, vous devez d'abord compiler l'exécutable à l'aide de la commande suivante à la racine du projet :
```bash
make
```

## Démarrage de l'interface graphique

L'interface graphique utilise un conteneur docker hébergant Grafana.

Pour démarrer le conteneur, il suffit de lancer la commande suivante dans le sous-dossier `docker`:
```bash
docker-compose up
```
Une fois démarré , à l'aide d'un navigateur internet, il faut se rendre à l'adresse suivante : `http://localhost:3000/`.


## Démarrage de la simulation

Pour démarrer la simulation, il suffit de faire la commande suivante à la racine du projet :
```bash
./lancement.sh
```

***

## Démarche

### Question 1

Nous avons décidé d'utilisé un [fichier json](lib/structure.json) qui nous permet de lister toutes les variables et structures qui seront générées automatiquement via un [script python](lib/generation_lib.py) et de la librairie [Jinja2](https://jinja.palletsprojects.com/en/stable/).

Jinja2 permet de générer du code à partir de template. Nous avons alors créé des templates pours les différents types de fichiers que l'on voulait créer ([define.h](lib/templates/define.h), [donnees.c](lib/templates/donnees.c), [donnees.h](lib/templates/donnees.h) et [types.h](lib/templates/types.h)).

### Question 2

Afin de générer la librairie statisque, nous utilisons un script shell qui permet de créer une librairie statistique, [lib.a](lib/lib.a), à partir des fichiers générés par le script python de la question 1.

Le [fichier makefile](lib/makefile) permet d'automatiser la compilation de la librairie statique, en particulier avec l'utilisation de la commande `ar rcs $(LIB_NAME) *.o`.

### Question 3

Nous avons bien utilisé et testé les fonctions `drv_open` et `drv_read_udp_100ms`. Ces fonctions nous permettent d'afficher la trame envooyée en UDP toutes les les 100ms.

### Question 4

Nous avons créé 3 fichiers, ainsi que leur 3 fichiers d'entête, qui contiennent chacun une finite state machine (FSM) : la [FSM pour les feux classiques](fsm/fsm_feux_classiques.c), la [FSM pour les clignotants et warnings](fsm/fsm_feux_clignotants_warnings.c) et la [FSM pour les essuie-glaces et le lave glace](fsm/fsm_essuie-glaces_lave_glace.c).

Chaque FSM reprend le schéma présent dans l'énoncé et gère les différentes transitions entre les états en fonction des entrées.

### Question 5

En ce qui concerne l'automatisation de la compilation des librairie, nous avons suivie le principe suivant:

- dans le dossier `lib` : un makefile qui permet la compilation des variables et strucutres de la question 1 et la compilation de la librairie statique de la question 2.

- dans le dossier `app` : un autre makefile qui permet de créer l'éxécutable qui s'occupera de la gestion de BCGV.

- À la racine du projet : un dernier makefile qui appelle les deux autres makefiles.

Le fichier [lancement.sh](lancement.sh) permet de lancer notre application et le driver ensemble.

### Question 6

À partir de cette question, nous nous sommes de nouveau intéressé à la lecture des trames. À l'aide des tableau présent dans l'annonce, nous avons pus traduire correctement les bits que nous lisions à l'aide question 3. Nous avons créer deux fonctions, une pour l'UDP (`decoder_trame_udp`) et une pour la série (`decoder_trame_serie`).

À l'intérieur des ces fonctions, après les avoir décoder, stockons les valeurs des variables grâces aux setters crées automatiquement dans la question 1

### Question 7

Dans cette question, il s'affit de faire l'inverse de la question précédente. Nous avons crées une fonction pour chaque trame à encoder (`encoder_trame_udp` et `envoyer_trame_serie`) afin de permettre de préparer des trames pour l'envoyer vers le tableau de bord.  

### Question 8

Dans cette question, nous avons pu retrouver dans les parties de code que nous avions précédemment faire afin de mettre au propre le main pour suivre le déroulement expliqué dans le diagramme.