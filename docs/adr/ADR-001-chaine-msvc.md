# ADR-001 — Chaîne de compilation MSVC

- **Statut :** Acceptée
- **Date :** 2026-11-14
- **Source de la décision :** Q-001, essai des outils existants demandé par l’utilisateur.
- **Remplace :** aucune

## Contexte

Le cadrage initial demandait Visual Studio 2022. Sur le poste de développement, `vswhere` détecte Visual Studio Build Tools 18.10.1 avec les outils MSVC x64 et ne trouve aucune installation Visual Studio 2022.

## Décision

Utiliser les Build Tools 18 installés pour TASK-002, avec le générateur CMake `Visual Studio 18 2026` et l’architecture `-A x64`.

## Alternatives et conséquences

Installer Visual Studio 2022 demanderait une installation supplémentaire. Le 14 novembre 2026, CMake 4.5.0-rc3, MSVC 19.51.36257 et le SDK Windows 10.0.28000.0 ont configuré et compilé en Release un exécutable Win32 x64 en C++20. L’exécutable a initialisé COM et renvoyé le code 0. L’essai a réussi depuis un chemin court dans le dépôt, après une erreur MSBuild `FTK1011` dans le dossier temporaire du bac à sable.

## Autorités concernées

- [TASK-002](../tasks/TASK-002-changer-les-fonds-manuellement.md) pour les instructions de compilation et la validation de l’application.
