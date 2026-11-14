# TASK-002 — Changer les fonds manuellement

- **Statut :** En cours
- **Phase :** Phase 1 — Fonds manuels par écran
- **Priorité :** Haute
- **Dépendances :** TASK-001
- **Dernière mise à jour :** 2026-11-14

## Résultat attendu

Construire une première application Win32 x64 avec CMake et MSVC pour associer des dossiers d’images à deux écrans et appliquer leurs fonds manuellement. Documenter la compilation et le lancement dans le README.

## Critères d'acceptation

- [ ] Sur Windows 11 avec deux écrans d’orientations différentes, l’application affiche leur nom, leur résolution et leur orientation.
- [ ] L’utilisateur associe un dossier distinct à chaque écran.
- [ ] En déclenchant un changement sur chaque écran à tour de rôle, l’image provient du dossier associé et le fond de l’autre écran reste identique.

## Validation et preuves

- 2026-11-14 — Compilation Debug x64 du lot 1 réussie sans avertissement avec CMake et MSVC 19.51.

## Exécution

| Lot | Contenu | Préconditions | Contrôles | État |
|---|---|---|---|---|
| 1 | Application Win32 x64 et affichage des écrans | TASK-001 | Compilation Debug | Terminé |
| 2 | Dossiers par écran, application ciblée et README | Lot 1 | Compilation et parcours manuel sur deux écrans | À faire |

## Obstacles et reprise

Utiliser la [chaîne de compilation validée](../adr/ADR-001-chaine-msvc.md).
