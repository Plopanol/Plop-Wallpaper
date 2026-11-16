# BUG-008 — Plusieurs instances peuvent démarrer

- **Statut :** Clos
- **Gravité :** Normale
- **Composant :** Application
- **Signalé le :** 2026-11-16
- **Version et environnement :** 0.1.13, Windows 11.
- **Fréquence :** reproduit sur 0.1.13.
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

La version 0.1.14 conserve un mutex nommé pendant toute l’exécution. Si le mutex existe déjà, le deuxième lancement se termine avant l’initialisation de l’application. Windows libère le mutex lorsque le dernier processus qui le détient se termine.

- 2026-11-16 — Le test échoue sur 0.1.13, où le processus reste actif plus de cinq secondes. Il réussit sur 0.1.14. Compilation Debug x64 et sept tests CTest réussis.
- 2026-11-16 — Contrôle documentaire complet lancé avant la fusion.
- Vérification manuelle attendue : lancer deux fois l’exécutable 0.1.14 et vérifier qu’une seule icône apparaît dans la zone de notification. Quitter depuis son menu, puis relancer et vérifier que l’icône revient.
- 2026-11-16 — Validation manuelle de l’instance unique sur la version 0.1.14 confirmée par l’utilisateur.
