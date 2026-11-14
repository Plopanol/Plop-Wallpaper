# TASK-006 — Préparer la publication

- **Statut :** Terminée
- **Phase :** Phase 4 — Persistance et événements Windows
- **Priorité :** Haute
- **Dépendances :** TASK-005
- **Dernière mise à jour :** 2026-11-14
- **Fichiers concernés :** `src/`, `LICENSE`, `tools/livrable.ps1`, documentation associée.
- **Objectif :** Corriger les anomalies de la revue, rendre les réglages par écran lisibles et appliquer la licence MIT au projet.

## Critères d’acceptation

- [x] Le branchement ou le débranchement d’un écran actualise la liste sans relancer l’application. La rotation cible les écrans présents et la reconnexion restaure les réglages de l’écran.
- [x] Après un changement de dossier, le fond encore affiché peut être réajusté en portrait. La prochaine rotation utilise le nouveau dossier.
- [x] Le double-clic sur l’icône de notification restaure la fenêtre sans changer de fond. Le clic simple conserve son action actuelle.
- [x] Le tableau affiche une ligne par écran et les colonnes Nom écran, Mode actuel (Rotation ou Unique), Aléatoire/Fixe, Dossier et Ajustement. La sélection pilote les réglages de l’écran et les cellules suivent les changements.
- [x] Le mode Rotation ou Unique est mémorisé par écran et affiche uniquement sa rubrique. Les deux anciennes cases de rotation sont retirées. Seuls les écrans en mode Rotation ayant un dossier changent automatiquement de fond.
- [x] Le fichier `LICENSE` contient la licence MIT. Le script inclut et vérifie ce fichier au prochain conditionnement.
- [x] La compilation Release x64, les tests de non-régression, le contrôle documentaire complet et le scan de publication passent. Les parcours visibles sont vérifiés manuellement sur deux écrans.

## Exécution

| Lot | Contenu | Préconditions | Contrôles | État |
|---|---|---|---|---|
| 1 | Licence MIT et inclusion dans le paquet | Attribution du copyright validée | Contrôle documentaire et lecture du script | Terminé |
| 2 | Actualiser les écrans et conserver le fond affiché durant le changement de dossier | Plan validé | Compilation et tests de non-régression | Terminé |
| 3 | Restaurer la fenêtre au double-clic et présenter les réglages par écran | Lot 2 | Compilation, contrôles complets et essai manuel | Terminé |
| 4 | Choisir le mode et afficher sa rubrique par écran | Plan validé | Compilation, contrôles et essai manuel sur deux écrans | Terminé |

## Journal des lots

2026-11-14 — Lot 4 implémenté après validation du plan. Les boutons Rotation et Unique remplacent les deux cases d’activation et déterminent la rubrique visible.

2026-11-14 — Lot 1 implémenté avec l’attribution « Copyright (c) 2026 Plop ». Le script inclut la licence dans l’archive et exige sa présence lors de la vérification.

2026-11-14 — Lot 2 implémenté. Release et Debug x64 de validation construits, deux tests CTest réussis. Les deux premiers critères sont validés par l’utilisateur.

2026-11-14 — Lot 3 implémenté. Le tableau présente les réglages par écran et le double-clic annule le clic simple différé.

## Validation et preuves

2026-11-14 — Validation utilisateur reçue après livraison du Debug x64 `c63b7fd` et demande de construire le livrable. Les compilations Release et Debug x64 et les quatre tests CTest avaient réussi sur ce code.

2026-11-14 — Lot 4 : après suppression d’une référence restante à l’ancien réglage global, les compilations Release et Debug x64, les quatre tests CTest, le contrôle documentaire complet et le scan de publication avec historique passent. Validation manuelle en attente sur `build/debug-modes/Debug/PlopWallpaper.exe`.

2026-11-14 — L’utilisateur a validé les parcours de branchement et de changement de dossier sur le Debug x64 du commit `39b206a`.

2026-11-14 — Compilations Release x64 et Debug x64 réussies, trois tests CTest réussis en Release, dont l’annulation du clic différé. Essai manuel du tableau et de l’icône en attente sur `build/debug-validation/Debug/PlopWallpaper.exe`.

2026-11-14 — Scan de publication avec historique sans correspondance. Le contrôle documentaire a d’abord détecté le lien vers la tâche en attente de validation dans « Prochaine tâche ». Après correction de la roadmap, Test-Docs et Sync-Commun ont réussi.

## Obstacles et reprise

Anomalies liées : [BUG-002](../bugs/BUG-002-liste-ecrans-obsolete.md), [BUG-003](../bugs/BUG-003-ajustement-apres-changement-dossier.md), [BUG-004](../bugs/BUG-004-entetes-tableau-mode-sombre.md) et [BUG-005](../bugs/BUG-005-modes-ecran-illisibles.md).
