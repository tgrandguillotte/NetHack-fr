#!/usr/bin/env bash
# Exécuté une seule fois à la création du codespace / du conteneur :
# installe les outils de compilation puis compile et installe NetHack.
set -euo pipefail

sudo apt-get update
sudo DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    build-essential libncurses-dev uuid-dev python3 tmux

bash "$(dirname "$0")/build.sh"
