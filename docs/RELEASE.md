# Production d’une version

La phase de livraison ajoutera le point d’entrée `tools/livrable.ps1`. Sur demande explicite, il construira l’exécutable Release x64, préparera le paquet, calculera son empreinte et appellera l’outil commun de vérification.

| Cible | Format | Contenu |
|---|---|---|
| Windows 11 x64 | `Plop-Wallpaper-<version>-windows-x64.zip` et son fichier `.sha256` | Exécutable portable, exemple INI, `CHANGELOG.md` et notices de licences si nécessaire. |

Le paquet est produit dans `livrable/` à partir de la version déclarée dans [VERSION](../VERSION). L’exemple INI accompagne l’exécutable et le paquet exclut les réglages personnels ainsi que les images de l’utilisateur.
