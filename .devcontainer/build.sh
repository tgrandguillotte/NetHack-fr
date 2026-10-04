#!/usr/bin/env bash
# Compile et installe NetHack dans le répertoire playground/ du dépôt.
#
# usage : .devcontainer/build.sh [curses] [clean]
#   curses : compile aussi l'interface curses (en plus de tty)
#   clean  : repart de zéro (make clean) avant de compiler
set -euo pipefail

cd "$(dirname "$0")/.."

WANT=(WANT_WIN_TTY=1)
CLEAN=0
for arg in "$@"; do
    case "$arg" in
        curses) WANT+=(WANT_WIN_CURSES=1) ;;
        clean)  CLEAN=1 ;;
        *) echo "argument inconnu : $arg" >&2; exit 2 ;;
    esac
done

# Génère les Makefile à partir du fichier d'indications Linux
sh sys/unix/setup.sh sys/unix/hints/linux.501

if [ "$CLEAN" = 1 ]; then
    make clean
fi

make -j"$(nproc)" "${WANT[@]}" all
make "${WANT[@]}" install

cat <<'FIN'

NetHack est compilé et installé dans playground/.
Pour jouer :      cd playground && ./nethack
Mode assistant :  cd playground && ./nethack -D
FIN
