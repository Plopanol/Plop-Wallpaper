# TASK-010 — Éviter le raccourci dans le profil Windows

- **Statut :** Abandonnée
- **Phase :** Hors séquence
- **Priorité :** Normale
- **Dépendances :** aucune
- **Dernière mise à jour :** 2026-11-16

## Résultat attendu

Les chemins d’écriture imposés ou utilisés par défaut restent dans le dossier de l’application. Les emplacements externes explicitement choisis restent utilisables.

## Critères d'acceptation

- [ ] Remplacer l’écriture du raccourci Plop-Wallpaper.lnk dans le dossier Démarrage du profil par une intégration compatible avec le socle. Voir src/startup.cpp.
- [ ] Conserver le choix explicite d’activer ou désactiver le démarrage automatique. Traiter les anciens raccourcis sans créer deux lancements.
- [ ] Vérifier après réouverture de session que l’activation fonctionne et que la désactivation empêche le démarrage automatique.
- [ ] Mettre à jour la documentation de stockage ou de livraison concernée et consigner les contrôles.

## Validation et preuves

2026-11-16 — Tâche abandonnée sur décision de l’utilisateur. Le socle autorise le raccourci dans le dossier Démarrage lorsqu’il est créé à la demande de l’utilisateur. Les essais d’ouverture de session prévus pour le remplacement du raccourci n’ont pas été exécutés.

## Obstacles et reprise

Aucun obstacle identifié.
