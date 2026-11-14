# Conventions — Plop-Wallpaper

## Commandes et contrôles

- Diagnostic de la chaîne Visual Studio — `vswhere.exe -all -products "*" -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64` depuis le répertoire de l’installateur Visual Studio.
- Configuration CMake, compilation x64 et lancement dans le [README](../README.md#compiler-et-lancer).
- Pour les contrôles rapide et complet du dépôt, suivre [AGENTS.md](../AGENTS.md#commandes-et-contrôles).

## Code et validation

C++20, Win32 en Unicode et compilation MSVC sans avertissement. Les validations visuelles se font sur Windows 11 avec deux écrans, dont un en portrait. Pour chaque tâche, consigner les commandes ou parcours exécutés et leurs résultats dans la fiche. Une tâche qui dépend d’un essai manuel reste à valider jusqu’à réception du résultat.
