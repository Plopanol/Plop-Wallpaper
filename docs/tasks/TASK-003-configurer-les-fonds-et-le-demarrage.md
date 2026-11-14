# TASK-003 — Configurer les fonds et le démarrage

- **Statut :** En cours
- **Phase :** Phase 2 — Rotation indépendante
- **Priorité :** Normale
- **Dépendances :** TASK-002
- **Dernière mise à jour :** 2026-11-14
- **Fichiers concernés :** code de gestion des fonds, de rotation, de configuration et de l’interface Win32.
- **Objectif :** régler l’affichage des images selon l’orientation de l’écran, la cadence générale de changement des fonds et le lancement de l’application à l’ouverture de session Windows. Voir le [périmètre produit](../01-PRODUIT.md) et l’[architecture](../02-ARCHITECTURE.md).
- **Travail à réaliser :** ajouter les réglages à l’interface et enregistrer les préférences pour les retrouver après relance.
- **Critères d’acceptation :**
  - [ ] Sur l’écran paysage, le mode « Étirer » adapte l’image à la largeur et à la hauteur de la résolution de l’écran, même si les proportions diffèrent.
  - [ ] Sur l’écran portrait, l’utilisateur peut choisir « Ajuster à la largeur » ou « Ajuster à la hauteur ». Chaque mode fait correspondre la dimension choisie à celle de l’écran en conservant les proportions de l’image.
  - [ ] L’utilisateur peut activer une rotation générale et saisir un intervalle de X minutes. Une fois l’option activée, le fond de chacun des écrans participant à la rotation change à cette cadence.
  - [ ] Quand l’option de lancement est activée, l’application s’ouvre à la prochaine ouverture de session Windows. Quand elle est désactivée, l’application ne se lance plus automatiquement.
  - [ ] Les réglages choisis sont restaurés après fermeture et relance de l’application.
  - [ ] Les contrôles sont exécutés et consignés dans la fiche. La fiche, l’index et la roadmap sont synchronisés et leurs liens sont vérifiés.
- **Risques :** vérifier qu’un réglage d’ajustement appliqué à un moniteur ne modifie pas le rendu de l’autre.
- **Tests prévus :** construire l’application, puis faire valider manuellement les modes avec des images de proportions différentes sur deux moniteurs, la rotation sur plusieurs intervalles, la persistance après relance et les deux états du lancement automatique après réouverture de session. Exécuter le contrôle complet du projet avant fusion.

## Exécution

| Lot | Contenu | Préconditions | Contrôles | État |
|---|---|---|---|---|
| 1 | Modes d’ajustement paysage et portrait | TASK-002 | Vérifier les trois modes sur deux écrans | À valider |
| 2 | Réglage général de rotation en minutes | TASK-002 | Vérifier l’activation, la cadence et la désactivation | À faire |
| 3 | Option de lancement avec Windows | Aucune | Vérifier l’ouverture de session avec l’option activée puis désactivée | À faire |

## Journal des lots

- 2026-11-14 — Lot 1 compilé en Debug x64. Le contrôle documentaire passe. Essai sur deux écrans en attente.

## Validation et preuves

Non exécutée.

## Obstacles et reprise

Aucun obstacle identifié.
