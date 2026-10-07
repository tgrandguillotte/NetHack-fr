INTRODUCTION
============

(Traduction française du document d'origine Readme.txt.)

L'interface (window port) « curses » est une nouvelle interface textuelle
pour NetHack, qui s'appuie sur les routines de haut niveau de curses pour
piloter l'affichage.  Elle a été compilée et testée sous Linux, macOS,
Windows et msdos, mais elle devrait aussi être portable sur de nombreux
autres systèmes, comme d'autres variantes d'UNIX et OS/2.

Parmi les fonctionnalités de cette interface par rapport à l'interface tty
traditionnelle :

 * Redimensionnement dynamique des fenêtres (par exemple en agrandissant
 une fenêtre de terminal)
 * Placement dynamique et configurable des fenêtres d'état et de messages
 par rapport à la carte
 * Meilleure exploitation des grandes fenêtres de terminal
 * Affichage plus soigné (bordures de fenêtres, boîtes de dialogue
 surgissantes facultatives, etc.)
 * Option « cursesgraphics » pour des caractères de tracé de lignes plus
 élégants pour dessiner le donjon ; elle devrait fonctionner sur la
 plupart des terminaux et plateformes


COMPILATION
===========

Instructions de compilation pour UNIX/Linux/macOS :

Suivez les instructions de
sys/unix/Install.unx.  Par défaut, le Makefile est configuré pour compiler
avec ncurses.  Modifiez Makefile.src si vous souhaitez compiler avec une
autre bibliothèque curses, comme PDCurses pour SDL.
Consultez sys/unix/NewInstall.unx pour plus d'informations sur la
compilation de NetHack 5.0 et des versions ultérieures.

Instructions de compilation pour Windows :

Par défaut, Makefile.nmake et GNUmakefile sont configurés pour compiler
curses à partir du dossier de sous-modules submodules/pdcurses (en
supposant que vous avez obtenu les sources de NetHack en clonant un dépôt
git).  Si vous avez obtenu NetHack par un autre moyen, par exemple en
téléchargeant un zip, suivez les instructions de
sys/windows/Install.windows.

Si vous utilisez un autre compilateur, vous devrez modifier manuellement
le Makefile approprié pour y inclure les fichiers de l'interface curses.

JEU
===

Le jeu devrait se dérouler comme avec l'interface tty de NetHack ; les
différences sont surtout visuelles.  Cette interface prend en charge le
redimensionnement dynamique de la fenêtre du terminal : vous pouvez donc
faire des essais en cours de partie pour trouver l'aspect qui vous
convient le mieux.  De même, les options align_status et align_message
peuvent être modifiées en cours de partie, ce qui vous permet d'essayer
la disposition qui vous plaît le plus.

Dans les menus, en plus des raccourcis clavier configurables habituels de
navigation décrits dans le Guidebook, vous pouvez utiliser les flèches
droite et gauche pour avancer ou reculer d'une page, et les touches
Début et Fin pour aller respectivement à la première et à la dernière
page.

Certaines options de configuration propres à l'interface curses, ou utiles
avec elle, sont présentées ci-dessous.  Copiez celles qui vous plaisent
dans votre fichier de configuration de nethack (par exemple .nethackrc sous
UNIX ou NetHack.cnf sous Windows) :
#
# Utilisez ceci si l'exécutable a été compilé avec plusieurs interfaces
# et que curses n'est pas l'interface par défaut
OPTIONS=windowtype:curses
#
# Activez ceci sous Windows, ou avec PDCurses pour SDL sur tout système.
# Ce dernier utilise une police cp437, qui fonctionne avec cette option
#OPTIONS=IBMgraphics
#
# Activez ceci si IBMgraphics ci-dessus ne fonctionne pas sur votre
# système.  Incompatible avec l'option précédente ; devrait fonctionner
# sur presque tous les systèmes.
OPTIONS=cursesgraphics
#
# Indique éventuellement l'alignement des fenêtres de messages et d'état
# par rapport à la fenêtre de la carte.  Sans précision, le code utilise
# par défaut les emplacements de l'interface tty : fenêtre de messages en
# haut et fenêtre d'état en bas.  Placer l'une d'elles à droite ou à
# gauche ne donne vraiment de bons résultats qu'avec des fenêtres de
# terminal plus larges.
OPTIONS=align_message:bottom,align_status:right
#
# Utilise une petite « fenêtre » surgissante pour les questions courtes,
# par exemple « Vraiment sauvegarder ? ».  Sinon, la fenêtre de messages
# est utilisée, comme dans l'interface tty.
OPTIONS=popup_dialog
#
# Indique la taille initiale de la fenêtre de NetHack, en caractères.
# Pris en charge par PDCurses pour SDL ainsi que par PDCurses pour
# Windows.
OPTIONS=term_cols:110,term_rows:32
#
# Contrôle l'utilisation des bordures pour les fenêtres principales de
# NetHack (messages, carte et état).  La valeur 1 force l'affichage des
# bordures, la valeur 2 les désactive, et la valeur 3 laisse le code
# décider de les afficher ou non selon la taille de la fenêtre du
# terminal.
OPTIONS=windowborders:3


CONTACT
=======

Merci de m'envoyer (Karl Garrison) vos rapports de bogues, suggestions,
correctifs ou autres retours à l'adresse : kgarrison@pobox.com.  Notez
qu'au moment où j'écris ces lignes, je n'ai qu'un accès sporadique à
Internet ; il se peut donc que je ne vous réponde pas tout de suite.

Bon jeu !

Karl Garrison
Mars 2009
