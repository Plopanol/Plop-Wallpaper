# Roadmap — Plop-Wallpaper

Cette page fixe l’ordre des travaux. Les statuts figurent dans l’[index des tâches](tasks/INDEX.md).

## Phase 0 — Cadrage

| Ordre | Livrable | Dépendances |
|---|---|---|
| [TASK-001](tasks/TASK-001-initialiser-le-projet.md) | Socle documentaire synchronisé et vérifié | Aucune |

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

Jalon : les rotations continuent lorsque la fenêtre est masquée.

## Phase 4 — Persistance et événements Windows

Configuration INI, démarrage, topologie des moniteurs, veille et session.

Jalon : les associations survivent au redémarrage et à la reconnexion d’un écran.

## Phase 5 — Mesures et livraison

Validation sur deux écrans, mesures CPU, mémoire et disque, puis construction portable sur demande.

Jalon : les mesures sont consignées et la procédure de production du paquet est vérifiée.

## Prochaine tâche

Aucune tâche planifiée.
