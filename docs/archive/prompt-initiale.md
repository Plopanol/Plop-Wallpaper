# Projet : Plop-Wallpaper – Gestionnaire de fonds d'écran Windows 11

## 1. Objectif

Développer une application Windows 11 permettant de gérer automatiquement les fonds d'écran de deux moniteurs, avec un dossier d'images indépendant pour chaque écran.

**Priorité absolue : minimiser la consommation de RAM, de CPU et les accès disque.**

L'application doit être légère, portable, fonctionner en arrière-plan et ne nécessiter aucune connexion Internet.

## 2. Technologies imposées

- Langage : C++20.
- Interface : API Win32 native.
- Compilation : Visual Studio 2022 / MSVC avec CMake.
- Gestion des fonds d'écran : API COM `IDesktopWallpaper` de Windows.
- Icône de notification : API `Shell_NotifyIcon`.
- Gestion des événements : boucle de messages Win32.
- Planification : timers Windows, sans boucle d'attente active.
- Configuration : fichier INI léger dans le dossier de l'application `config.ini`.
- Architecture : exécutable natif x64.

### Interdictions

- Ne pas utiliser Electron, Chromium, WebView ou un navigateur intégré.
- Ne pas utiliser Qt, Java, Python ou .NET.
- Aucun service Windows permanent.
- Aucun serveur local, aucune télémétrie et aucune connexion réseau.
- Aucun thread permanent de surveillance utilisant du polling.
- Aucune bibliothèque externe si une API Windows native suffit.

## 3. Gestion des moniteurs

L'application doit :

1. Détecter automatiquement les écrans connectés.
2. Identifier chaque moniteur à l'aide d'un identifiant Windows stable, autant que possible.
3. Afficher le nom, la résolution et l'orientation de chaque moniteur.
4. Permettre d'associer un dossier d'images à chaque moniteur.
5. Conserver l'association même si Windows change l'ordre des écrans.
6. Détecter les changements de configuration des moniteurs.
7. Gérer proprement les écrans déconnectés puis reconnectés.

Configuration cible :

- Écran 1 : mode paysage (exemple : 2560 × 1440).
- Écran 2 : mode portrait (exemple : 1440 × 2560).

Les résolutions doivent être détectées et ne jamais être codées en dur.

Chaque moniteur doit posséder sa propre séquence de fonds d'écran.

Utiliser `IDesktopWallpaper::SetWallpaper` en spécifiant l'identifiant du moniteur concerné.

## 4. Gestion des images

Chaque écran dispose d'un dossier indépendant.

Exemple :

```
D:\Wallpapers\Landscape\
D:\Wallpapers\Portrait\
```

Fonctionnalités :

- Prendre en charge les formats JPG, JPEG, PNG et BMP.
- Ignorer les fichiers non compatibles ou corrompus.
- Ne pas charger simultanément toutes les images en mémoire.
- Conserver uniquement la liste des chemins des fichiers.
- Ne pas créer de copies inutiles des images.
- Utiliser les API Windows pour définir les fonds d'écran.
- Proposer un ordre séquentiel ou aléatoire.
- En mode aléatoire, éviter de répéter immédiatement la même image.
- Gérer les dossiers vides ou devenus inaccessibles.
- Permettre de passer immédiatement à l'image suivante sur un écran donné.

## 5. Rotation automatique

Permettre de choisir un intervalle de changement du fond d'écran :

- 1 minute
- 5 minutes
- 10 minutes
- 15 minutes
- 30 minutes
- 60 minutes
- Intervalle personnalisé en minutes

Chaque écran peut disposer de son propre intervalle.

Prévoir également :

- Activation/désactivation du défilement par écran.
- Rotation séquentielle ou aléatoire.
- Bouton « Image suivante ».
- Bouton « Image précédente » lorsque l'historique le permet.
- Pause/reprise du défilement.

Utiliser des timers Windows événementiels.

Il est interdit d'utiliser une boucle infinie avec `Sleep()` pour gérer le défilement.

Aucun traitement ne doit être exécuté inutilement entre deux changements de fond d'écran.

## 6. Interface graphique

Créer une interface Win32 simple et fonctionnelle.

La fenêtre principale contient :

### Section Moniteur 1

- Nom du moniteur.
- Résolution.
- Orientation.
- Dossier d'images avec bouton Parcourir.
- Intervalle de rotation.
- Mode aléatoire ou séquentiel.
- Activation/désactivation.
- Boutons Précédent / Suivant.

### Section Moniteur 2

Mêmes paramètres que pour le moniteur 1.

### Paramètres généraux

- Démarrer automatiquement avec Windows (optionnel).
- Démarrer directement dans la zone de notification.
- Réduire dans la zone de notification.
- Mettre en pause toutes les rotations.
- Enregistrer automatiquement la configuration.

L'interface doit rester sobre et utiliser les contrôles Windows natifs.

Aucune animation graphique inutile.

## 7. Fonctionnement dans la zone de notification

Lorsqu'on réduit la fenêtre :

1. La fenêtre principale disparaît.
2. L'application reste active.
3. Une icône apparaît dans la zone de notification Windows, près de l'horloge.
4. La fenêtre ne doit plus occuper de place dans la barre des tâches.

