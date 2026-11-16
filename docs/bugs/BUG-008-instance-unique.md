# BUG-008 — Plusieurs instances peuvent démarrer

- **Statut :** Signalé
- **Gravité :** Normale
- **Composant :** Application
- **Signalé le :** 2026-11-16
- **Version et environnement :** 0.1.13, Windows 11.
- **Fréquence :** à confirmer.
- **Tâche liée :** aucune

## Reproduction

Lancer Plop-Wallpaper deux fois dans la même session Windows.

## Résultat observé

Le démarrage de la version 0.1.13 ne vérifie pas si une instance est déjà active.

## Résultat attendu

Une seule instance reste active dans la session Windows, conformément au [périmètre produit](../01-PRODUIT.md). Après la fermeture de cette instance, l’application peut être relancée.

## Diagnostic

Aucun verrou d’instance n’est acquis au démarrage.

## Correction et vérification

Le test de non-régression réserve le mutex de l’application avant de lancer l’exécutable et vérifie que le processus se termine normalement.
