# TASK-002 — Changer les fonds manuellement

- **Statut :** À faire
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

Non exécutée.

## Obstacles et reprise

Utiliser la [chaîne de compilation validée](../adr/ADR-001-chaine-msvc.md).
