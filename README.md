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

Sélectionnez un écran dans la liste, cliquez sur **Choisir un dossier…**, puis sur **Changer le fond**. Répétez ces étapes pour le second écran. À chaque changement, l’application parcourt les images JPG, JPEG, PNG et BMP du dossier dans l’ordre des chemins et ignore les images illisibles. Le fond est étiré sur un écran paysage. Sur un écran portrait, choisissez l’ajustement à la largeur ou à la hauteur.

Pour programmer les changements, cochez **Activer la rotation générale**, saisissez l’intervalle en minutes et cliquez sur **Appliquer**. La case **Faire participer cet écran à la rotation** s’applique à l’écran sélectionné. Les dossiers, les modes et les paramètres de rotation sont conservés dans `config.ini`, à côté de l’exécutable.

Cochez **Lancer Plop-Wallpaper à l’ouverture de session** pour ajouter un raccourci au dossier Démarrage de votre compte Windows. Décochez la case pour retirer ce raccourci.

- [Périmètre et critères produit](docs/01-PRODUIT.md)
- [Ordre des travaux](docs/03-ROADMAP.md)
- [Règles et contrôles du dépôt](AGENTS.md)
