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

Sélectionnez un écran dans la liste. Dans **Rotation**, cliquez sur **Choisir un dossier…**, puis sur **Image suivante** pour parcourir les images JPG, JPEG, PNG et BMP. L’ordre des chemins est utilisé par défaut. Dans **Unique**, cliquez sur **Choisir une image…** pour appliquer immédiatement cette image à l’écran sélectionné. Si cet écran participe à une rotation active, elle reprend à la prochaine échéance. Dans **Générale**, le fond est étiré sur un écran paysage et l’ajustement à la largeur ou à la hauteur est disponible en portrait.

Pour programmer les changements, cochez **Activer la rotation générale**, saisissez l’intervalle en minutes et cliquez sur **Appliquer**. La case **Faire participer cet écran à la rotation** s’applique à l’écran sélectionné. Pour tirer les images au hasard, cochez **Aléatoire** et cliquez sur **Appliquer**. Ce réglage s’applique aussi au bouton **Image suivante**. Les dossiers, les modes et les paramètres de rotation sont conservés dans `config.ini`, à côté de l’exécutable.

Réduisez la fenêtre pour afficher Plop-Wallpaper dans la zone de notification. Quand la rotation générale est active, un clic sur l’icône change immédiatement le fond des écrans participants. Quand elle est désactivée, le clic rouvre la fenêtre. Le menu du clic droit permet d’ouvrir ou de quitter l’application.

Cochez **Lancer Plop-Wallpaper à l’ouverture de session** pour ajouter un raccourci au dossier Démarrage de votre compte Windows. Décochez la case pour retirer ce raccourci.

La fenêtre suit le mode clair ou sombre des applications dans **Paramètres → Personnalisation → Couleurs** de Windows.

- [Périmètre et critères produit](docs/01-PRODUIT.md)
- [Ordre des travaux](docs/03-ROADMAP.md)
- [Règles et contrôles du dépôt](AGENTS.md)
