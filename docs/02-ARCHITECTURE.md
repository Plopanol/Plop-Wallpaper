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
| `RotationScheduler` | Timer Win32 commun aux moniteurs en mode Rotation disposant d’un dossier. Le timer est arrêté si aucun écran connecté ne remplit ces critères. |
| `TrayManager` | Icône à la réduction et menu via `Shell_NotifyIcon`, restauration après redémarrage d’Explorer. Le clic simple est différé selon le délai de double-clic Windows. Il déclenche la rotation si elle est active ou restaure la fenêtre. Le double-clic annule cette action différée et restaure la fenêtre. |
| `SettingsManager` | Chargement, valeurs par défaut et sauvegarde du fichier INI. |
| `MainWindow` | Contrôles Win32 et rubriques de configuration en français, avec suivi du mode clair ou sombre de Windows. |

## Données et activité

Le fichier `config.ini` se trouve à côté de l’exécutable. Il mémorise le dossier, le mode Rotation ou Unique et les images par écran, ainsi que l’intervalle et l’ordre communs. Une ancienne configuration conserve la rotation des écrans déjà actifs et le fond affiché sur les autres. L’ordre est séquentiel par défaut. En mode aléatoire, le catalogue réserve l’image affichée comme dernier recours. Le raccourci dans le dossier Démarrage du compte Windows conserve l’activation du lancement à l’ouverture de session. Le réglage `CloseToTray` de la section `[Window]` vaut `0` par défaut. Les intervalles individuels et la préférence de réduction restent prévus.

Garder les chemins en mémoire. Le gestionnaire génère un bitmap à la résolution du moniteur, puis applique ce bitmap à l’écran ciblé sans modifier le réglage global d’ajustement de Windows. En portrait, l’image est centrée et les bords sont rognés ou complétés en noir selon ses proportions. Le cache `fonds-générés/` se trouve à côté de l’exécutable et le dernier bitmap appliqué reste disponible pour Windows. Les changements de topologie, de session et de veille réconcilient les écrans et les timers. Les contrôles de fichiers interviennent quand le catalogue doit être actualisé ou qu’une image doit être appliquée.
