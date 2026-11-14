# TASK-006 — Préparer la publication

- **Statut :** En cours
- **Phase :** Phase 4 — Persistance et événements Windows
- **Priorité :** Haute
- **Dépendances :** TASK-005
- **Dernière mise à jour :** 2026-11-14
- **Fichiers concernés :** `src/`, `LICENSE`, `tools/livrable.ps1`, documentation associée.
- **Objectif :** Corriger les anomalies de la revue, rendre les réglages par écran lisibles et appliquer la licence MIT au projet.

## Critères d’acceptation

- [x] Le branchement ou le débranchement d’un écran actualise la liste sans relancer l’application. La rotation cible les écrans présents et la reconnexion restaure les réglages de l’écran.
- [x] Après un changement de dossier, le fond encore affiché peut être réajusté en portrait. La prochaine rotation utilise le nouveau dossier.
- [ ] Le double-clic sur l’icône de notification restaure la fenêtre sans changer de fond. Le clic simple conserve son action actuelle.
- [ ] Le tableau affiche une ligne par écran et les colonnes Nom écran, Mode actuel (Rotation ou Unique), Aléatoire/Fixe, Dossier et Ajustement. La sélection pilote les réglages de l’écran et les cellules suivent les changements.
- [ ] Le fichier `LICENSE` contient la licence MIT. Le script inclut et vérifie ce fichier au prochain conditionnement.
- [ ] La compilation Release x64, les tests de non-régression, le contrôle documentaire complet et le scan de publication passent. Les parcours visibles sont vérifiés manuellement sur deux écrans.

## Exécution

| Lot | Contenu | Préconditions | Contrôles | État |
|---|---|---|---|---|
| 1 | Licence MIT et inclusion dans le paquet | Attribution du copyright validée | Contrôle documentaire et lecture du script | À faire |
| 2 | Actualiser les écrans et conserver le fond affiché durant le changement de dossier | Plan validé | Compilation et tests de non-régression | Terminé |
| 3 | Restaurer la fenêtre au double-clic et présenter les réglages par écran | Lot 2 | Compilation, contrôles complets et essai manuel | À faire |

## Journal des lots

2026-11-14 — Lot 2 implémenté. Release et Debug x64 de validation construits, deux tests CTest réussis. Les deux premiers critères sont validés par l’utilisateur.

## Validation et preuves

2026-11-14 — L’utilisateur a validé les parcours de branchement et de changement de dossier sur le Debug x64 du commit `39b206a`.

## Obstacles et reprise

Plan du lot 2 validé le 2026-11-14. Validation manuelle du lot 2 consignée. Anomalies liées : [BUG-002](../bugs/BUG-002-liste-ecrans-obsolete.md) et [BUG-003](../bugs/BUG-003-ajustement-apres-changement-dossier.md).
