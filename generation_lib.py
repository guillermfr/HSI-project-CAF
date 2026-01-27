import json
import sys

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
        donnees = json.loads(contenu_fichier)
        return donnees
    except json.JSONDecodeError as e:
        raise ValueError(f"Le contenu n'est pas un JSON valide : {e}")


if __name__ == "__main__":
    fichier_cible = "structure.json"
    
    try:
        contenu = lire_fichier(fichier_cible)
        resultat = extraction_json(contenu)
        print("JSON extrait avec succès :")
        print(resultat)
        
    except (FileNotFoundError, ValueError, IOError) as e:
        print(e)