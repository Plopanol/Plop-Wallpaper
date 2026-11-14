# TASK-001 — Initialiser le projet

- **Statut :** Terminée
- **Phase :** Phase 0 — Cadrage
- **Priorité :** Normale
- **Dépendances :** aucune
- **Dernière mise à jour :** 2026-11-14

## Résultat attendu

Installer le socle Plop v6 et documenter le produit, l’architecture, les conventions et l’ordre des travaux.

## Critères d'acceptation

- [x] Le socle est synchronisé et les contrôles documentaires passent.
- [x] Le prompt initial conserve son contenu et figure dans l’index des archives.
- [x] La première tranche est référencée et l’écart de chaîne de compilation est consigné.

## Validation et preuves

- 2026-11-14, poste Windows — `Test-Docs.ps1` a validé le manifeste, les liens Markdown, les fiches et leurs index, ainsi que la roadmap.
- 2026-11-14, poste Windows — `Sync-Commun.ps1` sans `-Apply` a confirmé la conformité du socle. `core.hooksPath` vaut `.githooks`.
- 2026-11-14 — L’empreinte Git brute du prompt est `d03489856aba50dd0cb2123641c5d887def85231`. Une règle ciblée conserve ses fins de ligne d’origine.
- Compilation et essais applicatifs hors périmètre de cette tâche.

## Obstacles et reprise

La question Q-001 sur la compilation de TASK-002 a été résolue par [ADR-001](../adr/ADR-001-chaine-msvc.md).
