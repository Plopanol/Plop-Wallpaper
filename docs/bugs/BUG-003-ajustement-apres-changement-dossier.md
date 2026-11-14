# BUG-003 — Ajustement inactif après un changement de dossier

- **Statut :** Signalé
- **Gravité :** Normale
- **Composant :** Fonds d’écran
- **Signalé le :** 2026-11-14
- **Version et environnement :** 0.1.4, Windows 11, constat en revue de code.
- **Fréquence :** Systématique d’après la revue.
- **Tâche liée :** [TASK-006](../tasks/TASK-006-preparer-la-publication.md)

## Reproduction

Sur un écran portrait, appliquer une image depuis un dossier, choisir un autre dossier, puis changer l’ajustement avant d’appliquer une nouvelle image.

## Résultat observé

D’après la revue, le réglage est enregistré et le fond affiché garde son ancien ajustement.

## Résultat attendu

L’ajustement s’applique au fond affiché. La prochaine image est choisie dans le nouveau dossier.

## Diagnostic

Le choix du dossier efface la dernière image de rotation, qui sert aussi à retrouver le fond affiché.

## Correction et vérification

À confirmer sur un écran portrait.
