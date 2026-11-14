# TASK-004 — Organiser et thématiser l’interface

- **Statut :** En cours
- **Phase :** Phase 3 — Interface et zone de notification
- **Priorité :** Normale
- **Dépendances :** TASK-003
- **Dernière mise à jour :** 2026-11-14
- **Objectif :** regrouper les réglages par usage, choisir une image par écran et suivre le thème des applications Windows.
- **Critères d’acceptation :**
  - [ ] Un sélecteur d’écran commun dessert les rubriques Rotation, Unique et Générale.
  - [ ] Rotation contient le dossier, la participation de l’écran, l’activation générale et l’intervalle.
  - [ ] Unique applique une image à l’écran sélectionné et mémorise son chemin. Un écran participant reprend la rotation à la prochaine échéance.
  - [ ] Générale contient l’ajustement du fond et le lancement avec Windows.
  - [ ] La fenêtre suit le mode clair ou sombre des applications Windows, y compris pendant son utilisation.
  - [ ] La compilation et les contrôles documentaires passent. La validation manuelle est consignée.

## Exécution

| Lot | Contenu | Préconditions | Contrôles | État |
|---|---|---|---|---|
| 1 | Rubriques et choix d’une image par écran | TASK-003 | Vérifier l’application ciblée, la persistance et la reprise de rotation | À valider |
| 2 | Thèmes clair et sombre | Lot 1 | Changer le mode Windows pendant que la fenêtre est ouverte | À faire |

## Validation et preuves

- 2026-11-14 — Lot 1 compilé en Debug x64. Essai manuel en attente.
