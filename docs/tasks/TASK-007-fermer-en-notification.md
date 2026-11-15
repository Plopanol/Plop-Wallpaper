# TASK-007 — Fermer dans la barre de notification

- **Statut :** Terminée
- **Phase :** Phase 3 — Interface et zone de notification
- **Priorité :** Normale
- **Dépendances :** TASK-005
- **Dernière mise à jour :** 2026-11-15

## Résultat attendu

La rubrique Générale propose une case persistante pour masquer la fenêtre avec son bouton de fermeture.

## Critères d’acceptation

- [x] Case cochée, le bouton X masque la fenêtre, affiche l’icône et laisse les rotations actives.
- [x] La commande Quitter du menu contextuel arrête l’application, même avec la case cochée.
- [x] Le réglage est conservé au redémarrage. Case décochée ou clé absente, le bouton X quitte l’application.
- [x] Le bouton Réduire et la réouverture depuis l’icône fonctionnent comme avant.

## Validation et preuves

- 2026-11-15, Windows x64 : compilation Debug MSVC réussie et 5 tests CTest réussis.
- 2026-11-15 : Test-Docs, contrôle Sync-Commun et `git diff --check` réussis.
- 2026-11-15 : validation manuelle confirmée par l’utilisateur sur le parcours proposé avec `build/bug-006/Debug/PlopWallpaper.exe` (fermeture vers l’icône, rotations, réouverture, réduction, commande Quitter et persistance au redémarrage). La valeur par défaut lorsque la clé est absente a été vérifiée dans le code.
