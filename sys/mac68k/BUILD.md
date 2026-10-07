# Compiler NetHack pour le Mac OS classique (68k / PowerPC)

*(Traduction française du document d'origine en anglais.)*

Compilation croisée avec la chaîne d'outils GCC
[Retro68](https://github.com/autc04/Retro68). Trois cibles :

- **68k** — System 7+ sur 68020+ en mode d'adressage 32 bits
- **PowerPC** — de System 7.x à Mac OS 9 (CFM/PEF, sans Carbon)
- **Fat** — une seule application qui s'exécute nativement sur les deux

## Ce dont vous avez besoin

- La chaîne d'outils Retro68 dans `/opt/retro68` (ou définissez `RETRO68=`).
  Vérifiée avec son GCC 12 par défaut jusqu'à GCC 16 ; la compilation impose
  automatiquement `-std=gnu17`.
- Les Universal Interfaces 3.x d'Apple dans `/opt/retro68/universal` (les
  en-têtes Multiversal fournis avec Retro68 sont incomplets). Étapes
  d'installation ci-dessous.
- Outils de l'hôte : `hfsutils` et `python3` ; en option, `sit` pour l'archive
  StuffIt (voir ci-dessous) et `qemu-system-m68k` (8.0+) pour les tests. Sans
  `sit`, l'étape d'empaquetage saute le `.sit` mais produit quand même l'image
  disque et le fichier MacBinary.

## Obtenir les prérequis

### Installer Retro68 et les Universal Interfaces

Paquets de l'hôte (Debian/Ubuntu) :

    sudo apt install build-essential cmake bison flex texinfo ruby hfsutils \
        libgmp-dev libmpfr-dev libmpc-dev libboost-all-dev

### Installer sit (facultatif, pour l'archive StuffIt)

Compilez-le depuis les sources et placez-le dans le `PATH` :

    git clone https://github.com/thecloudexpanse/sit.git
    cd sit && make
    cp sit ~/.local/bin/

Sautez cette étape si vous n'avez besoin que de `NetHack.img` ou de
`NetHack.bin` ; les cibles d'empaquetage signalent l'outil manquant et
poursuivent.

Clonez avec les sous-modules (GCC et binutils sont des sous-modules) :

    git clone --recursive https://github.com/autc04/Retro68.git
    cd Retro68

Compilez la chaîne d'outils — binutils + GCC pour 68k et PowerPC, plus les
outils de l'hôte (Rez, MakePEF, Elf2Mac, ...) — depuis un répertoire de
compilation séparé, vers un `/opt/retro68` vide et accessible en écriture.
Cela prend un certain temps :

    sudo mkdir -p /opt/retro68 && sudo chown $USER /opt/retro68
    mkdir ../Retro68-build && cd ../Retro68-build
    ../Retro68/build-toolchain.bash --prefix=/opt/retro68 --no-carbon
    cd ../Retro68

> Sur un hôte récent (GCC 15+), la compilation de GCC 68k peut échouer dans
> libbacktrace avec le message *"NM has changed"* ; ajoutez `--disable-lto` à
> la ligne `gcc/configure` 68k dans `build-toolchain.bash` (celle du PowerPC
> l'a déjà) et relancez.

Placez la chaîne d'outils dans le `PATH` pour les étapes suivantes :

    export PATH=/opt/retro68/bin:$PATH

Les en-têtes « Multiversal » fournis avec Retro68 sont incomplets ; installez
donc par-dessus les Universal Interfaces 3.x d'Apple. Téléchargez l'image
disque **MPW 3.5 Golden Master** (MacBinary DiskCopy, ~25 Mo, servie sous le
nom `mpw-gm.img__0.bin`) depuis
<http://macintoshgarden.org/apps/macintosh-programmers-workshop> dans le
répertoire courant (sources de Retro68), puis — les arguments du second
script sont build-68k, build-PPC, skip-Carbon :

    ./install-universal-interfaces.sh . mpw-gm.img__0.bin
    ./interfaces-and-libraries.sh /opt/retro68 ./InterfacesAndLibraries true true false

`/opt/retro68/universal/CIncludes` contient désormais les ~390 en-têtes Apple.

## Configurer (une seule fois)

    cd NetHack
    sys/unix/setup.sh sys/unix/hints/linux.501

## Compiler et empaqueter

Lancez les cibles `*pkg` depuis `src/` (`make -C src`) — les chemins Lua
générés par le Makefile principal les font échouer lorsqu'elles sont
invoquées depuis la racine du dépôt.

### 68k

    make CROSS_TO_MAC68K=1 all
    make -C src CROSS_TO_MAC68K=1 mac68kpkg

`targets/mac68k/` : `NetHack.img` (disque SCSI à montage automatique, qui
intègre le pilote `.NHsd` du portage), `NetHack.sit` (StuffIt), `NetHack.bin`
(MacBinary).

### PowerPC

    make CROSS_TO_MACPPC=1 all
    make -C src CROSS_TO_MACPPC=1 macppcpkg

`targets/macppc/` : `NetHack.sit`, `NetHack.bin`.

### Fat (68k + PowerPC)

    make CROSS_TO_MAC68K=1 all
    make -C src CROSS_TO_MAC68K=1 mac68kpkg
    make CROSS_TO_MACPPC=1 all
    make -C src CROSS_TO_MACPPC=1 macfatpkg

`targets/macfat/` : `NetHack.sit`, `NetHack.bin` (+ `Recover.bin`).

## Tester avec QEMU (68k)

    qemu-system-m68k -M q800 -m 128 \
        -bios <quadra-800-rom> \
        -drive file=pram.img,format=raw,if=mtd \
        -drive file=boot.hda,format=raw,media=disk \
        -drive file=targets/mac68k/NetHack.img,format=raw,media=disk \
        -g 800x600x8

`pram.img` (`if=mtd`) conserve la PRAM. Le disque de démarrage doit contenir
System 7.x avec l'adressage 32 bits activé (tableau de bord Mémoire).

## Outils (`sys/mac68k/tools/`)

| Script | Rôle |
|--------|------|
| `make_scsi_image2.py` | Enveloppe une image HFS dans une Apple Partition Map pour SCSI ; `--driver <bin>` intègre le pilote de montage automatique |
| `decode_hqx.py` | Décode le BinHex 4.0 (`.hqx`) en branches de données et de ressources |
| `dump_rsrc.py` | Affiche le contenu de la branche de ressources (types, ID, tailles) |
| `make_info.py` | Écrit un fichier annexe `.info` de macutils pour la préparation StuffIt |
