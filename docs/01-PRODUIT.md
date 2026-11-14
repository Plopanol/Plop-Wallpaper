# Produit — Plop-Wallpaper

## Objectif

Permettre à une personne utilisant deux écrans sous Windows 11 de gérer leurs fonds indépendamment. L’application doit être portable, fonctionner hors ligne et rester discrète en arrière-plan.

## Périmètre

- Détecter les écrans connectés et afficher leur nom, leur résolution et leur orientation. Conserver les associations de dossiers lors d’une réorganisation et d’une reconnexion.
- Utiliser un dossier indépendant par moniteur et accepter JPG, JPEG, PNG et BMP. Ignorer les images corrompues et gérer les dossiers vides ou inaccessibles.
- Permettre de choisir et d’appliquer directement une image sur le moniteur sélectionné.
- Proposer l’étirement du fond à la résolution de l’écran paysage et, sur l’écran portrait, son ajustement proportionnel à la largeur ou à la hauteur.
- Proposer les actions Précédent selon l’historique et Suivant, une rotation séquentielle ou aléatoire sans répétition immédiate quand plusieurs images sont disponibles, et une activation par écran. Prévoir les intervalles de 1, 5, 10, 15, 30 et 60 minutes ainsi qu’une durée personnalisée en minutes.
- Proposer un réglage général en minutes pour appliquer une cadence commune aux écrans participant à la rotation.
- Proposer une fenêtre Win32, la pause générale, le démarrage automatique avec Windows en option et les préférences de démarrage et de réduction dans la zone de notification. L’icône permet d’ouvrir l’application, de changer le fond d’un écran ou des deux, de suspendre les rotations et de quitter.
- Rétablir les préférences au redémarrage et gérer la veille, le verrouillage de session, la reconnexion d’un écran et le redémarrage d’Explorer. L’application fonctionne avec des droits utilisateur et une seule instance.

## Validation cible

Sur deux écrans d’orientations différentes, chaque écran utilise uniquement son dossier et conserve son rythme lorsque la fenêtre est masquée. Vérifier la persistance, la déconnexion et l’arrêt complet. Mesurer le CPU au repos, la mémoire privée au repos et après plusieurs centaines de changements, ainsi que les accès disque au repos. Les cibles indicatives sont un CPU proche de 0 %, moins de 20 Mo de mémoire privée et aucun accès disque régulier.
