# Roadmap — Plop-Wallpaper

Cette page fixe l’ordre des travaux. Les statuts figurent dans l’[index des tâches](tasks/INDEX.md).

## Phase 0 — Cadrage

| Ordre | Livrable | Dépendances |
|---|---|---|
| [TASK-001](tasks/TASK-001-initialiser-le-projet.md) | Socle documentaire synchronisé et vérifié | Aucune |
| [TASK-008](tasks/TASK-008-nettoyer-depot.md) | Nettoyage des artefacts de développement | Aucune |

Jalon : les contrôles documentaires et la synchronisation passent.

## Phase 1 — Fonds manuels par écran

| Ordre | Livrable | Dépendances |
|---|---|---|
| [TASK-002](tasks/TASK-002-changer-les-fonds-manuellement.md) | Détection, association de dossiers et changement ciblé | TASK-001 |

Jalon : chacun des deux écrans reçoit manuellement une image de son dossier.

## Phase 2 — Rotation indépendante

Timers Win32 par moniteur, intervalles, séquences, historique et pause.

| Ordre | Livrable | Dépendances |
|---|---|---|
| [TASK-003](tasks/TASK-003-configurer-les-fonds-et-le-demarrage.md) | Ajustement des images, cadence générale de rotation et lancement avec Windows | TASK-002 |

Jalon : deux écrans changent de fond à des intervalles distincts.

## Phase 3 — Interface et zone de notification

Interface de configuration et commandes en arrière-plan, avec restauration de l’icône après redémarrage d’Explorer.

| Ordre | Livrable | Dépendances |
|---|---|---|
| [TASK-004](tasks/TASK-004-organiser-et-thematiser-interface.md) | Rubriques, image par écran et thème Windows | TASK-003 |
| [TASK-005](tasks/TASK-005-notification-et-rotation.md) | Réduction dans la zone de notification et ordre de rotation aléatoire | TASK-004 |
| [TASK-007](tasks/TASK-007-fermer-en-notification.md) | Fermeture dans la barre de notification sur option | TASK-005 |

Jalon : les rotations continuent lorsque la fenêtre est masquée.

## Phase 4 — Persistance et événements Windows

Configuration INI, démarrage, topologie des moniteurs, veille et session.

| Ordre | Livrable | Dépendances |
|---|---|---|
| [TASK-006](tasks/TASK-006-preparer-la-publication.md) | Corrections de la revue, tableau des écrans, double-clic et licence MIT | TASK-005 |
| [TASK-009](tasks/TASK-009-journal-erreurs.md) | Journal local des erreurs avec rotation | Aucune |
| [TASK-010](tasks/TASK-010-corriger-les-chemins-de-stockage.md) | Démarrage automatique sans raccourci dans le profil Windows | Aucune |

Jalon : les associations survivent au redémarrage et à la reconnexion d’un écran.

## Phase 5 — Mesures et livraison

Validation sur deux écrans, mesures CPU, mémoire et disque, puis construction portable sur demande.

Jalon : les mesures sont consignées et la procédure de production du paquet est vérifiée.

## Prochaine tâche

[TASK-010 — Éviter le raccourci dans le profil Windows](tasks/TASK-010-corriger-les-chemins-de-stockage.md)
