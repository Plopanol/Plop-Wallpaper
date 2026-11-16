# BUG-007 — Fenêtre affichée au démarrage

- **Statut :** Corrigé à vérifier
- **Gravité :** Normale
- **Composant :** Interface
- **Signalé le :** 2026-11-16
- **Version et environnement :** 0.1.12, Windows 11.
- **Fréquence :** systématique lors d’un lancement normal d’après le code.
- **Tâche liée :** aucune

## Reproduction

Quitter Plop-Wallpaper, puis relancer l’exécutable depuis l’Explorateur Windows.

## Résultat observé

La fenêtre principale s’affiche. Le chemin de démarrage de la version 0.1.12 appelle directement `ShowWindow` avec le mode d’affichage transmis par Windows.

## Résultat attendu

L’icône de Plop-Wallpaper apparaît dans la zone de notification sans affichage de la fenêtre principale. Un double-clic sur l’icône ouvre la fenêtre. Le comportement vaut aussi à l’ouverture de session lorsque le lancement automatique est activé.

## Diagnostic

L’inscription de l’icône est déclenchée par la réduction de la fenêtre après son ouverture.

## Correction et vérification

Test de non-régression manuel sur Windows 11 : quitter une instance éventuelle, lancer l’exécutable, vérifier que seule l’icône apparaît dans la zone de notification (éventuellement sous la flèche des icônes masquées), puis double-cliquer sur l’icône et vérifier que la fenêtre s’ouvre. Refaire le parcours à l’ouverture de session si le lancement automatique est activé.

La version 0.1.13 inscrit l’icône avant d’afficher la fenêtre. Si l’inscription échoue, elle affiche la fenêtre pour garder l’application accessible.

- 2026-11-16 — Compilation Debug x64 et six tests CTest réussis. La compilation hors bac à sable a résolu l’erreur MSVC C1902 du premier essai.
- 2026-11-16 — Contrôle documentaire complet réussi.
- Le test de non-régression manuel reste à exécuter par l’utilisateur.
