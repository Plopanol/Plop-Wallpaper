# TASK-002 — Changer les fonds manuellement

- **Statut :** Terminée
- **Phase :** Phase 1 — Fonds manuels par écran
- **Priorité :** Haute
- **Dépendances :** TASK-001
- **Dernière mise à jour :** 2026-11-14

## Résultat attendu

Construire une première application Win32 x64 avec CMake et MSVC pour associer des dossiers d’images à deux écrans et appliquer leurs fonds manuellement. Documenter la compilation et le lancement dans le README.

## Critères d'acceptation

- [x] Sur Windows 11 avec deux écrans d’orientations différentes, l’application affiche leur nom, leur résolution et leur orientation.
- [x] L’utilisateur associe un dossier distinct à chaque écran.
- [x] En déclenchant un changement sur chaque écran à tour de rôle, l’image provient du dossier associé et le fond de l’autre écran reste identique.

## Validation et preuves

- 2026-11-14 — Compilation Debug x64 du lot 1 réussie sans avertissement avec CMake et MSVC 19.51.
- 2026-11-14 — Compilations Debug et Release x64 du lot 2 réussies sans avertissement avec la même chaîne. Le contrôle documentaire `Test-Docs` et `git diff --check` passent.
- 2026-11-14 — L’utilisateur confirme le bon fonctionnement à l’issue du parcours manuel ci-dessous.
- 2026-11-14 — Contrôle complet réussi. Test-Docs passe les quatre vérifications et Sync-Commun confirme la conformité du dépôt.

### Parcours manuel sur Windows 11

1. Avec un écran paysage et un écran portrait en bureau étendu, lancer l’exécutable Debug. Comparer les noms, résolutions et orientations aux paramètres d’affichage de Windows.
2. Sélectionner le premier écran et choisir un dossier contenant des images reconnaissables. Sélectionner le second et choisir un autre dossier. Revenir au premier et vérifier que son chemin est toujours affiché.
3. Cliquer sur **Changer le fond** sur le premier écran. Vérifier que l’image vient de son dossier et que le fond du second écran reste identique. Répéter sur le second écran.

## Exécution

| Lot | Contenu | Préconditions | Contrôles | État |
|---|---|---|---|---|
| 1 | Application Win32 x64 et affichage des écrans | TASK-001 | Compilation Debug | Terminé |
| 2 | Dossiers par écran, application ciblée et README | Lot 1 | Compilation et parcours manuel sur deux écrans | Terminé |

## Obstacles et reprise

Utiliser la [chaîne de compilation validée](../adr/ADR-001-chaine-msvc.md).
