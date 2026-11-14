# TASK-005 — Réduire en notification et varier la rotation

- **Statut :** Terminée
- **Phase :** Phase 3 — Interface et zone de notification
- **Priorité :** Normale
- **Dépendances :** TASK-004
- **Dernière mise à jour :** 2026-11-14
- **Objectif :** contrôler les rotations depuis la zone de notification et proposer un ordre aléatoire.
- **Critères d’acceptation :**
  - [x] Réduire masque la fenêtre et affiche une icône. Le menu de l’icône permet de rouvrir ou de quitter l’application. L’icône revient après un redémarrage d’Explorer.
  - [x] Un clic gauche déclenche la rotation des écrans participants lorsque la rotation générale est active. La cadence du minuteur est conservée.
  - [x] L’option **Aléatoire** est conservée au redémarrage et s’applique au minuteur comme au clic sur l’icône. Une image différente est utilisée dès qu’une alternative lisible existe.
  - [x] Compilation Debug, contrôle documentaire et parcours manuel sur Windows 11 consignés.

## Exécution

| Lot | Contenu | Préconditions | Contrôles | État |
|---|---|---|---|---|
| 1 | Réduction, restauration et rotation depuis l’icône | TASK-004 | Compilation, documentation, essai manuel | Terminée |
| 2 | Ordre aléatoire et persistance du réglage | Aucune | Compilation, documentation, essai manuel | Terminée |

## Journal des lots

- 2026-11-14 — Lot 1. Réduction, menu et rotation depuis l’icône implémentés dans le commit `1548927`.
- 2026-11-14 — Lot 2. Ordre aléatoire partagé par le bouton, le minuteur et le clic sur l’icône. Réglage enregistré dans l’INI.
- 2026-11-14 — Suivi de validation. Icône de l’application personnalisée avec le monogramme P/W. Correction des titres sombres suivie dans [BUG-001](../bugs/BUG-001-titres-rubriques-sombres.md).

## Validation et preuves

- 2026-11-14 — Windows x64, MSVC. `cmake --build build --config Debug` réussi sans avertissement pour les deux lots après reconstruction séquentielle du lot 1.
- 2026-11-14 — Test-Docs et `git diff --check` réussis.
- 2026-11-14 — L’utilisateur a confirmé la validation manuelle du build Debug sur Windows 11 pour la réduction, la rotation au clic, l’ordre aléatoire, les titres verts et l’icône P/W.
- 2026-11-14 — Test-Docs et Sync-Commun réussis pour la clôture en version 0.1.4. Compilation Release x64 et vérification du paquet réussies.
