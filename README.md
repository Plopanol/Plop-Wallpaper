# Plop-Wallpaper

Gestionnaire de fonds d’écran pour Windows 11 x64. Choisissez un dossier par moniteur et changez les fonds manuellement ou à intervalles réguliers.

## Compiler et lancer

Depuis la racine du dépôt, utilisez PowerShell 7, CMake 4.2 ou plus récent et les [Build Tools Visual Studio 18 avec MSVC x64 et le SDK Windows](docs/adr/ADR-001-chaine-msvc.md).

```powershell
cmake -S . -B build -G 'Visual Studio 18 2026' -A x64
cmake --build build --config Debug
.\build\Debug\PlopWallpaper.exe
```

Pour une compilation optimisée, utilisez `Release` comme configuration et comme sous-dossier de `build`.

## Changer un fond

Sélectionnez un écran dans le tableau et choisissez **Rotation** ou **Unique**. Seule la rubrique du mode choisi apparaît. En **Rotation**, choisir un dossier lance les changements automatiques et **Image suivante** avance immédiatement. Les formats JPG, JPEG, PNG et BMP sont pris en charge. En **Unique**, le fond reste fixe et **Choisir une image…** applique immédiatement l’image choisie. Dans **Générale**, le fond est étiré sur un écran paysage et l’ajustement à la largeur ou à la hauteur est disponible en portrait.

L’intervalle et l’ordre des images sont communs aux écrans en mode **Rotation**. Modifiez-les dans cette rubrique, puis cliquez sur **Appliquer**. L’ordre des chemins est utilisé par défaut. L’ordre choisi s’applique aussi à **Image suivante**. Les réglages sont conservés dans `config.ini`, à côté de l’exécutable.

Réduisez la fenêtre pour afficher Plop-Wallpaper dans la zone de notification. Un double-clic sur l’icône rouvre la fenêtre sans déclencher de rotation. Après le délai de double-clic défini par Windows, un clic simple change le fond des écrans en mode **Rotation** qui ont un dossier. Si aucun écran ne tourne, le clic simple rouvre la fenêtre. Le menu du clic droit permet d’ouvrir ou de quitter l’application.

Cochez **Lancer Plop-Wallpaper à l’ouverture de session** pour ajouter un raccourci au dossier Démarrage de votre compte Windows. Décochez la case pour retirer ce raccourci.

La fenêtre suit le mode clair ou sombre des applications dans **Paramètres → Personnalisation → Couleurs** de Windows.

- [Périmètre et critères produit](docs/01-PRODUIT.md)
- [Ordre des travaux](docs/03-ROADMAP.md)
- [Règles et contrôles du dépôt](AGENTS.md)
