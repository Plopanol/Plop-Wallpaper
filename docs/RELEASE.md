# Production d’une version

Depuis la racine du dépôt, exécuter `pwsh -NoProfile -File tools/livrable.ps1`. Le script construit l’exécutable Release x64, prépare le paquet, calcule son empreinte et appelle l’outil commun de vérification.

| Cible | Format | Contenu |
|---|---|---|
| Windows 11 x64 | `Plop-Wallpaper-<version>-windows-x64.zip` et son fichier `.sha256` | Exécutable portable, exemple INI, `CHANGELOG.md` et notices de licences si nécessaire. |

Le paquet est produit dans `livrable/` à partir de la version déclarée dans [VERSION](../VERSION). L’exécutable utilise le runtime C++ statique. L’[exemple INI](config.example.ini) accompagne l’exécutable et le paquet exclut les réglages personnels ainsi que les images de l’utilisateur.
