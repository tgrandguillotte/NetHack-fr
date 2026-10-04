# Version française de NetHack — conventions de traduction

Ce document fixe les règles suivies pour traduire NetHack en français.
Il sert de référence à toute personne (ou tout agent) qui traduit un fichier.

## Principes généraux

* **Encodage** : UTF-8 partout (accents directement dans les chaînes C).
* **Registre** : le jeu **vouvoie** le joueur (« Vous frappez le triton. »).
* **Ponctuation** : espace avant `?`, `!`, `:` et `;` (« Que voulez-vous
  manger ? »). Guillemets droits `"` conservés dans le code (pas de « »
  dans les chaînes C, pour ne pas fausser les largeurs d'affichage).
* Garder le ton humoristique de l'original ; adapter les jeux de mots
  plutôt que de les traduire littéralement.
* **Ne pas traduire** :
  * les messages de débogage (`impossible()`, `panic()`, `paniclog()`,
    `debugpline*()`, `config_error_add()` purement techniques) — facultatif ;
  * les noms d'options, les mots-clés des fichiers de configuration, les
    noms des commandes étendues (`#pray`, `#force`…) — mais **traduire leur
    description** ;
  * toute chaîne comparée par `strcmp`/`strncmp`/`strcmpi`/`!strncmpi` à une
    saisie, à un nom interne ou à une autre chaîne du code (vérifier !) ;
  * les noms de fichiers, identifiants Lua, noms de sons, codes de rôles
    (`"Val"`, `"Arc"`…), touches de commande, réponses `ynq` des
    `yn_function()` (les touches y/n restent celles d'origine) ;
  * les noms propres (dieux, Excalibur, Vlad, Rodney, zorkmid…).
* **Contenu protégé** : ne pas traduire ni recopier de paroles de chansons
  ou d'extraits de livres. Les fichiers `dat/tribute` (extraits de Terry
  Pratchett) et `dat/data.base` (citations littéraires) restent en anglais.
  Une rumeur ou épitaphe qui cite une chanson ou un livre est laissée telle
  quelle ou remplacée par une plaisanterie originale.

## Préfixes des fonctions de message (src/pline.c)

| Fonction       | Préfixe ajouté                    | Exemple de traduction |
|----------------|-----------------------------------|-----------------------|
| `You(...)`     | `"Vous "`                         | `You("frappez %s.", mon_nam(m))` |
| `Your(...)`    | `"Votre "`                        | `Your("sac est vide.")` ; si le nom est pluriel, utiliser `pline("Vos ...")` |
| `You_feel(...)`| `"Vous "` (ou « Vous rêvez que vous ») | `You_feel("vous sentez mieux.")`, `You_feel("sentez une secousse.")` |
| `You_cant(...)`| `"Vous ne pouvez pas "`           | `You_cant("voir cela.")` |
| `You_hear(...)`| `"Vous entendez "`                | `You_hear("une porte s'ouvrir.")` |
| `You_see(...)` | `"Vous voyez "`                   | `You_see("une fumée noire.")` |
| `pline_The(...)` | *aucun*                         | `pline_The("porte s'ouvre.")` devient `pline_The("La porte s'ouvre.")` |
| `There(...)`   | *aucun*                           | `There("is a door here.")` devient `There("Il y a une porte ici.")` |
| `verbalize()`  | guillemets autour du texte        | inchangé |

## Noms et articles

Les fonctions de nommage renvoient maintenant du français **avec l'article
correctement accordé** :

| Fonction                    | Résultat                              |
|-----------------------------|---------------------------------------|
| `mon_nam(m)` / `Monnam(m)`  | « le triton » / « Le triton », « la chauve-souris » |
| `a_monnam(m)` / `Amonnam(m)`| « un triton » / « Un triton »          |
| `y_monnam(m)` / `YMonnam(m)`| « votre chien » / « Votre chien »      |
| `some_mon_nam(m)`           | « quelqu'un » / « un triton »          |
| `Adjmonnam(m, "blessé")`    | « Le chien blessé » (adjectif postposé et accordé) |
| `noit_mon_nam(m)`           | idem `mon_nam` sans « il/elle »        |
| `xname(obj)`                | « épée longue », « potions de soins »  (sans article) |
| `doname(obj)`               | « une épée longue maudite +2 »         |
| `the(xname(o))`/`The(...)`  | « l'épée longue » / « L'épée longue »  |
| `an(xname(o))`/`An(...)`    | « une épée longue », « des flèches »   |
| `yname(o)` / `Yname2(o)`    | « votre épée longue » / « Vos flèches » |
| `du(s)`, `au(s)`, `de(s)`   | contractions : « du dragon », « de l'or », « à l'autel », « aux chiens », « d'or » |
| `accord(s)`                 | suffixe d'accord : `""`, `"e"`, `"s"`, `"es"` |
| `fr_adj("maudit", genre, pluriel)` / `fr_adj_accord("maudit", nom)` | adjectif accordé |
| `UE`                        | `"e"` si l'héroïne est féminine (« Vous êtes mort%s. », `UE`) |
| `MON_IL(m)` / `MON_E(m)`    | « il »/« elle », `""`/`"e"` selon le sexe du monstre |

Exemples :

```c
/* anglais */
pline("%s hits!", Monnam(mtmp));
pline("%s %s.", Yobjnam2(obj, "glow"), hcolor(NH_BLUE));
You("are hit by %s%s", doname(obj), exclam(dam));
/* français */
pline("%s frappe !", Monnam(mtmp));
pline("%s %s.", Yobjnam2(obj, "briller"), hcolor(NH_BLUE));  /* « Votre épée brille bleu. » */
You("êtes touché%s par %s%s", UE, doname(obj), exclam(dam));
```

`s_suffix()` (génitif anglais « 's ») n'a pas d'équivalent : réécrire la
phrase avec `du()`/`de()` (« l'épée de Gandalf », « la tête du triton »).

## Verbes

`otense()`, `vtense()`, `aobjnam()`, `Tobjnam()`, `yobjnam()`, `Yobjnam2()`,
`monverbself()` prennent désormais un **infinitif français** et le conjuguent
au présent (3ᵉ personne singulier/pluriel, ou 2ᵉ pluriel si le sujet est
« vous ») :

```c
pline("%s.", Tobjnam(obj, "disparaître"));   /* « L'épée disparaît. » / « Les flèches disparaissent. » */
pline("%s %s.", The(xname(obj)), otense(obj, "rouiller")); /* « L'épée rouille. » */
```

Pour un verbe pronominal : `"se briser"`, `"s'éteindre"`.
`fr_conj(inf, personne, pluriel)` est disponible directement.

`getobj()`, `ggetobj()`, `query_objlist()`… : le mot-action passé en
argument (« drop », « eat », « put on », « take off »…) est **un identifiant
interne** comparé par `strcmp` dans `invent.c` et ailleurs : **ne pas le
traduire à l'appel**. C'est `invent.c` qui le convertit à l'affichage via
`fr_verbe_getobj(word)` (« manger », « mettre », « retirer »…), pour
construire « Que voulez-vous manger ? ». Règle générale : une chaîne servant
à la fois d'identifiant et de texte affiché reste en anglais dans le code
et est traduite au moment de l'affichage.

## Autres fonctions utilitaires

* `exclam(dam)` renvoie `" !"` ou `"."` (espace insécable de la ponctuation
  française déjà incluse) : écrire `"…%s%s", nom, exclam(dam)` sans espace.
* `body_part(HAND)` renvoie un nom français sans article (« main ») ;
  `makeplural(body_part(HAND))` → « mains » ; utiliser `the()`, `du()`,
  « votre %s » / « vos %s » selon le nombre.
* `hcolor(NH_BLUE)` renvoie un adjectif masculin singulier (« bleu ») ;
  l'accorder avec `fr_adj(hcolor(NH_BLUE), FR_FEM, FALSE)` si besoin
  (« une lueur bleue »).
* `plur(n)` renvoie `"s"` si n ≠ 1 (convient au français dans la plupart des
  cas) ; `ordin(n)` renvoie « er »/« e » (« 1er », « 2e »).
* `currency(n)` renvoie « zorkmid »/« zorkmids ».

## Tampons

Le français est plus long que l'anglais (~20–30 %). Vérifier la taille des
tampons fixes (`char buf[40]`…) qui reçoivent des chaînes traduites et les
agrandir si nécessaire (`BUFSZ` = 256). Conserver tous les spécificateurs
`%s %d %ld %c`… dans le même ordre ; si l'ordre des mots doit changer,
réordonner aussi les arguments.

## Vérification

Après modification d'un fichier C :

```sh
verif.sh fichier.c      # compilation -fsyntax-only, doit afficher OK
```
