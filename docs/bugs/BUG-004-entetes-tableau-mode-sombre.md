# BUG-004 — En-têtes du tableau illisibles en mode sombre

- **Statut :** Clos
- **Gravité :** Normale
- **Composant :** Interface
- **Signalé le :** 2026-11-14
- **Version et environnement :** Debug x64 0.1.4, commit `e8dcd69`, Windows 11 en mode sombre.
- **Fréquence :** Observé lors de la validation du tableau.
- **Tâche liée :** [TASK-006](../tasks/TASK-006-preparer-la-publication.md)

## Reproduction

Activer le mode sombre de Windows et ouvrir Plop-Wallpaper. L’utilisateur signale que les intitulés des cinq colonnes sont illisibles.

## Correction

Le dessin des en-têtes en mode sombre applique le vert des titres de section sur le fond sombre des champs. Le correctif se trouve sur la branche de TASK-006, où le tableau a été introduit.

## Test de non-régression

Ouvrir le Debug x64 en mode sombre et vérifier que les cinq intitulés sont verts et lisibles. Passer au mode clair avec l’application ouverte et vérifier que les intitulés restent lisibles. Revenir au mode sombre et vérifier que le vert est rétabli.

## Contrôles

- 2026-11-14 — Release x64, trois tests CTest et contrôle documentaire complet réussis.
- 2026-11-14 — Édition de liens du Debug précédent en échec (LNK1104, exécutable verrouillé). Reconstruction Debug x64 réussie dans `build/debug-entetes`.
- 2026-11-14 — Validation manuelle confirmée par l’utilisateur sur le Debug x64 0.1.6 reconstruit depuis `main` (`0cb83f1`). Les cinq en-têtes sont verts et lisibles en mode sombre, restent lisibles en mode clair et redeviennent verts au retour en mode sombre.
