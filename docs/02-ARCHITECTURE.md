# Architecture — Plop-Wallpaper

## Plateforme

Exécutable natif Windows 11 x64 en C++20, compilé avec MSVC et CMake. La boucle de messages Win32 pilote l’interface, les timers et les événements Windows. L’application fonctionne hors ligne avec les API Windows natives.

## Responsabilités prévues

| Composant | Responsabilité |
|---|---|
| `Application` | Cycle de vie, instance unique, événements système et libération des ressources. |
| `MonitorManager` | Détection, identité Windows stable quand disponible, résolution, orientation et suivi de la topologie. |
| `WallpaperManager` | Interaction COM avec `IDesktopWallpaper`, changement ciblé avec `SetWallpaper` et prise en compte des limites d’ajustement. |
| `ImageCatalog` | Catalogue de chemins par dossier, actualisé au démarrage et lorsque nécessaire. |
| `RotationScheduler` | Timer Win32 commun aux moniteurs participants pour la cadence générale. Timers indépendants, historique et pause prévus. |
| `TrayManager` | Icône et menu via `Shell_NotifyIcon`, restauration après redémarrage d’Explorer. |
| `SettingsManager` | Chargement, valeurs par défaut et sauvegarde du fichier INI. |
| `MainWindow` | Contrôles Win32 et rubriques de configuration en français, avec suivi du mode clair ou sombre de Windows. |

## Données et activité

Le fichier `config.ini` se trouve à côté de l’exécutable. Il mémorise le dossier, le mode, la participation, la dernière image de rotation et l’image choisie dans Unique par identifiant de moniteur, ainsi que l’activation et l’intervalle de la rotation générale. Le raccourci dans le dossier Démarrage du compte Windows conserve l’activation du lancement à l’ouverture de session. Les intervalles individuels et la préférence de réduction restent prévus. Prévoir des valeurs par défaut pour les données absentes ou invalides.

Garder les chemins en mémoire. Le gestionnaire génère un bitmap à la résolution du moniteur, puis applique ce bitmap à l’écran ciblé sans modifier le réglage global d’ajustement de Windows. En portrait, l’image est centrée et les bords sont rognés ou complétés en noir selon ses proportions. Le cache `fonds-générés/` se trouve à côté de l’exécutable et le dernier bitmap appliqué reste disponible pour Windows. Les changements de topologie, de session et de veille réconcilient les écrans et les timers. Les contrôles de fichiers interviennent quand le catalogue doit être actualisé ou qu’une image doit être appliquée.
