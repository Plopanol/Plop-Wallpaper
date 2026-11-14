# Questions ouvertes

Prochain identifiant libre : Q-002.

| ID | Sujet | Bloquante | Responsable | État |
|---|---|---|---|---|
| [Q-001](#q-001) | Chaîne CMake et MSVC | Oui, pour la compilation | Utilisateur | Ouverte |

## Q-001

Le prompt exige Visual Studio 2022. Sur le poste inspecté, `vswhere` a détecté des Build Tools 18 avec les outils MSVC x86/x64. La recherche d’une installation en version 17 n’a renvoyé aucun chemin.

- Confirmer la disponibilité des outils MSVC de Visual Studio 2022, ou autoriser explicitement l’utilisation des Build Tools 18.
- Traiter cet arbitrage dans [TASK-002](tasks/TASK-002-changer-les-fonds-manuellement.md) avant de figer la chaîne de compilation.
