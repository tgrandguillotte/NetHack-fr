Le script makefont.lua convertit les polices Terminus des sources BDF au
format PSF version 2, utilisé par NetHack sous MS-DOS. Le répertoire
sys/msdos/fonts reçoit les polices converties.

makefont.lua est conçu spécifiquement pour NetHack : il réordonne les
glyphes d'entrée afin que les 256 premières positions soient conformes à
IBM437. Les polices peuvent alors prendre en charge IBMGraphics sans
recourir à la table Unicode.
