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

### Question 4

### Question 5

En ce qui concerne l'automatisation de la compilation des librairie, nous avons suivie le principe suivant:

- dans le dossier `lib` : un makefile qui permet la compilation des variables et strucutres de la question 1 et la compilation de la librairie statique de la question 2.

- dans le dossier `app` : un autre makefile qui permet de créer l'éxécutable qui s'occupera de la gestion de BCGV.
- À la racine du projet : un dernier makefile qui appelle les deux autres makefiles.

Le fichier [lancement.sh](lancement.sh) permet de lancer notre application et le driver ensemble.

### Question 6

### Question 7

### Question 8

Dans cette question, nous avons pu retrouver dans les parties de code que nous avions précédemment faire afin de mettre au propre le main pour suivre le déroulement expliqué dans le diagramme.