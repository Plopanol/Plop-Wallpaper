---
name: add-memory
description: Consigne dans la mémoire commune de _modele-projet un blocage qui empêche de poursuivre ou valider le travail, ou ajoute une leçon transverse. Met à jour la fiche et les index du coffre.
---

# Ajouter à la mémoire commune

1. Depuis la racine du projet, ouvrir `../_modele-projet/memory/INDEX.md`. Depuis le modèle, ouvrir `memory/INDEX.md`. Chercher une fiche existante et la compléter si elle couvre le même sujet.
2. Créer au besoin `memory/<theme>/<titre-court>.md` selon `memory/_gabarit-lecon.md`. Pour un blocage, noter le projet, le contexte, le symptôme, les tentatives et leurs résultats, puis le prochain essai ou le prérequis manquant. Distinguer les constats des hypothèses. Mettre `statut: partiel` tant que la résolution reste à vérifier, puis `verifie` après validation. Retirer les secrets des extraits et utiliser des liens relatifs.
3. Mettre à jour l’entrée de la fiche sous le thème de `memory/INDEX.md` avec le résultat connu ou le blocage restant. Ajouter au besoin le lien dans `memory/projets/<projet>.md` et référencer cette page dans l’index principal. Garder le suivi de la tâche ou de l’anomalie dans son registre de projet.
4. Depuis `_modele-projet`, lancer `pwsh -File outils/Test-Docs.ps1 -Path memory`. Vérifier que le diff couvre uniquement la fiche et ses index, puis commiter ces changements dans ce dépôt avec `docs(memoire): …`.
5. Dans le compte rendu, indiquer la fiche et le prochain prérequis si le blocage persiste. Si le coffre est inaccessible, signaler ce qui empêche l’écriture et conserver le constat dans la fiche de travail du projet si elle est accessible.
