# BUG-001 — Titres de rubriques illisibles en mode sombre

- **Statut :** Corrigé à vérifier
- **Gravité :** Normale
- **Composant :** Interface
- **Signalé le :** 2026-11-14
- **Version et environnement :** 0.1.3, build Debug de TASK-005 sous Windows 11
- **Fréquence :** systématique en mode sombre d’après le signalement
- **Tâche liée :** [TASK-005](../tasks/TASK-005-notification-et-rotation.md)

## Reproduction

Activer le mode sombre des applications dans les paramètres de personnalisation de Windows et ouvrir Plop-Wallpaper.

## Résultat observé

Les titres **Rotation**, **Unique** et **Générale** s’affichent en noir sur fond sombre.

## Résultat attendu

Les trois titres sont verts et lisibles en mode sombre. Le mode clair utilise la couleur de texte du système.

## Diagnostic

Le contrôle de groupe natif dessine son titre avec le thème Windows. La couleur de texte définie par l’application ne lui est pas appliquée.

## Correction et vérification

Les trois cadres et leurs titres sont dessinés par l’application. Parcours de non-régression à exécuter après compilation : ouvrir la fenêtre en mode sombre, vérifier les trois titres verts et lisibles, puis basculer en mode clair et revenir en mode sombre sans relancer. Vérifier que les trois cadres, leurs champs et leurs boutons restent lisibles et utilisables.

Le 2026-11-14, compilation Debug dans `build`, Test-Docs et `git diff --check` réussis. La première édition de liens a échoué avec l’exécutable ouvert. La reconstruction dans `build` a réussi après sa fermeture. Essai manuel et clôture en attente.
