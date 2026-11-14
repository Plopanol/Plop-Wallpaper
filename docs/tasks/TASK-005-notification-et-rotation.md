# TASK-005 — Réduire en notification et varier la rotation

- **Statut :** À valider
- **Phase :** Phase 3 — Interface et zone de notification
- **Priorité :** Normale
- **Dépendances :** TASK-004
- **Dernière mise à jour :** 2026-11-14
- **Objectif :** contrôler les rotations depuis la zone de notification et proposer un ordre aléatoire.
- **Critères d’acceptation :**
  - [ ] Réduire masque la fenêtre et affiche une icône. Le menu de l’icône permet de rouvrir ou de quitter l’application. L’icône revient après un redémarrage d’Explorer.
  - [ ] Un clic gauche déclenche la rotation des écrans participants lorsque la rotation générale est active. La cadence du minuteur est conservée.
  - [ ] L’option **Aléatoire** est conservée au redémarrage et s’applique au minuteur comme au clic sur l’icône. Une image différente est utilisée dès qu’une alternative lisible existe.
  - [ ] Compilation Debug, contrôle documentaire et parcours manuel sur Windows 11 consignés.

## Exécution

| Lot | Contenu | Préconditions | Contrôles | État |
|---|---|---|---|---|
| 1 | Réduction, restauration et rotation depuis l’icône | TASK-004 | Compilation, documentation, essai manuel | À valider |
| 2 | Ordre aléatoire et persistance du réglage | Aucune | Compilation, documentation, essai manuel | À valider |

## Journal des lots

- 2026-11-14 — Lot 1. Réduction, menu et rotation depuis l’icône implémentés dans le commit `1548927`.
- 2026-11-14 — Lot 2. Ordre aléatoire partagé par le bouton, le minuteur et le clic sur l’icône. Réglage enregistré dans l’INI.

## Validation et preuves

- 2026-11-14 — Windows x64, MSVC. `cmake --build build --config Debug` réussi sans avertissement pour les deux lots après reconstruction séquentielle du lot 1.
- 2026-11-14 — Test-Docs et `git diff --check` réussis.
- À exécuter sur Windows 11 avec deux écrans. Configurer deux dossiers, laisser un seul écran participant et activer la rotation. Réduire la fenêtre et vérifier son retrait de la barre des tâches. Cliquer sur l’icône et vérifier que seul l’écran participant change. Redémarrer Explorer pendant que la fenêtre est masquée et vérifier le retour de l’icône. Utiliser le clic droit pour rouvrir la fenêtre.
- Cocher **Aléatoire**, cliquer sur **Appliquer**, puis vérifier les changements au clic et au minuteur avec au moins trois images lisibles. Vérifier que la même image ne s’affiche pas deux fois de suite et que la case reste cochée après **Quitter** puis relance. Désactiver la rotation et vérifier qu’un clic rouvre la fenêtre.
- Contrôle complet, version et fusion en attente du retour de l’essai manuel.
