# BUG-002 — Liste des écrans obsolète après un branchement

- **Statut :** Clos
- **Gravité :** Normale
- **Composant :** Moniteurs
- **Signalé le :** 2026-11-14
- **Version et environnement :** 0.1.4, Windows 11, constat en revue de code.
- **Fréquence :** Systématique d’après la revue.
- **Tâche liée :** [TASK-006](../tasks/TASK-006-preparer-la-publication.md)

## Reproduction

Ouvrir l’application, débrancher un écran puis le reconnecter.

## Résultat observé

D’après la revue, la liste reste celle du démarrage et la rotation peut encore cibler l’écran déconnecté.

## Résultat attendu

La liste et les cibles de rotation suivent la topologie, conformément à l’[architecture](../02-ARCHITECTURE.md).

## Diagnostic

La lecture des moniteurs se fait au démarrage et la fenêtre ne traite pas le changement de topologie.

## Correction et vérification

La liste est réénumérée après les notifications d’affichage et de périphériques. La sélection et les réglages des écrans présents sont associés à leur identifiant. Les écrans reconnectés retrouvent leurs réglages enregistrés.

Le test CTest BUG-002 simule une séquence à deux, un puis deux écrans avec un faux service de fonds d’écran. Il échoue en 0.1.4 car la réénumération ajoute les résultats à la liste précédente.

- 2026-11-14 — CTest en échec avant correction, réussi après correction en Release et Debug x64. Contrôle documentaire complet réussi.
- Debug standard en échec sur la chaîne MSVC (LNK1101). Debug local construit avec les symboles embarqués et sans édition de liens incrémentielle.
- 2026-11-14 — Essai manuel validé par l’utilisateur sur le Debug x64 du commit `39b206a` de TASK-006 : liste actualisée au débranchement, rotation limitée à l’écran présent et réglages restaurés à la reconnexion.
- 2026-11-14 — Vérification de clôture sur `main` (`0cb83f1`, version 0.1.6) après fusion de TASK-006 : reconstruction complète en Debug x64 et quatre tests CTest réussis, dont BUG-002. La validation manuelle du parcours est consignée ci-dessus.
