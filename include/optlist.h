/* NetHack 5.0	optlist.h */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef OPTLIST_H
#define OPTLIST_H

/*
 *  NOTE:  If you add (or delete) an option, please review:
 *             doc/options.txt
 *
 *         It contains how-to info and outlines some required/suggested
 *         updates that should accompany your change.
 */

#define BACKWARD_COMPAT

extern int optfn_boolean(int, int, boolean, char *, char *);

enum OptType { BoolOpt, CompOpt, OthrOpt };
enum Y_N { No, Yes };
enum Off_On { Off, On };
/* Advanced options are only shown in the full, traditional options menu */
enum OptSection {
    OptS_General, OptS_Behavior, OptS_Map, OptS_Status, OptS_Advanced
};
enum menu_terminology_preference {
    Term_False, Term_Off, Term_Disabled, Term_Excluded, num_terms
};

struct allopt_t {
    const char *name;
    enum OptSection section;
    int minmatch;
    int expectedbuf;
    int idx;
    enum optset_restrictions setwhere;
    enum OptType opttyp;
    enum Y_N negateok;
    enum Y_N valok;
    enum Y_N dupeok;
    enum Y_N pfx;
    enum menu_terminology_preference termpref;
    boolean opt_in_out, *addr;
    int (*optfn)(int, int, boolean, char *, char *);
    const char *alias;
    const char *descr;
    const char *prefixgw;
    boolean initval, has_handler, dupdetected, disregarded;
};

#endif /* OPTLIST_H */

#if defined(NHOPT_PROTO) || defined(NHOPT_ENUM) || defined(NHOPT_PARSE)
/* clang-format off */
/* *INDENT-OFF* */

#define NoAlias ((const char *) 0)

#if defined(NHOPT_PROTO)
#define NHOPTB(a, sec, b, c, s, i, n, v, d, al, bp, termp, desc) /*empty*/
#define NHOPTC(a, sec, b, c, s, n, v, d, h, al, z)               \
static int optfn_##a(int, int, boolean, char *, char *);
#define NHOPTP(a, sec, b, c, s, n, v, d, h, al, z)               \
static int pfxfn_##a(int, int, boolean, char *, char *);
#define NHOPTO(m, sec, a, b, c, s, n, v, d, al, z)               \
static int optfn_##a(int, int, boolean, char *, char *);

#elif defined(NHOPT_ENUM)
#define NHOPTB(a, sec, b, c, s, i, n, v, d, al, bp, termp, desc) opt_##a,
#define NHOPTC(a, sec, b, c, s, n, v, d, h, al, z)   opt_##a,
#define NHOPTP(a, sec, b, c, s, n, v, d, h, al, z)   pfx_##a,
#define NHOPTO(m, sec, a, b, c, s, n, v, d, al, z)   opt_##a,

#elif defined(NHOPT_PARSE)
#define NHOPTB(a, sec, b, c, s, i, n, v, d, al, bp, termp, desc)             \
    { #a, OptS_##sec, 0, b, opt_##a, s, BoolOpt, n, v, d, No, termp, c,  \
      bp, &optfn_boolean, al, desc, (const char *) 0, i, 0, 0 , 0 },
