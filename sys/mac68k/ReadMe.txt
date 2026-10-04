                   NetHack 5.0 pour Macintosh 68k
                   ==============================

(Traduction française du document d'origine en anglais.)

Configuration requise
---------------------
    - Un Macintosh doté d'un processeur 68020 ou plus récent.
    - System 7.0 ou ultérieur, avec l'adressage 32 bits activé
      (tableau de bord Mémoire ; redémarrez après la modification).
    - 6 Mo de RAM libre au minimum ; 8 Mo est la partition conseillée.
    - La couleur est facultative : les tuiles exigent un écran 4 ou
      8 bits ; les Mac noir et blanc jouent en ASCII.

Installation
------------
Le jeu est distribué sous deux formes ; choisissez-en une :

NetHack.img -- une image disque SCSI complète, prête à jouer (Apple
Partition Map + volume HFS « NetHack 5.0 »). Attachez-la comme disque
dans QEMU, ou copiez-la sur une carte SD BlueSCSI. Rien à installer :
démarrez, ouvrez le volume et double-cliquez sur NetHack.

NetHack.sit -- une archive StuffIt pour installer le jeu sur un système
existant. Décompressez-la SUR LE MAC avec StuffIt Expander (la
décompresser sur une autre machine fait perdre les « resource forks »).
Elle contient :

    NetHack             l'application
    Recover             application de récupération après plantage
    nhdat               données du jeu empaquetées (niveaux, Lua, textes)
    NetHack Defaults    fichier de configuration (TEXT modifiable)
    Guidebook           comment jouer à NetHack
    Read Me             ce fichier
    license, symbols

Gardez tout dans un même dossier et double-cliquez sur NetHack. Les
fichiers de sauvegarde, les fichiers de niveau et le fichier record
(meilleurs scores) sont créés dans ce même dossier pendant la partie.

Modes d'affichage
-----------------
Sur un écran couleur, la carte démarre en tuiles graphiques ; choisissez
« Tile Mode » dans le menu File à tout moment de la partie pour basculer
entre tuiles et ASCII. Les écrans noir et blanc utilisent toujours
l'ASCII.

La carte occupe sa propre fenêtre : déplacez-la où vous voulez,
redimensionnez-la et, sur les grands écrans, utilisez ses barres de
défilement. Les positions et tailles des fenêtres (pour chaque mode
d'affichage) sont mémorisées d'une partie à l'autre dans « NetHack
Preferences », dans le dossier Préférences du Dossier Système.

Vous pouvez cliquer sur la carte pour vous y déplacer, et les invites de
la ligne de messages proposent des boutons cliquables pour les questions
oui/non.

Configuration
-------------
Modifiez « NetHack Defaults » avec n'importe quel éditeur de texte
(SimpleText convient) ; les commentaires du fichier décrivent chaque
option. Entrées utiles :

    OPTIONS=!tiled_map          démarrer en ASCII même sur écran couleur
    OPTIONS=menucolors          entrées d'inventaire en couleur

Les motifs de couleur des menus utilisent des jokers de type shell ;
entourez-les donc de '*' :

    MENUCOLOR="* cursed *"=red
    MENUCOLOR="* blessed *"=cyan

Parties plantées
----------------
Les points de reprise (checkpoint) sont activés par défaut. Après un
plantage ou une coupure de courant, le démarrage suivant refusera de
commencer une nouvelle partie tant que les fichiers de la partie plantée
sont présents. Lancez l'application Recover et choisissez le fichier
« .0 » de la partie plantée (les fichiers de niveau portent le nom de
votre personnage : « 1Brunhilda.0 », « 1Brunhilda.1 », ...) pour
reconstruire un fichier de sauvegarde que vous pourrez restaurer. Pour
abandonner plutôt la partie plantée, supprimez ces fichiers numérotés du
dossier du jeu.

Remarques
---------
Les fichiers de sauvegarde et les fichiers d'os (bones) des versions
antérieures de NetHack ne fonctionnent pas avec la 5.0.

Code source et instructions de compilation (compilation croisée Retro68) :
    https://github.com/ingpaschke/NetHack -- voir sys/mac68k/BUILD.md

Basé sur le portage Macintosh classique de Dean Luick, Kevin Hugo,
Mark Modrall, Jon W{tte, David Hairston et Michael Hamel.
Ressuscité pour NetHack 5.0 par Ingo Paschke.

Ce n'est pas un portage officiel de NetHack ; veuillez envoyer rapports
de bogues, suggestions et commentaires à ipaschke@lpclabs.de.
