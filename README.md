# Plop-Wallpaper

Gestionnaire de fonds d’écran pour Windows 11 x64. Cette version permet de choisir un dossier d’images et de changer le fond manuellement pour chaque moniteur.

## Compiler et lancer

Depuis la racine du dépôt, utilisez PowerShell 7, CMake 4.2 ou plus récent et les [Build Tools Visual Studio 18 avec MSVC x64 et le SDK Windows](docs/adr/ADR-001-chaine-msvc.md).

```powershell
cmake -S . -B build -G 'Visual Studio 18 2026' -A x64
cmake --build build --config Debug
.\build\Debug\PlopWallpaper.exe
```

Pour une compilation optimisée, utilisez `Release` comme configuration et comme sous-dossier de `build`.

## Changer un fond

Sélectionnez un écran dans la liste, cliquez sur **Choisir un dossier…**, puis sur **Changer le fond**. Répétez ces étapes pour le second écran. À chaque clic, l’application parcourt les images JPG, JPEG, PNG et BMP du dossier dans l’ordre des chemins et ignore les images illisibles. Les dossiers restent associés aux écrans pendant la session de l’application.

- [Périmètre et critères produit](docs/01-PRODUIT.md)
- [Ordre des travaux](docs/03-ROADMAP.md)
- [Règles et contrôles du dépôt](AGENTS.md)