Clic droit sur l'icône :

- Ouvrir Plop-Wallpaper
- Changer le fond de l'écran 1
- Changer le fond de l'écran 2
- Changer tous les fonds
- Pause / Reprendre
- Quitter

Double-clic sur l'icône : ouvrir la fenêtre principale.

Cliquer sur « Quitter » doit réellement terminer l'application et libérer toutes les ressources.

Gérer correctement le redémarrage d'Explorer.exe afin de restaurer l'icône de notification.

## 8. Optimisation CPU et RAM

Cette section est prioritaire.

### CPU

- Consommation CPU proche de 0 % lorsque l'application est inactive.
- Aucune boucle de surveillance continue.
- Aucun rafraîchissement périodique inutile de l'interface.
- Réagir uniquement aux événements Windows et aux expirations des timers.
- Ne pas recalculer les listes d'images à chaque rotation.

### RAM

- Éviter de charger les images complètes en mémoire.
- Stocker principalement les chemins des fichiers.
- Ne pas générer de miniatures si elles ne sont pas nécessaires.
- Éviter les caches volumineux.
- Libérer correctement les ressources COM, GDI et Win32.
- Éviter les threads inutiles.

### Disque

- Scanner les dossiers au démarrage et lorsque leur contenu doit être actualisé.
- Éviter les scans périodiques.
- Ne lire les fichiers image que lorsque cela est nécessaire.
- Ne pas réencoder les images si Windows peut les utiliser directement.

### Objectifs de performance

Cibles indicatives, à mesurer et non à garantir :

- CPU inactif : proche de 0 %.
- RAM au repos : viser moins de 20 Mo de mémoire privée.
- Aucun accès disque régulier lorsque l'application est inactive.
- Aucun processus secondaire permanent.

## 9. Configuration et persistance

Conserver les paramètres dans le dossier d'installation de l'application :

`config.ini`

Mémoriser :

- Identifiants des moniteurs.
- Dossiers d'images associés.
- Intervalles.
- Modes de défilement.
- Activation/désactivation des rotations.
- Dernière image utilisée.
- Préférences de démarrage et de réduction.

Au redémarrage, restaurer les paramètres et reprendre normalement les rotations.

Prévoir un comportement fiable en cas de fichier de configuration absent, invalide ou incomplet.

## 10. Fiabilité et comportement Windows

- Compatible Windows 11 x64.
- Fonctionnement sans droits administrateur.
- Compatible avec les sessions multiécrans.
- Gérer la sortie de veille.
- Gérer le verrouillage/déverrouillage de session.
- Gérer les moniteurs déconnectés.
- Éviter plusieurs instances simultanées de l'application.
- Ne pas modifier les paramètres système qui ne concernent pas les fonds d'écran.
- Ne pas laisser de ressources COM ou de handles Windows ouverts inutilement.
- Gérer le mode d'ajustement des images en tenant compte des limitations de l'API Windows.

## 11. Architecture attendue

Séparer clairement les responsabilités :

- `Application` : cycle de vie.
- `MonitorManager` : détection et identification des moniteurs.
- `WallpaperManager` : interaction avec `IDesktopWallpaper`.
- `ImageCatalog` : référencement des images.
- `RotationScheduler` : timers et changements automatiques.
- `TrayManager` : icône et menu de notification.
- `SettingsManager` : lecture et écriture des paramètres.
- `MainWindow` : interface graphique Win32.

Éviter toute abstraction excessive : l'application doit rester simple, maintenable et performante.

## 12. Livrables

Produire :

1. Le code source C++20 complet.
2. Un fichier `CMakeLists.txt`.
3. Les ressources Windows nécessaires (icône, manifest).
4. Un fichier README contenant les instructions de compilation.
5. Un exécutable Windows x64 en configuration Release, si l'environnement permet de le compiler.
6. Un exemple de configuration INI.

Le programme doit pouvoir être lancé comme une application portable, sans installation obligatoire.

## 13. Critères de validation

Vérifier les scénarios suivants :

1. Deux moniteurs avec orientations différentes sont détectés correctement.
2. Chaque moniteur utilise exclusivement les images de son dossier.
3. Les rotations fonctionnent indépendamment.
4. La réduction vers la zone de notification fonctionne.
5. Les rotations continuent lorsque la fenêtre principale est masquée.
6. La consommation CPU reste négligeable entre deux rotations.
7. L'application conserve une faible consommation mémoire après plusieurs centaines de changements.
8. Les préférences sont conservées après redémarrage.
9. La déconnexion d'un moniteur ne provoque aucun plantage.
10. La fermeture complète libère les ressources.

## 14. Méthode de développement

Commencer par une application minimale capable de :

1. Détecter les moniteurs.
2. Associer un dossier à chaque moniteur.
3. Changer manuellement le fond d'écran de chaque moniteur.
4. Ajouter la rotation automatique.
5. Ajouter l'interface de configuration.
6. Ajouter la gestion de l'icône de notification.
7. Finaliser la persistance et les cas particuliers.
8. Tester et optimiser les performances.

Privilégier les API Windows natives et la simplicité du code à l'ajout de fonctionnalités superflues.

**Résultat attendu : une petite application Windows 11 réellement utilisable, économe en ressources et totalement autonome.**