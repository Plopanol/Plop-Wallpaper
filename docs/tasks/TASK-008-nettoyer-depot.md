# TASK-008 — Nettoyer les artefacts du dépôt

- **Statut :** Terminée
- **Phase :** Phase 0 — Cadrage
- **Priorité :** Normale
- **Dépendances :** aucune
- **Dernière mise à jour :** 2026-11-15

## Résultat attendu

Un script PowerShell 7 inventorie puis supprime sur demande les sorties de compilation, caches et journaux locaux. Le skill `nettoyer-repo` appelle le script.

## Critères d’acceptation

- [x] L’inventaire ne supprime rien et `-Execute` nettoie les artefacts ciblés.
- [x] Les fichiers suivis, réglages, données, livrables et liens sont conservés.
- [x] Le skill et les conventions indiquent la commande.

## Validation et preuves

- 2026-11-15, Windows et PowerShell 7. Inventaire du dépôt réel, puis essai en dépôt Git temporaire. L’inventaire et `-WhatIf` conservent tous les fichiers. `-Execute` supprime les artefacts attendus et préserve notamment une jonction, sa cible et un chemin suivi contenant un accent. La seconde exécution ne trouve plus de cible.
- 2026-11-15. En-tête et commande du skill vérifiés directement. Le validateur Python fourni est indisponible faute de PyYAML.
- 2026-11-15. Contrôle documentaire et synchronisation du socle réussis. Suppression dans le dépôt réel non exécutée.
