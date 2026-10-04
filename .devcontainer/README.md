# Compiler NetHack dans GitHub Codespaces

Ce dossier configure un environnement de développement prêt à l'emploi
(GitHub Codespaces ou « Dev Containers » de VS Code) pour compiler et lancer
NetHack sans rien installer sur votre machine.

## Ouvrir un codespace

1. Sur la page GitHub du dépôt, cliquez sur **Code** → onglet **Codespaces**
   → **Create codespace on …** (choisissez la branche voulue).
2. À la création, le script `.devcontainer/post-create.sh` installe les
   dépendances (`build-essential`, `libncurses-dev`, `uuid-dev`) puis
   compile et installe le jeu dans `playground/`. Comptez quelques minutes.

## Jouer

Dans le terminal du codespace :

```sh
cd playground
./nethack
```

Le terminal intégré est en UTF-8 : les textes accentués s'affichent
correctement.

## Recompiler après une modification

```sh
.devcontainer/build.sh              # interface tty
.devcontainer/build.sh curses clean # tty + curses, en repartant de zéro
```

Le `clean` est nécessaire quand on change d'interfaces : `make` ne recompile
pas les fichiers déjà compilés avec d'autres options.

Dans VS Code, les mêmes actions sont disponibles via **Terminal → Exécuter
la tâche…** : « Compiler NetHack », « Compiler NetHack (tty + curses,
propre) » et « Lancer NetHack ». La compilation est aussi la tâche de build
par défaut (Ctrl+Maj+B).

## Utilisation locale

Avec Docker et l'extension *Dev Containers* de VS Code, la commande
**Dev Containers: Reopen in Container** utilise la même configuration.
