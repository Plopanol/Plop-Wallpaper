---
name: nettoyer-repo
description: Nettoie les sorties de compilation, caches locaux et journaux de Plop-Wallpaper. À utiliser pour inventorier ou supprimer les artefacts Debug, Release, CMake et target du dépôt.
---

# Nettoyer le dépôt

Depuis la racine du dépôt, lancer `pwsh -NoProfile -File tools/nettoyer-repo.ps1` pour afficher les cibles et les exclusions.

Si l’utilisateur a demandé la suppression, lancer `pwsh -NoProfile -File tools/nettoyer-repo.ps1 -Execute`. Pour un inventaire, s’arrêter après la première commande. Signaler les chemins conservés et les éventuels échecs.
