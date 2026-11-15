# BUG-006 — Échec de détection des écrans au démarrage

- **Statut :** Corrigé à vérifier
- **Gravité :** Majeure
- **Composant :** Moniteurs
- **Signalé le :** 2026-11-15
- **Version et environnement :** 0.1.6, Windows 11.
- **Fréquence :** une occurrence signalée.
- **Tâche liée :** aucune

## Reproduction

Démarrer le PC en laissant les écrans éteints pendant le lancement de Plop-Wallpaper. Allumer les écrans et relever l’éventuel message de démarrage.

## Résultat observé

La capture montre le message « Impossible de détecter les écrans. (0x80004005) ». Le 15 novembre 2026, l’utilisateur a confirmé que le relancement de la version 0.1.6 aboutit avec les écrans allumés.

## Résultat attendu

Si la détection échoue temporairement au démarrage, l’application reste active et détecte les écrans dès qu’ils deviennent disponibles. Voir la gestion des changements d’affichage dans l’[architecture](../02-ARCHITECTURE.md).

## Diagnostic

Le code affiche ce message quand l’énumération initiale des moniteurs échoue, puis quitte avant la création de la fenêtre qui reçoit les notifications d’affichage. La cause précise du retour 0x80004005 reste à établir.

## Correction et vérification

Depuis la version 0.1.7, l’application crée sa fenêtre avant la première détection et réutilise le mécanisme d’actualisation existant. Si cette lecture échoue, elle retente la détection via le timer différé et reste disponible pour les notifications d’affichage et de périphériques.

- 2026-11-15 — Compilation Release x64 et cinq tests CTest réussis. Le test BUG-006 simule une erreur COM E_FAIL suivie d’une détection de deux écrans. Il couvre la récupération de l’énumération. La survie du processus au démarrage et la réception des notifications restent à vérifier sur le poste.
- 2026-11-15 — Contrôle documentaire complet réussi.
- Vérification manuelle attendue : démarrer Windows avec les écrans éteints et la version 0.1.7 configurée pour se lancer à l’ouverture de session. Allumer les écrans et vérifier l’absence de boîte d’erreur, la présence de l’application et le remplissage de sa liste sans relancement.

### Récidive du 15 novembre 2026

L’utilisateur signale une liste vide au démarrage avec le message « Impossible d’actualiser les écrans connectés. » La version de l’exécutable installé reste à établir. Le code de la première correction n’armait qu’une tentative différée. Après son échec, l’application dépendait notamment d’une notification Windows pour retenter la détection.

La version 0.1.10 effectue jusqu’à quinze tentatives supplémentaires espacées de deux secondes après l’échec du timer d’actualisation. Elle affiche le HRESULT en cas d’échec pour faciliter le diagnostic sur le poste. Le test BUG-006 couvre deux échecs successifs avant la récupération et la limite de tentatives.

- 2026-11-15 — Compilation Debug x64 et cinq tests CTest réussis hors bac à sable après une erreur MSVC C1902 dans le bac à sable. Contrôle documentaire complet réussi.
- Vérification manuelle attendue sur 0.1.10 : démarrer avec les écrans éteints, les allumer puis vérifier que les lignes des écrans apparaissent sans relancer l’application. Si la liste reste vide, relever le code hexadécimal affiché.
