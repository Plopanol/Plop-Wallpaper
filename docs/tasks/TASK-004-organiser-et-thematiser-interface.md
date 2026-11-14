# TASK-004 — Organiser et thématiser l’interface

- **Statut :** Terminée
- **Phase :** Phase 3 — Interface et zone de notification
- **Priorité :** Normale
- **Dépendances :** TASK-003
- **Dernière mise à jour :** 2026-11-14
- **Objectif :** regrouper les réglages par usage, choisir une image par écran et suivre le thème des applications Windows.
- **Critères d’acceptation :**
  - [x] Un sélecteur d’écran commun dessert les rubriques Rotation, Unique et Générale.
  - [x] Rotation contient le dossier, la participation de l’écran, l’activation générale et l’intervalle.
  - [x] Unique applique une image à l’écran sélectionné et mémorise son chemin. Un écran participant reprend la rotation à la prochaine échéance.
  - [x] Générale contient l’ajustement du fond et le lancement avec Windows.
  - [x] La fenêtre suit le mode clair ou sombre des applications Windows, y compris pendant son utilisation.
  - [x] La compilation et les contrôles documentaires passent. La validation manuelle est consignée.

## Exécution

| Lot | Contenu | Préconditions | Contrôles | État |
|---|---|---|---|---|
| 1 | Rubriques et choix d’une image par écran | TASK-003 | Vérifier l’application ciblée, la persistance et la reprise de rotation | Validé |
| 2 | Thèmes clair et sombre | Lot 1 | Changer le mode Windows pendant que la fenêtre est ouverte | Validé |

## Validation et preuves

- 2026-11-14 — Les deux lots compilent en Debug x64.
- 2026-11-14 — L’utilisateur valide les parcours proposés pour les trois rubriques, le choix d’image par écran, la persistance, la reprise de rotation et le changement de thème Windows pendant l’utilisation.
- 2026-11-14 — Contrôle complet réussi. Test-Docs valide les quatre contrôles et Sync-Commun confirme la conformité du socle.
