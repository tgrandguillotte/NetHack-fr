# Compiler et lancer NetHack (version française)

Ce document résume comment compiler et jouer à cette version française de
NetHack. Les instructions détaillées par système se trouvent dans
`sys/unix/NewInstall.unx` (Linux, macOS, BSD), `sys/windows/Install.windows`
(Windows) et, pour la compilation croisée (MS-DOS, Amiga…), dans
`Cross-compiling`.

## Linux (Debian, Ubuntu et dérivées)

### 1. Dépendances

```sh
sudo apt install build-essential libncurses-dev uuid-dev
```

* `build-essential` fournit `gcc` (ou utilisez `clang`) et `make` ;
* `libncurses-dev` fournit les bibliothèques `ncursesw` et `tinfo` utilisées
  par l'interface texte ;
* `uuid-dev` est requis par la configuration Linux par défaut.

Sous Fedora : `sudo dnf install gcc make ncurses-devel libuuid-devel`.
Sous Arch : `sudo pacman -S base-devel ncurses util-linux-libs`.

L'interpréteur Lua nécessaire au jeu est inclus dans le dépôt
(`nhlua/lua/`) : rien à télécharger.

### 2. Configuration

Depuis la racine du dépôt :

```sh
sh sys/unix/setup.sh sys/unix/hints/linux.501
```

Cette commande génère les `Makefile` à partir des modèles de `sys/unix/` et du
fichier d'indications choisi. **Relancez-la après toute modification de
`sys/unix/Makefile.*` ou d'un fichier d'indications** (par exemple après avoir
ajouté un fichier source) : les `Makefile` générés ne se mettent pas à jour
tout seuls.

### 3. Compilation et installation

```sh
make all
make install
```

Par défaut, le jeu s'installe pour un seul utilisateur dans le répertoire
`playground/` à la racine du dépôt (ignoré par git). Des messages
`nroff: not found` ou `expr: syntax error` peuvent apparaître pendant la
compilation : ils concernent seulement la documentation et sont sans
conséquence.

Pour ajouter l'interface curses en plus de l'interface tty :

```sh
make clean
make WANT_WIN_TTY=1 WANT_WIN_CURSES=1 all
make WANT_WIN_TTY=1 WANT_WIN_CURSES=1 install
```

Le `make clean` est indispensable quand on change d'interfaces : `make` ne
recompile pas les fichiers déjà compilés avec d'autres options. On choisit
ensuite l'interface au lancement avec l'option `windowtype` (par exemple
`OPTIONS=windowtype:curses` dans `~/.nethackrc`).

### 4. Lancer le jeu

```sh
cd playground
./nethack
```

Utilisez un terminal **en UTF-8** (c'est le cas de la plupart des terminaux
modernes) : les textes du jeu contiennent des lettres accentuées.

## macOS

Installez les outils en ligne de commande de Xcode (`xcode-select --install`),
puis suivez les mêmes étapes avec le fichier d'indications macOS :

```sh
sh sys/unix/setup.sh sys/unix/hints/macOS.501
make all
make install
```

Voir `sys/unix/NewInstall.unx` pour les détails (et `sys/unix/README.xcode`
pour une compilation avec Xcode).

## Windows

Voir `sys/windows/Install.windows`, puis selon votre chaîne d'outils :
`sys/windows/build-vs.txt` (Visual Studio), `sys/windows/build-msys2.txt`
(MSYS2/MinGW) ou `sys/windows/build-nmake.txt` (nmake).

## Particularités de la version française

* **Fichiers de configuration** : les noms d'options (`autopickup`,
  `pettype`, `role`…) et leurs valeurs restent ceux de NetHack ; un fichier
  `~/.nethackrc` existant fonctionne tel quel. Pour le familier, `pettype`
  accepte aussi `chat`, `chien`, `cheval`.
* **Commandes étendues** : leurs noms restent en anglais (`#pray`, `#force`…)
  pour conserver les raccourcis habituels ; leurs descriptions sont traduites.
* **Vœux** : on peut souhaiter un objet en français (« 2 parchemins de
  génocide bénis ») comme en anglais (« blessed scroll of genocide »).
* **Mode assistant** (débogage) : `./nethack -D` pour les utilisateurs listés
  dans la variable `WIZARDS` de `playground/sysconf`.

## Pour les traducteurs et développeurs

* Les conventions de traduction et l'API grammaticale (`the()`, `an()`,
  `du()`, `accord()`, `fr_conj()`…) sont décrites dans `doc/TRADUCTION.md`.
* Le dictionnaire des genres grammaticaux est la liste `src/fr_genres.txt`.
  Après l'avoir modifiée, régénérez l'en-tête compilé :

  ```sh
  python3 util/gen_fr_genres.py
  ```

* `src/noms_en.c` conserve les noms anglais d'origine des objets, monstres et
  éléments de terrain : les niveaux Lua (`dat/*.lua`) les utilisent pour
  désigner objets et monstres, et ils restent acceptés pour les vœux.
* Le manuel du joueur (Guidebook) est traduit dans ses deux sources,
  `doc/Guidebook.mn` (roff) et `doc/Guidebook.tex` (LaTeX). Après avoir
  modifié `Guidebook.mn`, régénérez la version texte (UTF-8) avec groff et
  `col` (paquets `groff` et `bsdextrautils` sous Debian/Ubuntu) :

  ```sh
  sh sys/unix/setup.sh sys/unix/hints/linux.501
  make -C doc Guidebook.txt
  ```

* Pour vérifier rapidement un fichier C modifié sans tout recompiler :

  ```sh
  make -C src francais.o      # remplacez par le fichier voulu
  ```
