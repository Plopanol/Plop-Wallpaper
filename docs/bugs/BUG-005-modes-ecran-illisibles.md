# BUG-005 — Options du mode de l’écran illisibles

- **Statut :** Clos
- **Gravité :** Normale
- **Composant :** Interface
- **Signalé le :** 2026-11-14
- **Version et environnement :** Debug x64 0.1.4, commit `902152d`, Windows 11 en mode sombre.
- **Tâche liée :** [TASK-006](../tasks/TASK-006-preparer-la-publication.md)

## Reproduction

En mode sombre, sélectionner un écran. Les libellés Rotation et Unique sont illisibles d’après l’essai utilisateur.

## Correction et vérification

Le dessin personnalisé des deux boutons radio utilise le vert des titres et conserve l’indicateur de sélection natif. La correction est intégrée sur la branche de TASK-006, qui introduit les options.

Pour le test manuel, vérifier que les deux libellés sont verts en mode sombre, puis basculer entre Rotation et Unique à la souris et au clavier. Vérifier l’indicateur de sélection, la rubrique affichée et la lisibilité après passage au mode clair.

2026-11-14 — Compilations Release et Debug x64, quatre tests CTest et contrôle documentaire complet réussis. Exécutable de validation dans `build/debug-contraste/Debug/PlopWallpaper.exe`.

2026-11-14 — Validation manuelle confirmée par l’utilisateur sur le Debug x64 0.1.6 reconstruit depuis `main` (`0cb83f1`). Les libellés Rotation et Unique sont lisibles en modes sombre et clair. La sélection à la souris et au clavier actualise l’indicateur et la rubrique affichée.
