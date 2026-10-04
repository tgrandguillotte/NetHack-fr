# À propos
*(Traduction française du document d'origine README.md.)*

Ceci crée une bibliothèque NetHack pouvant être intégrée à d'autres programmes. Deux bibliothèques différentes sont actuellement disponibles :
* libnethack.a - une bibliothèque binaire Unix
* nethack.js / nethack.wasm - une bibliothèque [WebAssembly / WASM](https://webassembly.org/) destinée aux programmes JavaScript (nodejs comme navigateur)

## Compilation
Cette bibliothèque n'a été compilée que sous MacOS, mais devrait fonctionner sous Linux et d'autres plateformes de type Unix. En cas de problème, commencez par reprendre les fichiers d'indications (hints) de `sys/unix/hints` correspondant à votre plateforme. Les contributions pour d'autres plateformes sont les bienvenues.

La compilation du module WASM nécessite que la [chaîne d'outils / le SDK emscripten soit installé](https://emscripten.org/docs/getting_started/downloads.html).

De manière générale, la compilation est identique à la compilation Unix :

[Modification du 4 octobre 2020 : utiliser le Makefile et les hints existants, ainsi que le système hints/include pour la compilation croisée]
1. `cd sys/unix`
2. `./setup.sh hints/macOS.501`
3. `cd ../..`
4. Pour `libnethack.a` : `make WANT_LIBNH=1 all`
5. Pour `nethack.js` : `make CROSS_TO_WASM=1 all`

[Texte d'origine :]
1. `cd sys/lib`
2. Pour `libnethack.a` : `./setup.sh hints/macOS.501` ; pour `nethack.js` : `./setup.sh hints/wasm`
3. `cd ../..`
4. `make`


[Modification du 4 octobre 2020 :]
Les bibliothèques produites se trouveront dans le répertoire `targets/wasm` avec `CROSS_TO_WASM=1`.
Les bibliothèques produites se trouveront dans le répertoire `src` avec `WANT_LIBNH=1`.

[Texte d'origine :]
Les bibliothèques produites se trouveront dans le répertoire `src`.

Pour WASM, il existe aussi un module npm qui peut être publié depuis `sys/lib/npm-library`. Une fois `nethack.js` compilé, il peut être publié ainsi :
1. `cd sys/lib/npm-library`
2. `npm publish`

## API : libnethack.a
L'API se compose de deux fonctions :
* `nhmain(int argc, char *argv[])` - La fonction principale de NetHack, qui configure le programme et exécute `moveloop()` jusqu'à la fin de la partie. Les arguments de cette fonction sont les [arguments de ligne de commande](https://nethackwiki.com/wiki/Options) de NetHack.
* `shim_graphics_set_callback(shim_callback_t cb)` - Une fonction unique qui définit une fonction de rappel (callback) chargée de recevoir les événements graphiques : écrire une chaîne à l'écran, obtenir une saisie de l'utilisateur, etc. À vous de fournir une fonction de rappel et de traiter tous les événements de rendu demandés pour afficher NetHack à l'écran. La fonction de rappel est `void shim_callback_t(const char *name, void *ret_ptr, const char *fmt,  ...)`
  * `name` est le nom de la [fonction de fenêtrage](https://github.com/NetHack/NetHack/blob/NetHack-3.7/doc/window.txt) à traiter
  * `ret_ptr` est un pointeur vers un espace mémoire destiné à la valeur de retour. Le type attendu dans ce pointeur est indiqué par le premier caractère de la chaîne `fmt`.
  * `fmt` est une chaîne qui décrit la signature de la fonction de rappel. Le premier caractère de la chaîne est le type de retour, et les caractères suivants décrivent les arguments variables : `i` pour un entier, `s` pour une chaîne, `p` pour un pointeur, `c` pour un caractère, `v` pour void. Par exemple, si le format est "vis", la fonction de rappel ne renvoie rien (void), le premier argument est un entier et le second une chaîne. Si le format est "iii", la fonction de rappel doit renvoyer un entier, et les deux arguments transmis sont des entiers.
  * [Arguments variadiques](https://www.gnu.org/software/libc/manual/html_node/Variadic-Example.html) : un nombre et des types d'arguments variables selon la `window function` appelée. Les arguments associés à chaque `name` sont décrits dans le fichier [window.txt de NetHack](https://github.com/NetHack/NetHack/blob/NetHack-3.7/doc/window.txt).

Où est le fichier d'en-tête de l'API, me direz-vous ? Il n'y en a pas. Il s'agit de trois fonctions : placez simplement les déclarations anticipées en tête de votre fichier (ou créez votre propre en-tête). Trouver comment installer et copier des fichiers d'en-tête demanderait plus de travail que cela n'en vaut la peine pour une API aussi petite. Si vous n'êtes pas d'accord, n'hésitez pas à proposer une PR pour y remédier. :)

## API : nethack.js
L'API WebAssembly a une signature semblable à celle de `libnethack.a`, à quelques différences syntaxiques près :
* `main(int argc, char argv[])` - La fonction principale de NetHack
* `shim_graphics_set_callback(char *cbName)` - Une `String` représentant le nom d'une fonction de rappel. La fonction de rappel doit être enregistrée sous la forme `globalThis[cbName] = function yourCallback(name, ... args) { /* your stuff */ }`. Notez que [globalThis](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/globalThis) désigne `window` dans les navigateurs et `global` dans node.js.
  * `name` est le nom de la [fonction de fenêtrage](https://github.com/NetHack/NetHack/blob/NetHack-3.7/doc/window.txt) à traiter
  * `... args` est un nombre et des types d'arguments variables selon la `window function` appelée. Les arguments associés à chaque `name` sont décrits dans le fichier [window.txt de NetHack](https://github.com/NetHack/NetHack/blob/NetHack-3.7/doc/window.txt)
  * La fonction doit renvoyer la valeur attendue pour le `name` indiqué


## Stabilité de l'API
L'API « shim graphics » devrait en général rester stable. J'aimerais remplacer les arguments de ligne de commande (argc / argv) par une structure d'options ; les fonctions `nhmain()` et `main()` pourraient donc changer à un moment donné.

## Exemple pour libnethack.a
``` c
#include <stdio.h>

int nhmain(int argc, char *argv[]);
typedef void(*shim_callback_t)(const char *name, void *ret_ptr, const char *fmt, ...);
void shim_graphics_set_callback(shim_callback_t cb);

void window_cb(const char *name, void *ret_ptr, const char *fmt, ...) {
    /* TODO */
}

int main(int argc, char *argv[]) {
    shim_graphics_set_callback(window_cb);
    nhmain(argc, argv);
}
```

## Exemple pour nethack.js
``` js
const path = require("path");

// starts nethack
function nethackStart(cb, inputModule = {}) {
    // set callback
    let cbName = cb.name;
    if (cbName === "") cbName = "__anonymousNetHackCallback";
    let userCallback = globalThis[cbName] = cb;

    // Emscripten Module config
    let Module = inputModule;
    savedOnRuntimeInitialized = Module.onRuntimeInitialized;
    Module.onRuntimeInitialized = function (... args) {
        // after the WASM is loaded, add the shim graphics callback function
        Module.ccall(
            "shim_graphics_set_callback", // C function name
            null, // return type
            ["string"], // arg types
            [cbName], // arg values
            {async: true} // options
        );
    };

    // load and run the module
    var factory = require(path.join(__dirname, "../build/nethack.js"));
    factory(Module);
}

nethackStart(yourCallbackFunction);
```