#define NHOPTC(a, sec, b, c, s, n, v, d, h, al, z) \
    { #a, OptS_##sec, 0, b, opt_##a, s, CompOpt, n, v, d, No, 0, c,  \
      (boolean *) 0, &optfn_##a, al, z, (const char *) 0, Off, h, 0, 0 },
#define NHOPTP(a, sec, b, c, s, n, v, d, h, al, z) \
    { #a, OptS_##sec, 0, b, pfx_##a, s, CompOpt, n, v, d, Yes, 0, c, \
      (boolean *) 0, &pfxfn_##a, al, z, #a, Off, h, 0, 0 },
#define NHOPTO(m, sec, a, b, c, s, n, v, d, al, z) \
    { m, OptS_##sec, 0, b, opt_##a, s, OthrOpt, n, v, d, No, 0, c,   \
      (boolean *) 0, &optfn_##a, al, z, (const char *) 0, On, On, 0, 0 },

/* this is not reliable because TILES_IN_GLYPHMAP might be defined
 * in a multi-interface binary but not apply to the current interface */
#ifdef TILES_IN_GLYPHMAP
#define tiled_map_Def On
#define ascii_map_Def Off
#else
#define ascii_map_Def On
#define tiled_map_Def Off
#endif
#endif

/* B:nm, sec, ln, opt_*, setwhere?, on?, negat?, val?, dup?, hndlr? Alias,
            bool_p, term */
/* C:nm, sec, ln, opt_*, setwhere?, negateok?, valok?, dupok?, hndlr? Alias,
            desc */
/* P:pfx, sec, ln, opt_*, setwhere?, negateok?, valok?, dupok?, hndlr? Alias,
            desc*/
    /*
     * Most of the options are in alphabetical order; a few are forced
     * to the top of list so that doset() will list them first and
     * all_options_str() will gather them first to write to the top of
     * a new RC file by #saveoptions.
     *
     * windowtype comes first because its value can affect how wc_ and
     * wc2_ options are processed; playmode (for players who can't or
     * don't know how to specify a command line) and name (ditto, more
     * or less) come next; then role, race, gender, align.  Those will
     * be at the top of the file for #saveoptions constructed RC file.
     */
    NHOPTC(windowtype, Advanced, WINTYPELEN, opt_in, set_gameview,
                No, Yes, No, No, NoAlias,
                "système de fenêtrage à utiliser (à indiquer en premier)")
    NHOPTC(playmode, Advanced, 8, opt_in, set_gameview,
                No, Yes, No, No, NoAlias,
                "jeu normal, mode exploration (sans score) ou mode débogage")
    NHOPTC(name, Advanced, PL_NSIZ, opt_in, set_gameview,
                No, Yes, No, No, NoAlias,
                "nom de votre personnage (ex. : name:Merlin-W)")
    NHOPTC(role, Advanced, PL_CSIZ, opt_in, set_gameview,
                Yes, Yes, Yes, No, "character",
                "votre rôle de départ (ex. : Barbarian, Valkyrie)")
    NHOPTC(race, Advanced, PL_CSIZ, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias,
                "votre race de départ (ex. : Human, Elf)")
    NHOPTC(gender, Advanced, 8, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias,
                "votre sexe de départ (male ou female)")
    NHOPTC(alignment, Advanced, 8, opt_in, set_gameview,
                Yes, Yes, Yes, No, "align",
                "votre alignement de départ (lawful, neutral ou chaotic)")
    /* end of special ordering; remainder of entries are in alphabetical order
     */
    NHOPTB(accessiblemsg, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &a11y.accessiblemsg, Term_False,
           "ajouter la position aux messages")
    NHOPTB(acoustics, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.acoustics, Term_False,
           "votre personnage entend-il quelque chose")
 /* NHOPTC(align) -- moved to top */
    NHOPTC(align_message, Advanced, 20, opt_in, set_gameview,
                Yes, Yes, No, Yes, NoAlias, "alignement de la fenêtre de messages")
    NHOPTC(align_status, Advanced, 20, opt_in, set_gameview,
                No, Yes, No, Yes, NoAlias, "alignement de la fenêtre d'état")
#ifdef WIN32
    NHOPTC(altkeyhandling, Advanced, 20, opt_in, set_in_game,
                No, Yes, No, Yes, "altkeyhandler", "gestion alternative des touches")
#else
    NHOPTC(altkeyhandling, Advanced, 20, opt_in, set_in_config,
                No, Yes, No, Yes, "altkeyhandler", "(sans objet)")
#endif
#ifdef ALTMETA
    NHOPTB(altmeta, Advanced, 0, opt_out, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.altmeta, Term_False,
           "traiter \"ESC c\" comme M-c (Méta+c, 8e bit activé)")
#elif defined(AMIGA_INTUITION)
    NHOPTB(altmeta, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &sysflags.altmeta, Term_False,
           "traiter ALT+c comme M-c (Méta+c, 8e bit activé)")
#else
    NHOPTB(altmeta, Advanced, 0, opt_out, set_in_config,
           Off, Yes, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
    NHOPTB(armorstatus, Advanced, 0, opt_in, set_in_game,
                Off, Yes, No, No, NoAlias, &flags.armorstatus, Term_False,
                "résumer l'armure portée dans un champ d'état")
    /* this one needs unique handling because different window ports
       expect different defaults */
    NHOPTB(ascii_map, Advanced, 0, ascii_map_Def, set_in_game,
                ascii_map_Def, Yes, No, No, NoAlias, &iflags.wc_ascii_map,
                Term_False, "afficher la carte en mode texte")
    NHOPTO("autocompletions", Advanced, o_autocomplete, BUFSZ, opt_in,
                set_in_game, No, Yes, No, NoAlias, "modifier les complétions automatiques")
    NHOPTB(autodescribe, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &iflags.autodescribe, Term_False,
           "décrire le terrain sous le curseur")
    NHOPTB(autodig, Behavior, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.autodig, Term_False,
           "creuser en se déplaçant avec un outil de creusage en main")
    NHOPTB(autoopen, Behavior, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.autoopen, Term_False,
           "marcher dans une porte tente de l'ouvrir")
    NHOPTB(autopickup, Behavior, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.pickup, Term_False,
           "ramasser automatiquement les objets")
    NHOPTO("autopickup exceptions", Behavior, o_autopickup_exceptions, BUFSZ,
                opt_in, set_in_game,
                No, Yes, No, NoAlias, "modifier les exceptions de ramassage automatique")
    NHOPTB(autoquiver, Behavior, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.autoquiver, Term_False,
           "remplir automatiquement le carquois vide lors d'un tir")
    NHOPTC(autounlock, Behavior, 80, opt_out, set_in_game,
                Yes, Yes, No, Yes, NoAlias,
                "action face à une porte ou un coffre verrouillé")
    NHOPTB(bgcolors, Map, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &iflags.bgcolors, Term_Off,
           "utiliser la couleur de fond pour certaines mises en valeur de la carte")
    NHOPTO("bind keys", Advanced, o_bind_keys, BUFSZ, opt_in, set_in_game,
                No, Yes, No, NoAlias, "modifier les associations de touches")
#if defined(MICRO) && !defined(AMIGA)
    NHOPTB(BIOS, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, &iflags.BIOS, Term_False,
           "utiliser les appels ROM BIOS IBM")
#else
    NHOPTB(BIOS, Advanced, 0, opt_in, set_in_config,
           Off, No, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
    NHOPTB(blind, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, "permablind", &u.uroleplay.blind, Term_False,
           "votre personnage est aveugle en permanence")
    NHOPTB(bones, Advanced, 0, opt_out, set_in_config,
           On, Yes, No, No, NoAlias, &flags.bones, Term_False,
           "autoriser le chargement des fichiers d'ossements")
#ifdef BACKWARD_COMPAT
    NHOPTC(boulder, Advanced, 1, opt_in, set_in_game,
                No, Yes, No, No, NoAlias,
                "obsolète (utilisez plutôt S_boulder dans le fichier de symboles)")
#endif
    NHOPTC(catname, Advanced, PL_PSIZ, opt_in, set_gameview,
                No, Yes, No, No, NoAlias,
                "nom de votre familier de départ si c'est un chaton")
#ifdef INSURANCE
    NHOPTB(checkpoint, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.ins_chkpt, Term_False,
           "sauvegarder la partie à chaque changement de niveau")
#else
    NHOPTB(checkpoint, Advanced, 0, opt_out, set_in_config,
           Off, No, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
    NHOPTB(cmdassist, Behavior, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &iflags.cmdassist, Term_False,
           "aider en cas d'erreur de saisie de direction")
    NHOPTB(color, Map, 0, opt_out, set_in_game,
           On, Yes, No, No, "colour", &iflags.wc_color, Term_False,
           "utiliser la couleur sur la carte")
    NHOPTB(confirm, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.confirm, Term_False,
           "demander avant de frapper un monstre apprivoisé ou pacifique")
#ifdef CRASHREPORT
    NHOPTC(crash_email, Advanced, PL_NSIZ, opt_in, set_in_game,
                No, Yes, No, No, NoAlias,
                "adresse électronique pour les rapports")
    NHOPTC(crash_name, Advanced, PL_NSIZ, opt_in, set_in_game,
                No, Yes, No, No, NoAlias,
                "votre nom pour les rapports")
    NHOPTC(crash_urlmax, Advanced, PL_NSIZ, opt_in, set_in_game,
                No, Yes, No, No, NoAlias,
                "longueur de la plus longue URL générable")
#endif
#ifdef CURSES_GRAPHICS
    NHOPTC(cursesgraphics, Advanced, 70, opt_in, set_in_config,
                No, Yes, No, No, NoAlias,
                "charger les symboles d'affichage curses dans le jeu de symboles")
#endif
    NHOPTB(customcolors, Map, 0, opt_out, set_in_game,
           On, Yes, No, No, "customcolours", &iflags.customcolors,
           Term_False, "utiliser des couleurs personnalisées sur la carte")
    NHOPTB(customsymbols, Map, 0, opt_out, set_in_game,
           On, Yes, No, No, "customsymbols", &iflags.customsymbols,
           Term_False, "utiliser des symboles utf8 personnalisés sur la carte")
    NHOPTB(dark_room, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.dark_room, Term_False,
           "afficher différemment le sol hors du champ de vision")
    NHOPTB(deaf, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, "permadeaf", &u.uroleplay.deaf, Term_False,
           "votre personnage est sourd en permanence")
#ifdef BACKWARD_COMPAT
    NHOPTC(DECgraphics, Advanced, 70, opt_in, set_in_config,
                Yes, Yes, No, No, NoAlias,
                "charger les symboles DECGraphics dans le jeu de symboles")
#endif
    NHOPTB(debug_hunger, Advanced, 0, opt_in, set_wiznofuz,
           Off, Yes, No, No, NoAlias, &iflags.debug_hunger, Term_False,
           "pas de faim")
    NHOPTB(debug_mongen, Advanced, 0, opt_in, set_wiznofuz,
           Off, Yes, No, No, NoAlias, &iflags.debug_mongen, Term_False,
           "pas de génération aléatoire de monstres")
    NHOPTB(debug_overwrite_stairs, Advanced, 0, opt_in, set_wiznofuz,
                Off, Yes, No, No, NoAlias, &iflags.debug_overwrite_stairs,
           Term_False, "la génération de niveau peut écraser les escaliers")
    NHOPTC(disclose, Advanced, sizeof flags.end_disclose * 2,
                opt_in, set_in_game,
                Yes, Yes, No, Yes, NoAlias,
                "les informations à révéler en fin de partie")
    NHOPTC(dogname, Advanced, PL_PSIZ, opt_in, set_gameview,
                No, Yes, No, No, NoAlias,
                "nom de votre familier de départ si c'est un petit chien")
    NHOPTB(dropped_nopick, Behavior, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.nopick_dropped, Term_False,
           "ne pas ramasser automatiquement les objets lâchés")
    NHOPTC(dungeon, Advanced, MAXDCHARS + 1,opt_in, set_in_config,
                No, Yes, No, No, NoAlias,
                "liste des symboles pour dessiner la carte du donjon")
    NHOPTC(effects, Advanced, MAXECHARS + 1, opt_in, set_in_config,
                No, Yes, No, No, NoAlias,
                "liste des symboles pour dessiner les effets spéciaux")
    NHOPTB(eight_bit_tty, Advanced, 0, opt_in, set_in_game,
                Off, Yes, No, No, NoAlias, &iflags.wc_eight_bit_input,
           Term_False, "envoyer directement les caractères 8 bits au terminal")
    NHOPTB(extmenu, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.extmenu, Term_False,
           "utiliser un menu pour les commandes étendues")
    NHOPTB(female, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, "male", &flags.female, Term_False,
           "obsolète ; utilisez gender:female")
    NHOPTB(fireassist, Behavior, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &iflags.fireassist, Term_False,
           "la commande de tir essaie de vous aider")
    NHOPTB(fixinv, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.invlet_constant, Term_False,
           "les objets de l'inventaire gardent leur lettre")
    NHOPTC(font_map, Advanced, 40, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias, "police de la fenêtre de carte")
    NHOPTC(font_menu, Advanced, 40, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias, "police des menus")
    NHOPTC(font_message, Advanced, 40, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias,
                "police de la fenêtre de messages")
    NHOPTC(font_size_map, Advanced, 20, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias, "taille de la police de la carte")
    NHOPTC(font_size_menu, Advanced, 20, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias, "taille de la police des menus")
    NHOPTC(font_size_message, Advanced, 20, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias, "taille de la police des messages")
    NHOPTC(font_size_status, Advanced, 20, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias, "taille de la police d'état")
    NHOPTC(font_size_text, Advanced, 20, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias, "taille de la police du texte")
    NHOPTC(font_status, Advanced, 40, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias, "police de la fenêtre d'état")
    NHOPTC(font_text, Advanced, 40, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias, "police des fenêtres de texte")
    NHOPTB(force_invmenu, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.force_invmenu, Term_False,
           "les commandes demandant un objet affichent un menu")
    NHOPTC(fruit, General, PL_FSIZ, opt_in, set_in_game,
                No, Yes, No, No, NoAlias, "nom d'un fruit que vous aimez manger")
    NHOPTB(fullscreen, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, &iflags.wc2_fullscreen, Term_False,
           "basculer en plein écran")
 /* NHOPTC(gender) -- moved to top */
    NHOPTC(glyph, Advanced, 40, opt_in, set_in_game,
                No, Yes, Yes, No, NoAlias,
                "représenter un glyphe par une valeur unicode et une couleur")
    NHOPTB(goldX, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.goldX, Term_False,
           "classer l'or comme inconnu ou non maudit")
    NHOPTB(guicolor, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &iflags.wc2_guicolor, Term_False,
           "utiliser la couleur dans l'interface")
    NHOPTB(help, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.help, Term_False,
           "tout afficher avec la commande d'identification de symbole")
    NHOPTB(herecmd_menu, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.herecmd_menu, Term_False,
           "afficher les commandes disponibles à cet endroit")
#if 0
/* there is no optfn_hicolor() defined in options.c presently
   and that is required for NHOPTC */
#if defined(MAC68K)
    NHOPTC(hicolor, Advanced, 15, opt_in, set_in_config,
                No, Yes, No, No, NoAlias,
                "comme palette, mais dans l'ordre inverse")
#endif
#endif /* 0 */
    NHOPTB(hilite_pet, Map, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.wc_hilite_pet, Term_False,
           "mettre en valeur les familiers")
    NHOPTB(hilite_pile, Map, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.hilite_pile, Term_False,
           "mettre en valeur les tas d'objets")
#ifdef STATUS_HILITES
    NHOPTC(hilite_status, Advanced, 13, opt_out, set_in_game,
                Yes, Yes, Yes, No, NoAlias,
                "une règle de mise en valeur d'état (peut être répétée)")
#else
    NHOPTC(hilite_status, Advanced, 13, opt_out, set_in_config,
                Yes, Yes, Yes, No, NoAlias, "(indisponible)")
#endif
    NHOPTB(hitpointbar, Status, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.wc2_hitpointbar, Term_False,
           "afficher une barre colorée des points de vie")
    NHOPTC(horsename, Advanced, PL_PSIZ, opt_in, set_gameview,
                No, Yes, No, No, NoAlias,
                "nom de votre familier de départ si c'est un poney")
#ifdef BACKWARD_COMPAT
    NHOPTC(IBMgraphics, Advanced, 70, opt_in, set_in_config,
                Yes, Yes, No, No, NoAlias,
                "charger les symboles IBMGraphics dans le jeu de symboles")
#endif
    NHOPTB(idlecheckpoint, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.idlecheckpoint, Term_Off,
           "mettre à jour la sauvegarde après 10 secondes d'inactivité")
#ifndef MAC68K
    NHOPTB(ignintr, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.ignintr, Term_False,
           "ignorer les signaux d'interruption")
#else
    NHOPTB(ignintr, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
    NHOPTB(implicit_uncursed, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.implicit_uncursed, Term_False,
           "omettre \"non maudit\" dans l'inventaire")
#if 0   /* obsolete - pre-OSX Mac */
    NHOPTB(large_font, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, &iflags.obsolete,
           (char *)0)
#endif
    NHOPTB(legacy, Advanced, 0, opt_out, set_in_config,
           On, Yes, No, No, NoAlias, &flags.legacy, Term_False,
           "afficher le message d'introduction")
    NHOPTB(lit_corridor, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.lit_corridor, Term_False,
           "afficher éclairés les couloirs sombres en vue")
    NHOPTB(lootabc, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.lootabc, Term_False,
           "utiliser a/b/c plutôt que o/i/c pour fouiller")
    NHOPTB(mail, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.biff, Term_False,
           "activer le démon du courrier")
    NHOPTC(map_mode, Advanced, 20, opt_in, set_gameview,
                Yes, Yes, Yes, No, NoAlias, "mode d'affichage de la carte sous Windows")
    NHOPTB(mention_decor, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.mention_decor, Term_False,
           "signaler les éléments intéressants sur lesquels vous marchez")
    NHOPTB(mention_map, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &a11y.glyph_updates, Term_False,
           "signaler les changements d'endroits intéressants de la carte")
    NHOPTB(mention_walls, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.mention_walls, Term_False,
           "signaler quand vous marchez dans un mur")
    NHOPTC(menu_deselect_all, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "désélectionner tous les éléments d'un menu")
    NHOPTC(menu_deselect_page, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias,
                "désélectionner tous les éléments de cette page du menu")
    NHOPTC(menu_first_page, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "aller à la première page d'un menu")
    NHOPTC(menu_headings, Advanced, 4, opt_in, set_in_game,
                Yes, Yes, No, Yes, NoAlias, "style d'affichage des en-têtes de menu")
    NHOPTC(menu_invert_all, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "inverser la sélection de tous les éléments d'un menu")
    NHOPTC(menu_invert_page, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias,
                "inverser la sélection sur cette page du menu")
    NHOPTC(menu_last_page, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "aller à la dernière page d'un menu")
    NHOPTC(menu_next_page, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "aller à la page suivante du menu")
    NHOPTC(menu_objsyms, Advanced, 12, opt_in, set_in_game,
           Yes, Yes, No, Yes, "use_menu_glyphs",
           "afficher les symboles des objets dans les menus")
#ifdef TTY_GRAPHICS
    NHOPTB(menu_overlay, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &iflags.menu_overlay, Term_False,
           "menus superposés et alignés à droite")
#else
    NHOPTB(menu_overlay, Advanced, 0, opt_in, set_in_config,
           Off, No, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
    NHOPTC(menu_previous_page, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "aller à la page précédente du menu")
    NHOPTC(menu_search, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "rechercher un élément du menu")
    NHOPTC(menu_select_all, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "sélectionner tous les éléments d'un menu")
    NHOPTC(menu_select_page, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias,
                "sélectionner tous les éléments de cette page du menu")
    NHOPTC(menu_shift_left, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "faire défiler la page du menu vers la gauche")
    NHOPTC(menu_shift_right, Advanced, 4, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "faire défiler la page du menu vers la droite")
    NHOPTB(menu_tab_sep, Advanced, 0, opt_in, set_wizonly,
           Off, Yes, No, No, NoAlias, &iflags.menu_tab_sep, Term_False,
           "mise en forme des menus")
    NHOPTB(menucolors, Advanced, 0, opt_in, set_in_game,
           Off, Yes, Yes, No, NoAlias, &iflags.use_menu_color, Term_False,
           "utiliser des couleurs dans les menus")
    NHOPTO("menu colors", Status, o_menu_colors, BUFSZ, opt_in, set_in_game,
                No, Yes, No, NoAlias, "modifier les couleurs des menus")
    NHOPTC(menuinvertmode, Advanced, 5, opt_in, set_in_game,
                No, Yes, No, No, NoAlias,
                "comportement expérimental des inversions de menu")
    NHOPTC(menustyle, Advanced, MENUTYPELEN, opt_in, set_in_game,
                Yes, Yes, No, Yes, NoAlias,
                "interface de sélection des objets")
    NHOPTO("message types", Advanced, o_message_types, BUFSZ,
                opt_in, set_in_game,
                No, Yes, No, NoAlias, "modifier les types de messages")
    NHOPTB(mon_movement, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &a11y.mon_movement, Term_False,
           "message quand le héros voit un monstre bouger")
    NHOPTB(monpolycontrol, Advanced, 0, opt_in, set_wizonly,
           Off, Yes, No, No, NoAlias, &iflags.mon_polycontrol, Term_False,
           "contrôler les métamorphoses des monstres")
    NHOPTB(montelecontrol, Advanced, 0, opt_in, set_wizonly,
           Off, Yes, No, No, NoAlias, &iflags.mon_telecontrol, Term_False,
           "contrôler la destination des téléportations de monstres")
    NHOPTC(monsters, Advanced, MAXMCLASSES, opt_in, set_in_config,
                No, Yes, No, No, NoAlias,
                "liste des symboles des monstres")
    NHOPTC(mouse_support, Advanced, 0, opt_in, set_in_game,
                No, Yes, No, No, NoAlias,
                "le jeu reçoit les clics de la souris")
#if PREV_MSGS /* tty or curses */
    NHOPTC(msg_window, Advanced, 1, opt_in, set_in_game,
                Yes, Yes, No, Yes, NoAlias,
                "comportement de \"voir les messages précédents\" (^P)")
#else
    NHOPTC(msg_window, Advanced, 1, opt_in, set_in_config,
                Yes, Yes, No, Yes, NoAlias, "(sans objet)")
#endif
    NHOPTC(msghistory, Advanced, 5, opt_in, set_gameview,
                Yes, Yes, No, No, NoAlias,
                "nombre de messages de la ligne du haut à conserver")
 /* NHOPTC(name) -- moved to top */
#ifdef NEWS
    NHOPTB(news, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, &iflags.news, Term_False,
           "afficher les nouvelles en début de partie")
#else
    NHOPTB(news, Advanced, 0, opt_in, set_in_config,
           Off, No, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
    NHOPTB(nudist, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, &u.uroleplay.nudist, Term_False,
           "commencer sans armure")
    NHOPTB(null, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.null, Term_False,
           "autoriser l'envoi de caractères nuls au terminal")
    NHOPTC(number_pad, General, 1, opt_in, set_in_game,
                No, Yes, No, Yes, NoAlias,
                "utiliser le pavé numérique pour se déplacer")
    NHOPTC(objects, Advanced, MAXOCLASSES, opt_in, set_in_config,
                No, Yes, No, No, NoAlias,
                "liste des symboles des objets")
    NHOPTC(packorder, Advanced, MAXOCLASSES, opt_in, set_in_game,
                No, Yes, No, No, NoAlias,
                "ordre des objets dans votre inventaire")
#ifdef CHANGE_COLOR
#ifndef MAC68K     /* not old Mac OS9 */
    NHOPTC(palette, Advanced, 15, opt_in, set_gameview,
                No, Yes, Yes, No, "hicolor",
                "palette (ajuste une couleur RVB de la palette (couleur/R-V-B)")
#else
    NHOPTC(palette, Advanced, 15, opt_in, set_in_game,
                No, Yes, Yes, No, "hicolor",
                "palette (00c/880/-fff = bleu/jaune/blanc inversé)")
#endif
#endif
    /* prior to paranoid_confirmation, 'prayconfirm' was a distinct option */
    NHOPTC(paranoid_confirmation, Advanced, 28, opt_in, set_in_game,
                Yes, Yes, Yes, Yes, "prayconfirm",
                "confirmations supplémentaires dans certaines situations")
    NHOPTB(pauper, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, &u.uroleplay.pauper, Term_False,
           "commencer sans aucun objet")
    NHOPTB(perm_invent, Advanced, 0, opt_in, set_in_game,
                Off, Yes, No, No, NoAlias, &iflags.perm_invent, Term_Off,
                "afficher la fenêtre d'inventaire permanente")
    NHOPTC(perminv_mode, Advanced, 20, opt_in, set_in_game,
                Yes, Yes, No, Yes, NoAlias,
                "contenu de la fenêtre d'inventaire permanente")
    NHOPTC(petattr, Advanced, 88, opt_in, set_in_game, /* tty/curses only */
                No, Yes, No, Yes, NoAlias, "attributs de mise en valeur des familiers")
    /* pettype is ignored for some roles */
    NHOPTC(pettype, Advanced, 4, opt_in, set_gameview,
                Yes, Yes, No, No, "pet", "votre type de familier de départ préféré")
    NHOPTC(pickup_burden, Advanced, 20, opt_in, set_in_game,
                No, Yes, No, Yes, NoAlias,
                "charge maximale ramassée avant confirmation")
    NHOPTB(pickup_stolen, Behavior, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.pickup_stolen, Term_False,
           "ramasser automatiquement les objets volés")
    NHOPTB(pickup_thrown, Behavior, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.pickup_thrown, Term_False,
           "ramasser automatiquement les objets lancés")
    NHOPTC(pickup_types, Behavior, MAXOCLASSES, opt_in, set_in_game,
                No, Yes, No, Yes, NoAlias,
                "types d'objets à ramasser automatiquement")
    NHOPTC(pile_limit, Advanced, 24, opt_in, set_in_game,
                Yes, Yes, No, No, NoAlias,
                "seuil pour \"il y a beaucoup d'objets ici\"")
    NHOPTC(player_selection, Advanced, 12, opt_in, set_gameview,
                No, Yes, No, No, NoAlias,
                "choisir le personnage par dialogue ou questions")
 /* NHOPTC(playmode) -- moved to top */
    NHOPTB(popup_dialog, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.wc_popup_dialog, Term_False,
           (char *)0)
    NHOPTB(preload_tiles, Advanced, 0, opt_out, set_in_config, /* MSDOS only */
           On, Yes, No, No, NoAlias, &iflags.wc_preload_tiles, Term_False,
           (char *)0)
    NHOPTB(price_quotes, General, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.pricequotes, Term_False,
           "afficher les prix vus pour les objets non identifiés")
    NHOPTB(pushweapon, Behavior, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.pushweapon, Term_False,
           "l'arme précédente passe en arme secondaire")
    NHOPTB(query_menu, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.query_menu, Term_False,
           "utiliser un menu pour les questions oui/non")
    NHOPTB(quick_farsight, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.quick_farsight, Term_False,
           "sauter le parcours de la carte quand on doit la regarder")
 /* NHOPTC(race) -- moved to top */
#ifdef MICRO
    NHOPTB(rawio, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, &iflags.rawio, Term_False,
           "autoriser les E/S brutes")
#else
    NHOPTB(rawio, Advanced, 0, opt_in, set_in_config,
           Off, No, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
    NHOPTB(reroll, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, &u.uroleplay.reroll, Term_False,
           "autoriser le retirage de l'inventaire de départ")
    NHOPTB(rest_on_space, Advanced, 0, opt_in, set_in_game, Off,
           Yes, No, No, NoAlias, &flags.rest_on_space, Term_False,
           "la barre d'espace est associée à la commande de repos")
    NHOPTC(roguesymset, Advanced, 70, opt_in, set_in_game,
                No, Yes, No, Yes, NoAlias,
                "charger un jeu de symboles rogue depuis le fichier de symboles")
 /* NHOPTC(role) -- moved to top */
    NHOPTC(runmode, Advanced, sizeof "teleport", opt_in, set_in_game,
                Yes, Yes, No, Yes, NoAlias,
                "fréquence d'affichage pendant la course ou le voyage")
    NHOPTB(safe_pet, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.safe_dog, Term_False,
           "vous empêcher de frapper vos familiers")
    NHOPTB(safe_wait, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.safe_wait, Term_False,
           "empêcher d'attendre à côté d'ennemis")
    NHOPTB(sanity_check, Advanced, 0, opt_in, set_wizonly,
           Off, Yes, No, No, NoAlias, &iflags.sanity_check, Term_False,
           "effectuer des contrôles de cohérence")
    NHOPTC(scores, Advanced, 32, opt_in, set_in_game,
                No, Yes, No, No, NoAlias,
                "les parties du tableau des scores à afficher")
    NHOPTC(scroll_amount, Advanced, 20, opt_in, set_gameview,
                Yes, Yes, No, No, NoAlias,
                "défilement de la carte quand scroll_margin est atteint")
    NHOPTC(scroll_margin, Advanced, 20, opt_in, set_gameview,
                Yes, Yes, No, No, NoAlias,
                "faire défiler la carte à cette distance du bord")
    NHOPTB(selectsaved, Advanced, 0, opt_out, set_in_config,
           On, Yes, No, No, NoAlias, &iflags.wc2_selectsaved, Term_False,
           (char *)0)
    NHOPTB(showdamage, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.showdamage, Term_False,
           "afficher les dégâts subis dans la ligne de messages")
    NHOPTB(showexp, Status, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.showexp, Term_False,
           "afficher les points d'expérience dans la ligne d'état")
    NHOPTB(showrace, Map, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.showrace, Term_False,
           "représenter votre personnage par sa race plutôt que son rôle")
#ifdef SCORE_ON_BOTL
    NHOPTB(showscore, Status, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.showscore, Term_False,
           "afficher le score dans la ligne d'état")
#else
    NHOPTB(showscore, Status, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
    NHOPTB(showvers, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.showvers, Term_False,
           "afficher la version dans la ligne d'état")
    NHOPTB(silent, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.silent, Term_False,
           "ne pas utiliser la sonnerie du terminal")
    NHOPTB(softkeyboard, Advanced, 0, opt_in, set_in_config,
                Off, Yes, No, No, NoAlias, &iflags.wc2_softkeyboard,
           Term_False, (char *)0)
    NHOPTC(sortdiscoveries, Advanced, 0, opt_in, set_in_game,
                Yes, Yes, No, Yes, NoAlias,
                "ordre d'affichage des objets découverts")
    NHOPTC(sortloot, Advanced, 4, opt_in, set_in_game,
                No, Yes, No, Yes, NoAlias,
                "trier les listes de sélection d'objets par description")
    NHOPTB(sortpack, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.sortpack, Term_False,
           "grouper les objets de l'inventaire par type")
    NHOPTC(sortvanquished, Advanced, 0, opt_in, set_in_game,
                Yes, Yes, No, Yes, NoAlias,
                "ordre d'affichage des monstres vaincus")
    NHOPTC(soundlib, Advanced, WINTYPELEN, opt_in, set_gameview,
                No, Yes, No, No, NoAlias,
                "interface soundlib à utiliser (le cas échéant)")
#ifdef SND_LIB_INTEGRATED
    NHOPTB(sounds, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &iflags.sounds, Term_Off,
           "utiliser les effets sonores intégrés")
#else
    NHOPTB(sounds, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.sounds, Term_Off,
           "utiliser les sons")
#endif
    NHOPTB(sparkle, Map, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.sparkle, Term_False,
           "afficher un scintillement lors d'une résistance à la magie")
    NHOPTB(spot_monsters, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &a11y.mon_notices, Term_False,
           "message quand le héros repère un monstre")
    NHOPTB(splash_screen, Advanced, 0, opt_out, set_in_config,
           On, Yes, No, No, NoAlias, &iflags.wc_splash_screen, Term_False,
           (char *)0)
    NHOPTB(standout, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.standout, Term_False,
           "mettre en évidence --More--")
    NHOPTB(status_updates, Advanced, 0, opt_out, set_in_config,
           On, Yes, No, No, NoAlias, &iflags.status_updates, Term_False,
           "autoriser la mise à jour des lignes d'état")
    NHOPTO("status condition fields", Status, o_status_cond, BUFSZ,
                opt_in, set_in_game,
                No, Yes, No, NoAlias, "modifier la mise en valeur des conditions d'état")
#ifdef STATUS_HILITES
    NHOPTC(statushilites, Advanced, 20, opt_in, set_in_game,
                Yes, Yes, Yes, No, NoAlias,
                "0=pas de mise en valeur d'état, N=mise en valeur pendant N tours")
    NHOPTO("status highlight rules", Status, o_status_hilites, BUFSZ,
                opt_in, set_in_game,
                No, Yes, No, NoAlias, "modifier la mise en valeur de la ligne d'état")
#else
    NHOPTC(statushilites, Advanced, 20, opt_in, set_in_config,
                Yes, Yes, Yes, No, NoAlias, "contrôle de la mise en valeur")
#endif
    NHOPTC(statuslines, Status, 20, opt_in, set_in_game,
                No, Yes, No, No, NoAlias, "2 ou 3 lignes pour l'affichage de l'état")
#ifdef WIN32CON
    NHOPTC(subkeyvalue, Advanced, 7, opt_in, set_in_config,
                No, Yes, Yes, No, NoAlias, "remplacer la valeur d'une touche")
#endif
    NHOPTC(suppress_alert, Advanced, 8, opt_in, set_in_game,
                No, Yes, Yes, No, NoAlias,
                "supprimer les alertes sur les fonctions propres à une version")
    NHOPTC(symset, Map, 70, opt_in, set_in_game,
                No, Yes, No, Yes, NoAlias,
                "charger un jeu de symboles depuis le fichier de symboles")
    NHOPTC(term_cols, Advanced, 6, opt_in, set_in_config,
                No, Yes, No, No, "termcolumns", "nombre de colonnes")
    NHOPTC(term_rows, Advanced, 6, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "nombre de lignes")
    NHOPTB(terrainstatus, Advanced, 0, opt_in, set_in_game,
                Off, Yes, No, No, NoAlias, &flags.terrainstatus, Term_False,
                "afficher la position du héros dans un champ d'état")
    NHOPTC(tile_file, Advanced, 70, opt_in, set_gameview,
                No, Yes, No, No, NoAlias, "nom du fichier de tuiles")
    NHOPTC(tile_height, Advanced, 20, opt_in, set_gameview,
                Yes, Yes, No, No, NoAlias, "hauteur des tuiles")
    NHOPTC(tile_width, Advanced, 20, opt_in, set_gameview,
                Yes, Yes, No, No, NoAlias, "largeur des tuiles")
    NHOPTB(tiled_map, Advanced, 0, opt_in, set_in_game,
                tiled_map_Def, Yes, No, No, NoAlias, &iflags.wc_tiled_map,
           Term_False, (char *)0)
    NHOPTB(time, Status, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &flags.time, Term_False,
           "afficher les tours de jeu dans la ligne d'état")
#ifdef TIMED_DELAY
    NHOPTB(timed_delay, Map, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.nap, Term_False,
           "marquer une pause pour les effets visuels")
#else
    NHOPTB(timed_delay, Map, 0, opt_in, set_in_config,
           Off, No, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
    NHOPTB(tips, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.tips, Term_False,
           "afficher des astuces pendant le jeu")
    NHOPTB(tombstone, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.tombstone, Term_False,
           "afficher la pierre tombale à la mort du personnage")
    NHOPTB(toptenwin, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.toptenwin, Term_False,
           "afficher les meilleurs scores dans une fenêtre")
    NHOPTC(traps, Advanced, MAXTCHARS + 1, opt_in, set_in_config,
                No, Yes, No, No, NoAlias,
                "liste des symboles pour dessiner les pièges")
    NHOPTB(travel, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.travelcmd, Term_False,
           "activer le voyage par clic de souris")
#ifdef DEBUG
    NHOPTB(travel_debug, Advanced, 0, opt_in, set_wizonly,
           Off, Yes, No, No, NoAlias, &iflags.trav_debug, Term_False,
           (char *)0)
#else
    NHOPTB(travel_debug, Advanced, 0, opt_in, set_wizonly,
           Off, No, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
    NHOPTB(tutorial, Advanced, 0, opt_out, set_in_config,
           On, Yes, No, No, NoAlias, &flags.tutorial, Term_False,
           "proposer le tutoriel")
    NHOPTB(use_darkgray, Advanced, 0, opt_out, set_in_config,
           On, Yes, No, No, NoAlias, &iflags.wc2_darkgray, Term_False,
           "utiliser le noir gras au lieu du bleu")
    NHOPTB(use_inverse, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &iflags.wc_inverse, Term_False,
           "afficher en vidéo inverse les monstres détectés")
    NHOPTB(use_truecolor, Advanced, 0, opt_in, set_in_config,
                Off, Yes, No, No, "use_truecolour",
           &iflags.use_truecolor, Term_False,
           (char *)0)
    NHOPTC(vary_msgcount, Advanced, 20, opt_in, set_gameview,
                No, Yes, No, No, NoAlias, "afficher plus d'anciens messages à la fois")
    NHOPTB(verbose, Advanced, 0, opt_out, set_in_game,
           On, Yes, No, No, NoAlias, &flags.verbose, Term_False,
           (char *)0)
    NHOPTC(versinfo, Advanced, 80, opt_out, set_in_game,
           No, Yes, No, Yes, NoAlias, "informations supplémentaires pour 'showvers'")
#if defined(MSDOS) && defined(NO_TERMS)
    NHOPTC(video, Advanced, 20, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "méthode de mise à jour vidéo")
#endif
#ifdef VIDEOSHADES
    NHOPTC(videocolors, Advanced, 40, opt_in, set_gameview,
                No, Yes, No, No, "videocolours",
                "correspondance des couleurs pour les routines d'écran internes")
    NHOPTC(videoshades, Advanced, 32, opt_in, set_gameview,
                No, Yes, No, No, NoAlias,
                "nuances de gris pour noir/gris/blanc")
#endif
#ifdef MSDOS
    NHOPTC(video_width, Advanced, 10, opt_in, set_gameview,
                No, Yes, No, No, NoAlias, "largeur vidéo")
    NHOPTC(video_height, Advanced, 10, opt_in, set_gameview,
                No, Yes, No, No, NoAlias, "hauteur vidéo")
#endif
#ifdef SND_SPEECH
    NHOPTB(voices, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.voices, Term_Off,
           (char *)0)
#else
    NHOPTB(voices, Advanced, 0, opt_in, set_gameview,
           Off, Yes, No, No, NoAlias, &iflags.voices, Term_Excluded,
           (char *)0)
#endif
#ifdef TTY_TILES_ESCCODES
    NHOPTB(vt_tiledata, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, &iflags.vt_tiledata, Term_False,
           "émettre des codes d'échappement spéciaux")
#else
    NHOPTB(vt_tiledata, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
#ifdef TTY_SOUND_ESCCODES
    NHOPTB(vt_sounddata, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, &iflags.vt_sounddata, Term_False,
           "émettre les sons dans des codes d'échappement spéciaux")
#else
    NHOPTB(vt_sounddata, Advanced, 0, opt_in, set_in_config,
           Off, Yes, No, No, NoAlias, (boolean *) 0, Term_False,
           (char *)0)
#endif
    NHOPTC(warnings, Advanced, 10, opt_in, set_in_config,
                No, Yes, No, No, NoAlias, "caractères affichés pour les avertissements")
    NHOPTB(weaponstatus, Advanced, 0, opt_in, set_in_game,
                Off, Yes, No, No, NoAlias, &flags.weaponstatus, Term_False,
                "afficher l'arme en main dans un champ d'état")
    NHOPTC(whatis_coord, Advanced, 1, opt_in, set_in_game,
                Yes, Yes, No, Yes, NoAlias,
                "afficher les coordonnées en décrivant la position du curseur")
    NHOPTC(whatis_filter, Advanced, 1, opt_in, set_in_game,
                Yes, Yes, No, Yes, NoAlias,
                "filtrer les positions lors du ciblage suivant ou précédent")
    NHOPTB(whatis_menu, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.getloc_usemenu, Term_False,
           "afficher un menu pour choisir une position")
    NHOPTB(whatis_moveskip, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.getloc_moveskip, Term_False,
           "sauter les glyphes identiques pour choisir une position")
    NHOPTC(windowborders, Advanced, 9, opt_in, set_in_game,
                Yes, Yes, No, Yes, NoAlias, "0 (désactivé), 1 (activé), 2 (auto)")
#ifdef WINCHAIN
    NHOPTC(windowchain, Advanced, WINTYPELEN, opt_in, set_in_sysconf,
                No, Yes, No, No, NoAlias, "processeur de fenêtres à utiliser")
#endif
    NHOPTC(windowcolors, Advanced, 80, opt_in, set_gameview,
                No, Yes, Yes, No, NoAlias,
                "couleurs de premier plan/de fond des fenêtres")
 /* NHOPTC(windowtype) -- moved to top */
    NHOPTB(wizmgender, Advanced, 0, opt_in, set_wizonly,
           Off, Yes, No, No, NoAlias, &iflags.wizmgender, Term_False,
           (char *)0)
    NHOPTB(wizweight, Advanced, 0, opt_in, set_wizonly,
           Off, Yes, No, No, NoAlias, &iflags.wizweight, Term_False,
           (char *)0)
    NHOPTB(wraptext, Advanced, 0, opt_in, set_in_game,
           Off, Yes, No, No, NoAlias, &iflags.wc2_wraptext, Term_False,
           (char *)0)

    /*
     * Prefix-based Options
     */

    NHOPTP(cond_, Advanced, 0, opt_in, set_hidden,
                Yes, No, Yes, Yes, NoAlias, "préfixe des options cond_")
    NHOPTP(font, Advanced, 0, opt_in, set_hidden,
                Yes, Yes, Yes, No, NoAlias, "préfixe des options de police")
#if defined(MICRO) && !defined(AMIGA)
    /* included for compatibility with old NetHack.cnf files */
    NHOPTP(IBM_, Advanced, 0, opt_in, set_hidden,
                No, No, Yes, No, NoAlias, "préfixe des anciennes options IBM_ micro")
#endif /* MICRO */

#undef NoAlias
#undef NHOPTB
#undef NHOPTC
#undef NHOPTP
#undef NHOPTO

/* *INDENT-ON* */
/* clang-format on */
#endif /* NHOPT_PROTO || NHOPT_ENUM || NHOPT_PARSE */

/*optlist.h*/
