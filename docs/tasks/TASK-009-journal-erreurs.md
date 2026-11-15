# TASK-009 — Journaliser les erreurs

- **Statut :** Terminée
- **Phase :** Phase 4 — Persistance et événements Windows
- **Priorité :** Normale
- **Dépendances :** aucune
- **Dernière mise à jour :** 2026-11-15

## Résultat attendu

Conserver les erreurs de l’application dans un journal local horodaté et borné, à côté de l’exécutable.

## Critères d’acceptation

- [x] Les erreurs signalées par l’application sont ajoutées à un journal UTF-8.
- [x] Une écriture qui dépasserait 1 Mo ouvre un nouveau fichier et conserve la dernière archive.
- [x] Un échec d’écriture laisse l’application poursuivre.

## Validation et preuves

- 2026-11-15 — Compilation Debug x64 et six tests CTest réussis. Le test TASK-009 vérifie l’encodage des accents, l’écriture à 1 000 000 octets, le roulement, le remplacement de l’archive et un échec d’écriture récupérable.
- 2026-11-15 — Essai en dossier isolé réussi. L’exécutable lancé depuis un autre répertoire crée `erreurs.log` à côté de lui. Lorsque ce chemin est occupé par un dossier, le processus se termine normalement.
- 2026-11-15 — Contrôle documentaire et synchronisation du socle réussis. Aucun parcours d’interface requis pour le journal.
