# TASK-005 — Réduire en notification et varier la rotation

- **Statut :** En cours
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
| 2 | Ordre aléatoire et persistance du réglage | Aucune | Compilation, documentation, essai manuel | En cours |

## Journal des lots

- 2026-11-14 — Lot 1. Réduction, menu et rotation depuis l’icône implémentés. Test-Docs réussi. Essai manuel prévu après le lot 2.

## Validation et preuves

Non exécutée.
