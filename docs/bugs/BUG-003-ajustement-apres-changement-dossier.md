# BUG-003 — Ajustement inactif après un changement de dossier

- **Statut :** Corrigé à vérifier
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

Le changement de dossier réinitialise le curseur de rotation et conserve le chemin du dernier fond de rotation pour l’ajustement. Le chemin est mémorisé dans la configuration et le premier fond appliqué depuis le nouveau dossier le remplace.

Le test CTest BUG-003 reproduit la perte du chemin affiché en 0.1.4. Il vérifie les changements successifs de dossier, la prochaine image appliquée et le mode Unique.

- 2026-11-14 — CTest en échec avant correction, réussi après correction en Release et Debug x64. Contrôle documentaire complet réussi.
- Debug standard en échec sur la chaîne MSVC (LNK1101). Debug local construit avec les symboles embarqués et sans édition de liens incrémentielle.
- Essai manuel sur deux écrans en attente.
