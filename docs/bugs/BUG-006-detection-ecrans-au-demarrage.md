# BUG-006 — Échec de détection des écrans au démarrage

- **Statut :** Signalé
- **Gravité :** Majeure
- **Composant :** Moniteurs
- **Signalé le :** 2026-11-15
- **Version et environnement :** version installée à confirmer, Windows 11.
- **Fréquence :** une occurrence signalée.
- **Tâche liée :** aucune

## Reproduction

Démarrer le PC en laissant les écrans éteints pendant le lancement de Plop-Wallpaper. Allumer les écrans et relever l’éventuel message de démarrage.

## Résultat observé

La capture montre le message « Impossible de détecter les écrans. (0x80004005) ».

## Résultat attendu

Si la détection échoue temporairement au démarrage, l’application reste active et détecte les écrans dès qu’ils deviennent disponibles. Voir la gestion des changements d’affichage dans l’[architecture](../02-ARCHITECTURE.md).

## Diagnostic

Le code affiche ce message quand l’énumération initiale des moniteurs échoue, puis quitte avant la création de la fenêtre qui reçoit les notifications d’affichage. La cause précise du retour 0x80004005 reste à établir.

## Correction et vérification

Reproduction du démarrage avec les écrans éteints à confirmer sur une version identifiée. Aucune correction à ce stade.
