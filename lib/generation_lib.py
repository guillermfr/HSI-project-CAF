import json
import os
from datetime import datetime
from jinja2 import Environment, PackageLoader, select_autoescape

env = Environment(
    loader=PackageLoader("generation_lib", "templates"),
    autoescape=select_autoescape()
)

def lire_fichier(chemin_fichier):
    try:
        with open(chemin_fichier, 'r', encoding='utf-8') as fichier:
            return fichier.read()
    except FileNotFoundError:
        raise FileNotFoundError(f"Erreur : Le fichier '{chemin_fichier}' est introuvable.")
    except Exception as e:
        raise IOError(f"Erreur lors de la lecture du fichier : {e}")

def extraction_json(contenu_fichier):
    if not contenu_fichier:
        raise ValueError("Le contenu fourni est vide.")
    try:
        return json.loads(contenu_fichier)
    except json.JSONDecodeError as e:
        raise ValueError(f"Le contenu n'est pas un JSON valide : {e}")

def generer_fichier_types(donnees):
    output_dir = "output"
    
    os.makedirs(output_dir, exist_ok=True)

    try:
        template = env.get_template("types.h")
        
        rendu_final = template.render(
            types=donnees.get('types', []),
            date=datetime.now().strftime("%d/%m/%Y")
        )
        
        chemin_complet = os.path.join(output_dir, "types.h")
        with open(chemin_complet, "w", encoding="utf-8") as f:
            f.write(rendu_final)
            
        print(f"Fichier créé : {chemin_complet}")

    except Exception as e:
        print(f"Erreur lors de la génération Jinja2 : {e}")


if __name__ == "__main__":
    fichier_cible = "structure.json"
    
    try:
        contenu = lire_fichier(fichier_cible)
        resultat = extraction_json(contenu)
        
        print("JSON extrait avec succès.")
        
        generer_fichier_types(resultat)
        
    except (FileNotFoundError, ValueError, IOError) as e:
        print(f"STOP : {e}")