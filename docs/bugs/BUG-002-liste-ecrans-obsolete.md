# BUG-002 — Liste des écrans obsolète après un branchement

- **Statut :** Signalé
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

À confirmer sur deux écrans.
