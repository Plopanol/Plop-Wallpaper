---
name: nettoyer-repo
description: Inventorie ou nettoie les sorties de compilation, caches régénérables et journaux locaux désignés de Plop-Wallpaper, notamment Debug, Release, CMake et target.
---

# Nettoyer le dépôt

1. Avant chaque nettoyage, vérifier que les cibles de `tools/nettoyer-repo.ps1` correspondent aux commandes de compilation et aux chemins de sortie actuels du projet. Lorsqu'ils changent, mettre à jour le script et ce skill ensemble. Tester l'inventaire et la suppression sur des artefacts de test.
2. Depuis la racine du dépôt, lancer `pwsh -NoProfile -File tools/nettoyer-repo.ps1` pour afficher les cibles et les exclusions. Le script examine les sorties et caches locaux déclarés, dont `build`, `target`, `debug`, `release` et les sorties CMake, ainsi que `logs/` et les fichiers `*.log` à la racine.
3. Vérifier que les chemins proposés correspondent à la demande. Le script protège notamment les fichiers suivis, `config.ini`, `data/`, `livrable/` et les ressources locales d'icônes. Si la demande exige de préserver `target`, s'en tenir à l'inventaire et signaler que le script ne possède pas d'option pour exclure ce dossier.
4. Si l'utilisateur a demandé la suppression et que les cibles correspondent à sa demande, relancer avec `pwsh -NoProfile -File tools/nettoyer-repo.ps1 -Execute`. Indiquer les chemins supprimés, conservés et les éventuels échecs.
