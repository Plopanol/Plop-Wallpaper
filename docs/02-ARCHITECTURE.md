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
| `RotationScheduler` | Timers Win32 indépendants, sélection, historique et pause. |
| `TrayManager` | Icône et menu via `Shell_NotifyIcon`, restauration après redémarrage d’Explorer. |
| `SettingsManager` | Chargement, valeurs par défaut et sauvegarde du fichier INI. |
| `MainWindow` | Contrôles Win32 et interface de configuration en français. |

## Données et activité

Le fichier `config.ini` se trouve à côté de l’exécutable. Il mémorise par identifiant de moniteur le dossier, le mode, l’intervalle, l’activation et la dernière image, ainsi que les préférences générales de démarrage et de réduction. Prévoir des valeurs par défaut pour les données absentes ou invalides.

Garder les chemins en mémoire et laisser Windows appliquer les images originales. Les changements de topologie, de session et de veille réconcilient les écrans et les timers. Les contrôles de fichiers interviennent quand le catalogue doit être actualisé ou qu’une image doit être appliquée.
