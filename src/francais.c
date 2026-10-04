/* NetHack 5.0  francais.c  -- couche grammaticale de la version francaise */
/* NetHack may be freely redistributed.  See license for details. */

/*
 * Genre, nombre, articles, contractions, accords et conjugaison pour la
 * traduction francaise.  Voir include/francais.h et doc/TRADUCTION.md.
 *
 * Toutes les chaines sont en UTF-8.  Les comparaisons de mots se font
 * octet par octet apres passage en minuscules (ASCII et lettres latines
 * accentuees de la page U+00C0..U+00DE).
 */

#include "hack.h"

#define FRBUFS 16
static char frbufs[FRBUFS][BUFSZ];
static int frbufidx = 0;

staticfn char *frbuf(void);
staticfn void fr_lower(char *);
staticfn const char *skip_determinant(const char *, int *);
staticfn const char *skip_preadj(const char *);
staticfn void head_word(const char *, char *, size_t);
staticfn int dict_lookup(const char *);
staticfn int dict_cmp(const void *, const void *);
staticfn int genre_par_suffixe(const char *);
staticfn boolean est_nom_propre(const char *);
staticfn boolean a_determinant(const char *);
staticfn boolean finit_par(const char *, const char *);
staticfn void pluriel_mot(char *, size_t, boolean);
staticfn boolean mot_stop(const char *);
staticfn void singulier_mot(char *);

staticfn char *
frbuf(void)
{
    frbufidx = (frbufidx + 1) % FRBUFS;
    frbufs[frbufidx][0] = '\0';
    return frbufs[frbufidx];
}

/* minuscules ASCII + Latin-1 encode en UTF-8 (C3 80..9E -> C3 A0..BE) */
staticfn void
fr_lower(char *s)
{
    unsigned char *p = (unsigned char *) s;

    for (; *p; p++) {
        if (*p >= 'A' && *p <= 'Z') {
            *p = (unsigned char) (*p + ('a' - 'A'));
        } else if (*p == 0xC3 && p[1] >= 0x80 && p[1] <= 0x9E
                   && p[1] != 0x97) {
            p[1] = (unsigned char) (p[1] + 0x20);
            p++;
        } else if (*p == 0xC5 && p[1] == 0x92) { /* Œ -> œ */
            p[1] = 0x93;
            p++;
        }
    }
}

staticfn boolean
finit_par(const char *s, const char *suf)
{
    size_t ls = strlen(s), lf = strlen(suf);

    return (ls >= lf && !strcmp(s + ls - lf, suf));
}

/*
 * Dictionnaire des genres.  Trie par ordre d'octets (strcmp) ; il est
 * complete par les listes produites lors de la traduction des noms
 * d'objets et de monstres.  'm' = masculin, 'f' = feminin.
 */
struct fr_nom {
    const char *mot;
    char genre;
};

static const struct fr_nom fr_dico[] = {
#include "fr_genres.h"
};

staticfn int
dict_cmp(const void *a, const void *b)
{
    return strcmp(((const struct fr_nom *) a)->mot,
                  ((const struct fr_nom *) b)->mot);
}

/* renvoie FR_MASC, FR_FEM, ou -1 si le mot est inconnu */
staticfn int
dict_lookup(const char *mot)
{
    struct fr_nom key, *res;

    key.mot = mot;
    key.genre = 0;
    res = (struct fr_nom *) bsearch((genericptr_t) &key,
                                    (genericptr_t) fr_dico, SIZE(fr_dico),
                                    sizeof fr_dico[0], dict_cmp);
    if (!res)
        return -1;
    return (res->genre == 'f') ? FR_FEM : FR_MASC;
}

/* deviner le genre d'un mot inconnu d'apres sa terminaison */
staticfn int
genre_par_suffixe(const char *w)
{
    static const char *const fem[] = {
        "tion", "sion", "xion", "ette", "elle", "ure", "ance", "ence",
        "ade", "ise", "ière", "esse", "euse", "trice", "ine", "onne",
        "ée", "té", "ie", "aille", "eille", "ouille", "erie", "ite",
        "ude", "ève", "ole", "ule", "aine", "eine", "ase", "ose", "use",
        "ine", "ille", "ière", "oire", "ache", "anche", "otte", "ette",
        "ronne", "ière", "ée", (const char *) 0
    };
    static const char *const masc_excep[] = {
        "musée", "lycée", "trophée", "pygmée", "athlée", "scarabée",
        "comité", "été", "côté", "pâté", "traité", "dé", "thé", "pied",
        "génie", "incendie", "parapluie", "foie", "zombie", "lutrin",
        "grimoire", "ivoire", "laboratoire", "pourboire", "territoire",
        "accessoire", "rasoir", "squelette", "cimeterre", "magazine",
        "termite", "satellite", "ermite", "rite", "site", "gîte",
        "vampire", "empire", "sabre", "oracle", "obstacle", "spectacle",
        "domaine", "capitaine", "chêne", "frêne", "moine", "platine",
        "patrimoine", "golem", "dragon", "démon", (const char *) 0
    };
    int i;

    for (i = 0; masc_excep[i]; i++)
        if (!strcmp(w, masc_excep[i]))
            return FR_MASC;
    for (i = 0; fem[i]; i++)
        if (finit_par(w, fem[i]))
            return FR_FEM;
    return FR_MASC;
}

/* determinants reconnus en tete de groupe nominal :
   *pl recoit 1 si le determinant est pluriel, 0 s'il est singulier,
   2 s'il force le feminin singulier, 3 s'il force le masculin singulier */
static const struct {
    const char *det;
    int nombre; /* 0 sing, 1 plur */
    int genre;  /* -1 inconnu, FR_MASC, FR_FEM */
} fr_dets[] = {
    { "les ", 1, -1 }, { "des ", 1, -1 }, { "vos ", 1, -1 },
    { "mes ", 1, -1 }, { "ses ", 1, -1 }, { "ces ", 1, -1 },
    { "tes ", 1, -1 }, { "nos ", 1, -1 }, { "leurs ", 1, -1 },
    { "quelques ", 1, -1 }, { "plusieurs ", 1, -1 }, { "aux ", 1, -1 },
    { "de la ", 0, FR_FEM }, { "de l'", 0, -1 },
    { "le ", 0, FR_MASC }, { "la ", 0, FR_FEM }, { "l'", 0, -1 },
    { "un ", 0, FR_MASC }, { "une ", 0, FR_FEM }, { "du ", 0, FR_MASC },
    { "au ", 0, FR_MASC }, { "à la ", 0, FR_FEM }, { "à l'", 0, -1 },
    { "ce ", 0, FR_MASC }, { "cet ", 0, FR_MASC },
    { "cette ", 0, FR_FEM }, { "mon ", 0, -1 }, { "ma ", 0, FR_FEM },
    { "ton ", 0, -1 }, { "ta ", 0, FR_FEM }, { "son ", 0, -1 },
    { "sa ", 0, FR_FEM }, { "votre ", 0, -1 }, { "notre ", 0, -1 },
    { "leur ", 0, -1 }, { "aucun ", 0, FR_MASC },
    { "aucune ", 0, FR_FEM }, { "chaque ", 0, -1 },
    { "quelque ", 0, -1 }, { "un peu de ", 0, -1 },
    { "un peu d'", 0, -1 }, { "d'", 0, -1 }, { "de ", 0, -1 },
};

/* saute le determinant initial ; *info recoit nombre*4 + (genre+1) ou -1 */
staticfn const char *
skip_determinant(const char *s, int *info)
{
    int i;
    char low[BUFSZ];

    *info = -1;
    (void) strncpy(low, s, sizeof low - 1);
    low[sizeof low - 1] = '\0';
    fr_lower(low);
    /* nombre en chiffres */
    if (digit(*low)) {
        const char *p = s;
        long n = atol(s);

        while (digit(*p))
            p++;
        while (*p == ' ')
            p++;
        *info = ((n != 1L) ? 1 : 0) * 4 + 0;
        return p;
    }
    for (i = 0; i < SIZE(fr_dets); i++) {
        size_t l = strlen(fr_dets[i].det);

        if (!strncmp(low, fr_dets[i].det, l)) {
            *info = fr_dets[i].nombre * 4 + (fr_dets[i].genre + 1);
            return s + l;
        }
    }
    return s;
}

static const char *const fr_preadj[] = {
    "petit", "petite", "petits", "petites", "grand", "grande", "grands",
    "grandes", "gros", "grosse", "grosses", "vieux", "vieil", "vieille",
    "vieilles", "jeune", "jeunes", "bon", "bonne", "bons", "bonnes",
    "mauvais", "mauvaise", "mauvaises", "beau", "bel", "belle", "beaux",
    "belles", "nouveau", "nouvel", "nouvelle", "nouveaux", "nouvelles",
    "vrai", "vraie", "faux", "fausse", "haut", "haute", "demi", "mini",
    "très", "autre", "autres", "premier", "première", "dernier",
    "dernière", "simple", "pauvre", "étrange", "étranges", "énorme",
    "énormes", "minuscule", "minuscules", "immense", "immenses",
    "prochain", "prochaine", "seul", "seule", "même", "double", "triple",
    "saint", "sainte", "maître", "faible", "puissant", "puissante",
    "lointain", "lointaine", "certain", "certaine", "certains",
    "certaines", "drôle", "sacré", "sacrée", "profane", "piètre",
    "véritable", "brave", (const char *) 0
};

staticfn const char *
skip_preadj(const char *s)
{
    char w[BUFSZ];
    const char *p;
    int i, guard = 0;

    while (guard++ < 4) {
        size_t l;

        p = s;
        while (*p && *p != ' ')
            p++;
        if (!*p)
            return s; /* dernier mot : c'est le nom */
        l = (size_t) (p - s);
        if (l >= sizeof w)
            return s;
        (void) strncpy(w, s, l);
        w[l] = '\0';
        fr_lower(w);
        for (i = 0; fr_preadj[i]; i++)
            if (!strcmp(w, fr_preadj[i]))
                break;
        if (!fr_preadj[i])
            return s;
        s = p + 1;
    }
    return s;
}

/* extrait le nom principal (minuscules) d'un groupe nominal */
staticfn void
head_word(const char *s, char *out, size_t outsz)
{
    int info;
    size_t i = 0;

    s = skip_determinant(s, &info);
    s = skip_preadj(s);
    while (*s && *s != ' ' && *s != ',' && *s != '(' && *s != '\''
           && i < outsz - 1)
        out[i++] = *s++;
    out[i] = '\0';
    fr_lower(out);
}

int
fr_genre(const char *s)
{
    char w[BUFSZ];
    int info, g;
    const char *p;

    if (!s || !*s)
        return FR_MASC;
    p = skip_determinant(s, &info);
    if (info >= 0 && (info % 4) != 0)
        return (info % 4) - 1; /* le/la/une/cette... */
    head_word(s, w, sizeof w);
    if (!*w)
        return FR_MASC;
    if ((g = dict_lookup(w)) >= 0)
        return g;
    /* forme plurielle ? */
    {
        char sg[BUFSZ];

        Strcpy(sg, w);
        singulier_mot(sg);
        if (strcmp(sg, w) && (g = dict_lookup(sg)) >= 0)
            return g;
        /* mot compose "chauve-souris" : essayer la derniere partie */
        if ((p = strrchr(w, '-')) != 0 && p[1]) {
            if ((g = dict_lookup(p + 1)) >= 0)
                return g;
        }
        return genre_par_suffixe(sg);
    }
}

boolean
fr_pluriel(const char *s)
{
    char w[BUFSZ], sg[BUFSZ];
    int info;

    if (!s || !*s)
        return FALSE;
    (void) skip_determinant(s, &info);
    if (info >= 0)
        return (info / 4) == 1;
    head_word(s, w, sizeof w);
    if (!*w)
        return FALSE;
    if (dict_lookup(w) >= 0)
        return FALSE;
    Strcpy(sg, w);
    singulier_mot(sg);
    if (!strcmp(sg, w))
        return FALSE;
    if (dict_lookup(sg) >= 0)
        return TRUE;
    /* mot inconnu termine par s/x : probablement pluriel */
    return (finit_par(w, "s") || finit_par(w, "x"))
           && strlen(w) > 3 && !finit_par(w, "ss") && !finit_par(w, "us")
           && !finit_par(w, "is") && !finit_par(w, "os")
           && !finit_par(w, "ux") && !finit_par(w, "ix");
}

/* h aspire : pas d'elision */
static const char *const h_aspire[] = {
    "hach", "haie", "haill", "hain", "hair", "hal", "hamac", "hameau",
    "hamp", "hamst", "hanche", "hand", "hang", "hant", "happ", "harc",
    "hard", "hareng", "harf", "hargn", "haricot", "harn", "harp",
    "hasard", "hât", "hauss", "haut", "havr", "heaum", "hennir", "hérau",
    "hériss", "hern", "héron", "héros", "hêtre", "heurt", "hibou",
    "hideu", "hiér", "hobbit", "hochet", "hockey", "hollandais",
    "homard", "hongr", "honte", "hoquet", "horde", "hors", "hotte",
    "houb", "houe", "houl", "houp", "housse", "houx", "hublot", "huer",
    "huit", "hulotte", "humer", "hurl", "hussard", "hutte", "hyène",
    (const char *) 0
};

boolean
fr_elision(const char *s)
{
    unsigned char c0, c1;
    char low[16];
    int i;

    if (!s || !*s)
        return FALSE;
    (void) strncpy(low, s, sizeof low - 1);
    low[sizeof low - 1] = '\0';
    fr_lower(low);
    c0 = (unsigned char) low[0];
    c1 = (unsigned char) low[1];
    if (strchr("aeiou", (char) c0))
        return TRUE;
    if (c0 == 0xC3) {
        /* à â ä æ è é ê ë î ï ô ö ù û ü */
        switch (c1) {
        case 0xA0: case 0xA2: case 0xA4: case 0xA6: case 0xA8: case 0xA9:
        case 0xAA: case 0xAB: case 0xAE: case 0xAF: case 0xB4: case 0xB6:
        case 0xB9: case 0xBB: case 0xBC:
            return TRUE;
        default:
            return FALSE;
        }
    }
    if (c0 == 0xC5 && c1 == 0x93) /* œ */
        return TRUE;
    if (c0 == 'h') {
        for (i = 0; h_aspire[i]; i++)
            if (!strncmp(low, h_aspire[i], strlen(h_aspire[i])))
                return FALSE;
        return TRUE;
    }
    if (c0 == 'y') /* ypérite, yttrium... ; mais "le yéti", "le yak" */
        return (c1 == 't' || c1 == 'p');
    return FALSE;
}

/* commence par une majuscule (nom propre : Excalibur, Méduse, Fido) */
staticfn boolean
est_nom_propre(const char *s)
{
    const unsigned char *u = (const unsigned char *) s;

    if (*u >= 'A' && *u <= 'Z')
        return TRUE;
    if (*u == 0xC3 && u[1] >= 0x80 && u[1] <= 0x9E)
        return TRUE;
    if (*u == 0xC5 && u[1] == 0x92) /* Œ */
        return TRUE;
    return FALSE;
}

/* la chaine commence-t-elle deja par un determinant ou un nombre ? */
staticfn boolean
a_determinant(const char *s)
{
    int info;

    (void) skip_determinant(s, &info);
    return info >= 0;
}

/* passe "Le "/"La "/"L'"/"Les " initial en minuscule */
staticfn void
baisse_article(char *s)
{
    if (!strncmp(s, "Le ", 3) || !strncmp(s, "La ", 3)
        || !strncmp(s, "L'", 2) || !strncmp(s, "Les ", 4)
        || !strncmp(s, "Un ", 3) || !strncmp(s, "Une ", 4)
        || !strncmp(s, "Des ", 4))
        *s = lowc(*s);
}

/*
 * the() et an() remplacent les versions anglaises de objnam.c.
 */
char *
the(const char *str)
{
    char *buf = frbuf();

    if (!str || !*str)
        return buf;
    if (!strncmp(str, "Le ", 3) || !strncmp(str, "La ", 3)
        || !strncmp(str, "L'", 2) || !strncmp(str, "Les ", 4)) {
        Strcpy(buf, str);
        baisse_article(buf);
        return buf;
    }
    if (a_determinant(str) || est_nom_propre(str)) {
        Strcpy(buf, str);
        return buf;
    }
    if (fr_pluriel(str))
        Strcpy(buf, "les ");
    else if (fr_elision(str))
        Strcpy(buf, "l'");
    else
        Strcpy(buf, (fr_genre(str) == FR_FEM) ? "la " : "le ");
    (void) strncat(buf, str, BUFSZ - strlen(buf) - 1);
    return buf;
}

char *
The(const char *str)
{
    return fr_upstart(the(str));
}

char *
an(const char *str)
{
    char *buf = frbuf();

    if (!str || !*str)
        return buf;
    if (a_determinant(str) || est_nom_propre(str)) {
        Strcpy(buf, str);
        baisse_article(buf);
        return buf;
    }
    if (fr_pluriel(str))
        Strcpy(buf, "des ");
    else
        Strcpy(buf, (fr_genre(str) == FR_FEM) ? "une " : "un ");
    (void) strncat(buf, str, BUFSZ - strlen(buf) - 1);
    return buf;
}

char *
An(const char *str)
{
    return fr_upstart(an(str));
}

/* "de" + groupe nominal, avec contraction */
char *
du(const char *str)
{
    char *buf = frbuf();
    char low[8];

    if (!str || !*str)
        return buf;
    (void) strncpy(low, str, sizeof low - 1);
    low[sizeof low - 1] = '\0';
    fr_lower(low);
    if (!strncmp(low, "le ", 3)) {
        Sprintf(buf, "du %.*s", BUFSZ - 4, str + 3);
    } else if (!strncmp(low, "les ", 4)) {
        Sprintf(buf, "des %.*s", BUFSZ - 5, str + 4);
    } else if (!strncmp(low, "la ", 3) || !strncmp(low, "l'", 2)) {
        Sprintf(buf, "de %.*s", BUFSZ - 4, str);
        baisse_article(buf + 3);
    } else if (!strncmp(low, "des ", 4)) {
        Sprintf(buf, "de %.*s", BUFSZ - 4, str + 4);
    } else if (a_determinant(str) || est_nom_propre(str)) {
        return de(str);
    } else {
        return du(the(str));
    }
    return buf;
}

char *
Du(const char *str)
{
    return fr_upstart(du(str));
}

/* "à" + groupe nominal, avec contraction */
char *
au(const char *str)
{
    char *buf = frbuf();
    char low[8];

    if (!str || !*str)
        return buf;
    (void) strncpy(low, str, sizeof low - 1);
    low[sizeof low - 1] = '\0';
    fr_lower(low);
    if (!strncmp(low, "le ", 3)) {
        Sprintf(buf, "au %.*s", BUFSZ - 4, str + 3);
    } else if (!strncmp(low, "les ", 4)) {
        Sprintf(buf, "aux %.*s", BUFSZ - 5, str + 4);
    } else if (a_determinant(str) || est_nom_propre(str)) {
        Sprintf(buf, "à %.*s", BUFSZ - 4, str);
        baisse_article(buf + 3);
    } else {
        return au(the(str));
    }
    return buf;
}

char *
Au(const char *str)
{
    return fr_upstart(au(str));
}

/* "de X" ou "d'X" ; "de le X" -> "du X" */
char *
de(const char *str)
{
    char *buf = frbuf();
    char low[8];

    if (!str)
        return buf;
    (void) strncpy(low, str, sizeof low - 1);
    low[sizeof low - 1] = '\0';
    fr_lower(low);
    if (!strncmp(low, "le ", 3) || !strncmp(low, "les ", 4))
        return du(str);
    if (fr_elision(str))
        Sprintf(buf, "d'%.*s", BUFSZ - 3, str);
    else
        Sprintf(buf, "de %.*s", BUFSZ - 4, str);
    return buf;
}

const char *
accord(const char *str)
{
    boolean pl = fr_pluriel(str);
    int g = fr_genre(str);

    if (g == FR_FEM)
        return pl ? "es" : "e";
    return pl ? "s" : "";
}

/* adjectifs a feminin irregulier */
static const struct {
    const char *m, *f;
} adj_irreg[] = {
    { "beau", "belle" }, { "bel", "belle" }, { "vieux", "vieille" },
    { "vieil", "vieille" }, { "nouveau", "nouvelle" },
    { "nouvel", "nouvelle" }, { "fou", "folle" }, { "mou", "molle" },
    { "blanc", "blanche" }, { "franc", "franche" }, { "sec", "sèche" },
    { "frais", "fraîche" }, { "long", "longue" }, { "doux", "douce" },
    { "faux", "fausse" }, { "roux", "rousse" }, { "gros", "grosse" },
    { "bas", "basse" }, { "gras", "grasse" }, { "épais", "épaisse" },
    { "las", "lasse" }, { "gentil", "gentille" },
    { "favori", "favorite" }, { "malin", "maligne" },
    { "bénin", "bénigne" }, { "public", "publique" },
    { "grec", "grecque" }, { "turc", "turque" }, { "aigu", "aiguë" },
    { "ambigu", "ambiguë" }, { "jaloux", "jalouse" },
    { "meilleur", "meilleure" }, { "supérieur", "supérieure" },
    { "inférieur", "inférieure" }, { "intérieur", "intérieure" },
    { "extérieur", "extérieure" }, { "majeur", "majeure" },
    { "mineur", "mineure" }, { "complet", "complète" },
    { "incomplet", "incomplète" }, { "discret", "discrète" },
    { "inquiet", "inquiète" }, { "secret", "secrète" },
    { "concret", "concrète" }, { "protecteur", "protectrice" },
    { "destructeur", "destructrice" }, { "créateur", "créatrice" },
    { "révélateur", "révélatrice" }, { "libérateur", "libératrice" },
    { "conducteur", "conductrice" }, { "séducteur", "séductrice" },
    { "enchanteur", "enchanteresse" }, { "vengeur", "vengeresse" },
    { "pécheur", "pécheresse" }, { "traître", "traîtresse" },
    { "hébreu", "hébraïque" }, { "andalou", "andalouse" },
    { "chic", "chic" }, { "marron", "marron" }, { "orange", "orange" },
    { "kaki", "kaki" }, { "or", "or" }, { "argent", "argent" },
    { "turquoise", "turquoise" }, { "émeraude", "émeraude" },
    { "rubis", "rubis" }, { "pourpre", "pourpre" },
};

/* mots invariables qui ne s'accordent pas dans un groupe adjectival */
static const char *const adj_invar[] = {
    "non", "très", "peu", "plus", "moins", "bien", "mal", "trop",
    "assez", "si", "tout", "presque", "à", "demi", "mi", "semi",
    "clair", "foncé", "pâle", "vif", (const char *) 0
};

staticfn void
fem_mot(char *w, size_t sz)
{
    int i;
    size_t l = strlen(w);

    for (i = 0; i < SIZE(adj_irreg); i++)
        if (!strcmp(w, adj_irreg[i].m)) {
            (void) strncpy(w, adj_irreg[i].f, sz - 1);
            w[sz - 1] = '\0';
            return;
        }
    if (l + 3 >= sz || l == 0)
        return;
    if (w[l - 1] == 'e')
        return;
    if (finit_par(w, "eux")) {
        Strcpy(w + l - 1, "se");
    } else if (finit_par(w, "eur")) {
        Strcpy(w + l - 1, "se");          /* trompeur -> trompeuse */
    } else if (finit_par(w, "if")) {
        Strcpy(w + l - 1, "ve");
    } else if (finit_par(w, "f")) {
        Strcpy(w + l - 1, "ve");          /* neuf -> neuve */
    } else if (finit_par(w, "el") || finit_par(w, "eil")
               || finit_par(w, "en") || finit_par(w, "on")
               || finit_par(w, "et") || finit_par(w, "ul")) {
        w[l] = w[l - 1];
        Strcpy(w + l + 1, "e");           /* cruel -> cruelle, muet -> muette */
    } else if (finit_par(w, "er")) {
        Strcpy(w + l - 2, "ère");         /* léger -> légère */
    } else if (finit_par(w, "eau")) {
        Strcpy(w + l - 3, "elle");
    } else if (finit_par(w, "x")) {
        Strcpy(w + l - 1, "se");
    } else if (finit_par(w, "c")) {
        Strcpy(w + l - 1, "que");
    } else if (finit_par(w, "gu")) {
        Strcpy(w + l, "ë");
    } else {
        Strcpy(w + l, "e");
    }
}

staticfn void
plur_adj_mot(char *w, size_t sz)
{
    size_t l = strlen(w);

    if (!l || l + 2 >= sz)
        return;
    if (w[l - 1] == 's' || w[l - 1] == 'x')
        return;
    if (finit_par(w, "eau")) {
        Strcat(w, "x");
    } else if (finit_par(w, "al") && strcmp(w, "fatal") && strcmp(w, "naval")
               && strcmp(w, "final") && strcmp(w, "banal")
               && strcmp(w, "natal")) {
        Strcpy(w + l - 2, "aux");
    } else {
        Strcat(w, "s");
    }
}

staticfn boolean
adj_invariable(const char *w)
{
    int i;

    for (i = 0; adj_invar[i]; i++)
        if (!strcmp(w, adj_invar[i]))
            return TRUE;
    for (i = 0; i < SIZE(adj_irreg); i++)
        if (!strcmp(w, adj_irreg[i].m) && !strcmp(adj_irreg[i].m,
                                                  adj_irreg[i].f))
            return TRUE;
    return FALSE;
}

char *
fr_adj(const char *adj, int genre, boolean pluriel)
{
    char *buf = frbuf();
    char w[BUFSZ];
    const char *p = adj;
    boolean stop = FALSE;

    if (!adj)
        return buf;
    while (*p) {
        size_t l = 0;
        const char *q = p;
        char sep;

        while (*q && *q != ' ' && *q != '-')
            q++;
        l = (size_t) (q - p);
        if (l >= sizeof w)
            l = sizeof w - 1;
        (void) strncpy(w, p, l);
        w[l] = '\0';
        sep = *q;
        if (!stop && (mot_stop(w) || !strncmp(w, "d'", 2)
                      || !strncmp(w, "l'", 2)))
            stop = TRUE;
        if (!stop && !adj_invariable(w) && *w) {
            if (genre == FR_FEM)
                fem_mot(w, sizeof w);
            if (pluriel)
                plur_adj_mot(w, sizeof w);
        }
        if (strlen(buf) + strlen(w) + 2 < BUFSZ) {
            Strcat(buf, w);
            if (sep) {
                size_t bl = strlen(buf);

                buf[bl] = sep;
                buf[bl + 1] = '\0';
            }
        }
        p = sep ? q + 1 : q;
    }
    return buf;
}

char *
fr_adj_accord(const char *adj, const char *nom)
{
    return fr_adj(adj, fr_genre(nom), fr_pluriel(nom));
}

/*
 * Pluriel des noms et groupes nominaux.
 */

/* mots qui arretent la pluralisation : prepositions et complements */
staticfn boolean
mot_stop(const char *w)
{
    static const char *const stops[] = {
        "de", "du", "des", "à", "au", "aux", "en", "pour", "sans", "avec",
        "sur", "sous", "contre", "dans", "par", "nommé", "nommée",
        "nommés", "nommées", "appelé", "appelée", "appelés", "appelées",
        "étiqueté", "étiquetée", "étiquetés", "étiquetées", "contenant",
        "(", "et", "ou", "qui", "que", "d'", "chez", "vers", "entre",
        (const char *) 0
    };
    int i;

    if (*w == '(' || *w == '"' || digit(*w))
        return TRUE;
    for (i = 0; stops[i]; i++)
        if (!strcmp(w, stops[i]))
            return TRUE;
    return FALSE;
}

static const struct {
    const char *sg, *pl;
} noms_irreg[] = {
    { "œil", "yeux" }, { "ciel", "cieux" }, { "aïeul", "aïeux" },
    { "travail", "travaux" }, { "corail", "coraux" }, { "émail", "émaux" },
    { "vitrail", "vitraux" }, { "bail", "baux" }, { "soupirail",
                                                     "soupiraux" },
    { "bijou", "bijoux" }, { "caillou", "cailloux" }, { "chou", "choux" },
    { "genou", "genoux" }, { "hibou", "hiboux" }, { "joujou", "joujoux" },
    { "pou", "poux" }, { "bal", "bals" }, { "carnaval", "carnavals" },
    { "chacal", "chacals" }, { "festival", "festivals" },
    { "récital", "récitals" }, { "régal", "régals" }, { "landau",
                                                          "landaus" },
    { "bleu", "bleus" }, { "pneu", "pneus" }, { "émeu", "émeus" },
    { "monsieur", "messieurs" }, { "madame", "mesdames" },
    { "mademoiselle", "mesdemoiselles" }, { "gentilhomme",
                                            "gentilshommes" },
    { "bonhomme", "bonshommes" },
};

/* premiers elements invariables de mots composes (verbes, adverbes) */
static const char *const compose_invar[] = {
    "porte", "tire", "brise", "casse", "ouvre", "lance", "chasse", "coupe",
    "passe", "pare", "lave", "cache", "garde", "attrape", "tue", "mange",
    "presse", "croque", "gratte", "perce", "sous", "sur", "avant",
    "arrière", "hors", "après", "contre", "demi", "mi", "semi", "non",
    "vice", "ex", "anti", "pseudo", "quasi", "trompe", "crève", "rabat",
    "souffre", "pique", "boute", "taille", "vide", "grille", "réveille",
    (const char *) 0
};

/* pluralise un mot simple (nom ou adjectif) en place */
staticfn void
pluriel_simple(char *w, size_t sz)
{
    size_t l = strlen(w);
    int i;

    if (!l)
        return;
    for (i = 0; i < SIZE(noms_irreg); i++)
        if (!strcmp(w, noms_irreg[i].sg)) {
            (void) strncpy(w, noms_irreg[i].pl, sz - 1);
            w[sz - 1] = '\0';
            return;
        }
    if (l + 3 >= sz)
        return;
    /* mots tout en majuscules (labels de parchemins) : invariables */
    if (l > 1 && w[0] >= 'A' && w[0] <= 'Z' && w[1] >= 'A' && w[1] <= 'Z')
        return;
    if (w[l - 1] == 's' || w[l - 1] == 'x' || w[l - 1] == 'z')
        return;
    if (finit_par(w, "eau") || finit_par(w, "au") || finit_par(w, "eu")) {
        Strcat(w, "x");
    } else if (finit_par(w, "al")) {
        Strcpy(w + l - 2, "aux");
    } else {
        Strcat(w, "s");
    }
}

/* pluralise un mot, eventuellement compose avec des traits d'union */
staticfn void
pluriel_mot(char *w, size_t sz, boolean premier)
{
    char *h = strchr(w, '-');

    nhUse(premier);
    if (h) {
        char a[BUFSZ], b[BUFSZ];
        int i;
        boolean invar = FALSE;

        *h = '\0';
        Strcpy(a, w);
        Strcpy(b, h + 1);
        for (i = 0; compose_invar[i]; i++)
            if (!strcmp(a, compose_invar[i]))
                invar = TRUE;
        if (!invar)
            pluriel_simple(a, sizeof a);
        /* "-de-", "-à-" : seul le premier element varie */
        if (strncmp(b, "de-", 3) && strncmp(b, "à-", 3)
            && strncmp(b, "en-", 3) && strncmp(b, "d'", 2))
            pluriel_simple(b, sizeof b);
        Snprintf(w, sz, "%s-%s", a, b);
        return;
    }
    pluriel_simple(w, sz);
}

char *
makeplural(const char *oldstr)
{
    char *buf = frbuf();
    char w[BUFSZ];
    const char *p = oldstr;
    boolean stop = FALSE, premier = TRUE;
    int info;

    if (!oldstr || !*oldstr)
        return buf;
    /* determinant : le -> les, un -> des, etc. */
    {
        const char *rest = skip_determinant(oldstr, &info);

        if (info >= 0 && rest != oldstr) {
            char low[16];

            (void) strncpy(low, oldstr, sizeof low - 1);
            low[sizeof low - 1] = '\0';
            fr_lower(low);
            if (!strncmp(low, "le ", 3) || !strncmp(low, "la ", 3)
                || !strncmp(low, "l'", 2))
                Strcpy(buf, "les ");
            else if (!strncmp(low, "un ", 3) || !strncmp(low, "une ", 4))
                Strcpy(buf, "des ");
            else if (!strncmp(low, "votre ", 6))
                Strcpy(buf, "vos ");
            else if (!strncmp(low, "son ", 4) || !strncmp(low, "sa ", 3))
                Strcpy(buf, "ses ");
            else if (!strncmp(low, "mon ", 4) || !strncmp(low, "ma ", 3))
                Strcpy(buf, "mes ");
            else if (!strncmp(low, "ce ", 3) || !strncmp(low, "cet ", 4)
                     || !strncmp(low, "cette ", 6))
                Strcpy(buf, "ces ");
            else if (!strncmp(low, "leur ", 5))
                Strcpy(buf, "leurs ");
            else
                (void) strncat(buf, oldstr, (size_t) (rest - oldstr));
            if (*oldstr >= 'A' && *oldstr <= 'Z')
                *buf = highc(*buf);
            p = rest;
        }
    }
    while (*p) {
        const char *q = p;
        size_t l;
        char sep;

        while (*q && *q != ' ')
            q++;
        l = (size_t) (q - p);
        if (l >= sizeof w)
            l = sizeof w - 1;
        (void) strncpy(w, p, l);
        w[l] = '\0';
        sep = *q;
        if (!stop && (mot_stop(w) || !strncmp(w, "d'", 2)))
            stop = TRUE;
        if (!stop) {
            pluriel_mot(w, sizeof w, premier);
            premier = FALSE;
        }
        if (strlen(buf) + strlen(w) + 2 < BUFSZ) {
            Strcat(buf, w);
            if (sep)
                Strcat(buf, " ");
        }
        p = sep ? q + 1 : q;
    }
    return buf;
}

/* singulier d'un mot simple, en place (heuristique) */
staticfn void
singulier_mot(char *w)
{
    size_t l = strlen(w);
    int i;

    for (i = 0; i < SIZE(noms_irreg); i++)
        if (!strcmp(w, noms_irreg[i].pl)) {
            Strcpy(w, noms_irreg[i].sg);
            return;
        }
    if (l < 3 || dict_lookup(w) >= 0)
        return;
    if (finit_par(w, "eaux") || finit_par(w, "eux")
        || finit_par(w, "aux")) {
        char tmp[BUFSZ];

        Strcpy(tmp, w);
        tmp[l - 1] = '\0'; /* -eaux -> -eau, -eux -> -eu */
        if (finit_par(w, "aux") && !finit_par(w, "eaux")) {
            char al[BUFSZ];

            Strcpy(al, w);
            Strcpy(al + l - 3, "al");
            if (dict_lookup(al) >= 0 || dict_lookup(tmp) < 0) {
                Strcpy(w, al);
                return;
            }
        }
        Strcpy(w, tmp);
        return;
    }
    if (w[l - 1] == 's' || w[l - 1] == 'x') {
        char tmp[BUFSZ];

        Strcpy(tmp, w);
        tmp[l - 1] = '\0';
        if (dict_lookup(tmp) >= 0 || w[l - 1] == 's')
            Strcpy(w, tmp);
    }
}

char *
makesingular(const char *oldstr)
{
    char *buf = frbuf();
    char w[BUFSZ];
    const char *p = oldstr;
    boolean stop = FALSE;
    int info;

    if (!oldstr || !*oldstr)
        return buf;
    {
        const char *rest = skip_determinant(oldstr, &info);

        if (info >= 0 && rest != oldstr && (info / 4) == 1) {
            char low[16];

            (void) strncpy(low, oldstr, sizeof low - 1);
            low[sizeof low - 1] = '\0';
            fr_lower(low);
            p = rest;
            if (!strncmp(low, "les ", 4))
                Strcpy(buf, fr_genre(rest) == FR_FEM ? "la " : "le ");
            else if (!strncmp(low, "des ", 4))
                Strcpy(buf, fr_genre(rest) == FR_FEM ? "une " : "un ");
            else if (!strncmp(low, "vos ", 4))
                Strcpy(buf, "votre ");
            else if (digit(*oldstr))
                ; /* "3 flèches" -> "flèche" */
            else
                p = oldstr;
        }
    }
    while (*p) {
        const char *q = p;
        size_t l;
        char sep;

        while (*q && *q != ' ' && *q != '-')
            q++;
        l = (size_t) (q - p);
        if (l >= sizeof w)
            l = sizeof w - 1;
        (void) strncpy(w, p, l);
        w[l] = '\0';
        sep = *q;
        if (!stop && (mot_stop(w) || !strncmp(w, "d'", 2)))
            stop = TRUE;
        if (!stop && !adj_invariable(w))
            singulier_mot(w);
        if (strlen(buf) + strlen(w) + 2 < BUFSZ) {
            Strcat(buf, w);
            if (sep) {
                size_t bl = strlen(buf);

                buf[bl] = sep;
                buf[bl + 1] = '\0';
            }
        }
        p = sep ? q + 1 : q;
    }
    return buf;
}

/*
 * Conjugaison au present de l'indicatif.
 */
static const struct {
    const char *inf, *s3, *p3, *p2;
} verbes_irreg[] = {
    { "être", "est", "sont", "êtes" },
    { "avoir", "a", "ont", "avez" },
    { "aller", "va", "vont", "allez" },
    { "faire", "fait", "font", "faites" },
    { "dire", "dit", "disent", "dites" },
    { "pouvoir", "peut", "peuvent", "pouvez" },
    { "vouloir", "veut", "veulent", "voulez" },
    { "devoir", "doit", "doivent", "devez" },
    { "savoir", "sait", "savent", "savez" },
    { "voir", "voit", "voient", "voyez" },
    { "prévoir", "prévoit", "prévoient", "prévoyez" },
    { "recevoir", "reçoit", "reçoivent", "recevez" },
    { "apercevoir", "aperçoit", "aperçoivent", "apercevez" },
    { "falloir", "faut", "faut", "faut" },
    { "valoir", "vaut", "valent", "valez" },
    { "pleuvoir", "pleut", "pleuvent", "pleuvez" },
    { "venir", "vient", "viennent", "venez" },
    { "tenir", "tient", "tiennent", "tenez" },
    { "mourir", "meurt", "meurent", "mourez" },
    { "courir", "court", "courent", "courez" },
    { "fuir", "fuit", "fuient", "fuyez" },
    { "ouvrir", "ouvre", "ouvrent", "ouvrez" },
    { "couvrir", "couvre", "couvrent", "couvrez" },
    { "offrir", "offre", "offrent", "offrez" },
    { "souffrir", "souffre", "souffrent", "souffrez" },
    { "cueillir", "cueille", "cueillent", "cueillez" },
    { "accueillir", "accueille", "accueillent", "accueillez" },
    { "bouillir", "bout", "bouillent", "bouillez" },
    { "sortir", "sort", "sortent", "sortez" },
    { "partir", "part", "partent", "partez" },
    { "dormir", "dort", "dorment", "dormez" },
    { "sentir", "sent", "sentent", "sentez" },
    { "mentir", "ment", "mentent", "mentez" },
    { "servir", "sert", "servent", "servez" },
    { "vêtir", "vêt", "vêtent", "vêtez" },
    { "prendre", "prend", "prennent", "prenez" },
    { "mettre", "met", "mettent", "mettez" },
    { "battre", "bat", "battent", "battez" },
    { "connaître", "connaît", "connaissent", "connaissez" },
    { "paraître", "paraît", "paraissent", "paraissez" },
    { "croître", "croît", "croissent", "croissez" },
    { "naître", "naît", "naissent", "naissez" },
    { "plaire", "plaît", "plaisent", "plaisez" },
    { "taire", "tait", "taisent", "taisez" },
    { "vivre", "vit", "vivent", "vivez" },
    { "suivre", "suit", "suivent", "suivez" },
    { "lire", "lit", "lisent", "lisez" },
    { "écrire", "écrit", "écrivent", "écrivez" },
    { "décrire", "décrit", "décrivent", "décrivez" },
    { "boire", "boit", "boivent", "buvez" },
    { "croire", "croit", "croient", "croyez" },
    { "rire", "rit", "rient", "riez" },
    { "sourire", "sourit", "sourient", "souriez" },
    { "luire", "luit", "luisent", "luisez" },
    { "reluire", "reluit", "reluisent", "reluisez" },
    { "cuire", "cuit", "cuisent", "cuisez" },
    { "conduire", "conduit", "conduisent", "conduisez" },
    { "détruire", "détruit", "détruisent", "détruisez" },
    { "produire", "produit", "produisent", "produisez" },
    { "réduire", "réduit", "réduisent", "réduisez" },
    { "construire", "construit", "construisent", "construisez" },
    { "rompre", "rompt", "rompent", "rompez" },
    { "interrompre", "interrompt", "interrompent", "interrompez" },
    { "vaincre", "vainc", "vainquent", "vainquez" },
    { "convaincre", "convainc", "convainquent", "convainquez" },
    { "coudre", "coud", "cousent", "cousez" },
    { "moudre", "moud", "moulent", "moulez" },
    { "résoudre", "résout", "résolvent", "résolvez" },
    { "dissoudre", "dissout", "dissolvent", "dissolvez" },
    { "absoudre", "absout", "absolvent", "absolvez" },
    { "conclure", "conclut", "concluent", "concluez" },
    { "inclure", "inclut", "incluent", "incluez" },
    { "exclure", "exclut", "excluent", "excluez" },
    { "clore", "clôt", "closent", "closez" },
    { "asseoir", "assoit", "assoient", "assoyez" },
    { "émouvoir", "émeut", "émeuvent", "émouvez" },
    { "mouvoir", "meut", "meuvent", "mouvez" },
    { "haïr", "hait", "haïssent", "haïssez" },
    { "envoyer", "envoie", "envoient", "envoyez" },
    { "renvoyer", "renvoie", "renvoient", "renvoyez" },
    { "acquérir", "acquiert", "acquièrent", "acquérez" },
    { "conquérir", "conquiert", "conquièrent", "conquérez" },
};

/* verbes du 2e groupe courants dans le jeu (finir -> finit/finissent) */
static const char *const verbes_2e[] = {
    "finir", "rougir", "noircir", "blanchir", "jaunir", "verdir",
    "bleuir", "pâlir", "grandir", "grossir", "maigrir", "rajeunir",
    "vieillir", "guérir", "agir", "réagir", "saisir", "choisir",
    "remplir", "salir", "punir", "bondir", "rebondir", "surgir",
    "jaillir", "rugir", "frémir", "gémir", "vomir", "pourrir", "mûrir",
    "rétrécir", "durcir", "ramollir", "affaiblir", "envahir", "trahir",
    "obéir", "réussir", "nourrir", "périr", "subir", "ternir", "fléchir",
    "réfléchir", "franchir", "éblouir", "évanouir", "atterrir",
    "assombrir", "éclaircir", "élargir", "enrichir", "étourdir",
    "s'évanouir", "resplendir", "retentir", "applaudir", "brunir",
    "roussir", "moisir", "flétrir", "fournir", "garnir", "unir",
    "réunir", "aplatir", "engloutir", "anéantir", "ralentir", "chérir",
    "accomplir", "adoucir", "noircir", "épaissir", "vernir", "polir",
    "dépérir", "rôtir", "glapir", "vrombir", "mugir", "hennir", "ravir",
    "assouplir", "établir", "démolir", "embellir", "enlaidir",
    (const char *) 0
};

/* -ir non listes du 3e groupe se conjuguant comme sortir/partir :
   on choisit par defaut le 2e groupe, sauf terminaisons reconnues */

staticfn void
conj_simple(const char *inf, int pers, boolean pl, char *out, size_t sz)
{
    char stem[BUFSZ];
    size_t l = strlen(inf);
    int i;

    /* irreguliers, y compris composes (devenir, revenir, retenir...) */
    for (i = 0; i < SIZE(verbes_irreg); i++) {
        size_t li = strlen(verbes_irreg[i].inf);

        if (l >= li && !strcmp(inf + l - li, verbes_irreg[i].inf)) {
            const char *fin = (pers == 2) ? verbes_irreg[i].p2
                              : pl ? verbes_irreg[i].p3
                                   : verbes_irreg[i].s3;
            /* prefixe eventuel (de-venir, re-tenir, dis-paraître...) */
            if (li < l && strcmp(verbes_irreg[i].inf, "dire")
                && strcmp(verbes_irreg[i].inf, "lire")
                && strcmp(verbes_irreg[i].inf, "rire")
                && strcmp(verbes_irreg[i].inf, "voir")
                && strcmp(verbes_irreg[i].inf, "être")
                && strcmp(verbes_irreg[i].inf, "avoir")
                && strcmp(verbes_irreg[i].inf, "aller")
                && strcmp(verbes_irreg[i].inf, "taire")
                && strcmp(verbes_irreg[i].inf, "faire")) {
                Snprintf(out, sz, "%.*s%s", (int) (l - li), inf, fin);
                return;
            } else if (li == l) {
                Snprintf(out, sz, "%s", fin);
                return;
            } else if (!strcmp(verbes_irreg[i].inf, "faire")
                       || !strcmp(verbes_irreg[i].inf, "dire")
                       || !strcmp(verbes_irreg[i].inf, "voir")) {
                /* refaire, redire, revoir : prefixe acceptable */
                if (!strcmp(verbes_irreg[i].inf, "dire") && pers == 2)
                    fin = "disez"; /* contredisez, prédisez */
                Snprintf(out, sz, "%.*s%s", (int) (l - li), inf, fin);
                return;
            }
        }
    }
    if (l < 3) {
        Snprintf(out, sz, "%s", inf);
        return;
    }
    Strcpy(stem, inf);
    if (finit_par(inf, "er")) {
        stem[l - 2] = '\0';
        if (pers == 2) {
            Snprintf(out, sz, "%sez", stem);
            return;
        }
        /* changements de radical : -yer, -eler/-eter, e + consonne + er,
           é + consonne(s) + er */
        {
            size_t sl = strlen(stem);
            char base[BUFSZ];

            Strcpy(base, stem);
            if (sl >= 2 && base[sl - 1] == 'y'
                && (base[sl - 2] == 'o' || base[sl - 2] == 'u')) {
                base[sl - 1] = 'i'; /* nettoyer -> nettoie */
            } else if (sl >= 3 && (finit_par(base, "el")
                                   || finit_par(base, "et"))
                       && strcmp(base, "achet") && strcmp(base, "gel")
                       && strcmp(base, "pel") && strcmp(base, "model")
                       && !finit_par(base, "ébel") && !finit_par(base, "cel")
                       && !finit_par(base, "écel")) {
                base[sl] = base[sl - 1]; /* jeter -> jette */
                base[sl + 1] = '\0';
            } else if (sl >= 3 && base[sl - 2] == 'e'
                       && !strchr("aeiouy", base[sl - 1])) {
                /* lever -> lève, acheter -> achète, geler -> gèle */
                char tmp[BUFSZ];

                Snprintf(tmp, sizeof tmp, "%.*sè%s", (int) (sl - 2), base,
                         base + sl - 1);
                Strcpy(base, tmp);
            } else if (sl >= 4) {
                /* é + consonne(s) final : sécher -> sèche, régner -> règne */
                char *e = strstr(base + (sl >= 5 ? sl - 5 : 0), "é");

                if (e) {
                    char *after = e + 2;

                    if (*after && !strchr("aeiouy", *after)
                        && (after[1] == '\0'
                            || (!strchr("aeiouy", after[1])
                                && after[2] == '\0'))) {
                        e[1] = (char) 0xA8; /* é -> è */
                    }
                }
            }
            Snprintf(out, sz, "%s%s", base, pl ? "ent" : "e");
        }
        return;
    }
    if (finit_par(inf, "ir")) {
        boolean deux = FALSE;

        stem[l - 2] = '\0';
        for (i = 0; verbes_2e[i]; i++)
            if (!strcmp(inf, verbes_2e[i]))
                deux = TRUE;
        if (!deux && !finit_par(inf, "tir") && !finit_par(inf, "vir")
            && !finit_par(inf, "mir") && !finit_par(inf, "llir")
            && !finit_par(inf, "frir") && !finit_par(inf, "vrir"))
            deux = TRUE;
        if (deux) {
            Snprintf(out, sz, "%s%s", stem,
                     (pers == 2) ? "issez" : pl ? "issent" : "it");
        } else {
            /* sortir/dormir/servir : supprimer la consonne au singulier */
            char sg[BUFSZ];

            Strcpy(sg, stem);
            if (strlen(sg) > 1)
                sg[strlen(sg) - 1] = '\0';
            if (pers == 2)
                Snprintf(out, sz, "%sez", stem);
            else if (pl)
                Snprintf(out, sz, "%sent", stem);
            else
                Snprintf(out, sz, "%s%c", sg, stem[strlen(stem) - 1] == 'v'
                                             ? 't' : 't');
        }
        return;
    }
    if (finit_par(inf, "aître") || finit_par(inf, "oître")) {
        stem[l - strlen("ître")] = '\0';
        Snprintf(out, sz, "%s%s", stem,
                 (pers == 2) ? "issez" : pl ? "issent" : "ît");
        return;
    }
    if (finit_par(inf, "indre") || finit_par(inf, "oindre")) {
        /* éteindre -> éteint/éteignent, peindre, craindre, joindre */
        if (pers == 2 || pl) {
            stem[l - 4] = '\0';
            Snprintf(out, sz, "%s%s", stem, (pers == 2) ? "gnez" : "gnent");
        } else {
            stem[l - 3] = '\0';
            Snprintf(out, sz, "%st", stem);
        }
        return;
    }
    if (finit_par(inf, "re")) {
        /* fondre, rendre, perdre, mordre, répandre, tordre... */
        stem[l - 2] = '\0';
        Snprintf(out, sz, "%s%s", stem,
                 (pers == 2) ? "ez" : pl ? "ent" : "");
        return;
    }
    if (finit_par(inf, "oir")) {
        stem[l - 3] = '\0';
        Snprintf(out, sz, "%s%s", stem,
                 (pers == 2) ? "ez" : pl ? "oient" : "oit");
        return;
    }
    Snprintf(out, sz, "%s", inf);
}

char *
fr_conj(const char *inf, int pers, boolean pl)
{
    char *buf = frbuf();
    char verbe[BUFSZ], rest[BUFSZ], conj[BUFSZ];
    const char *sp;

    if (!inf || !*inf)
        return buf;
    rest[0] = '\0';
    /* pronominal : "se briser", "s'éteindre" */
    if (!strncmp(inf, "se ", 3) || !strncmp(inf, "s'", 2)) {
        const char *v = inf + ((inf[1] == '\'') ? 2 : 3);

        conj_simple(v, pers, pl, conj, sizeof conj);
        if (pers == 2)
            Snprintf(buf, BUFSZ, "vous %s", conj);
        else if (fr_elision(conj))
            Snprintf(buf, BUFSZ, "s'%s", conj);
        else
            Snprintf(buf, BUFSZ, "se %s", conj);
        return buf;
    }
    /* "verbe complement" : ne conjuguer que le premier mot */
    if ((sp = strchr(inf, ' ')) != 0) {
        Snprintf(verbe, sizeof verbe, "%.*s", (int) (sp - inf), inf);
        Strcpy(rest, sp);
    } else {
        Strcpy(verbe, inf);
    }
    conj_simple(verbe, pers, pl, conj, sizeof conj);
    Snprintf(buf, BUFSZ, "%s%s", conj, rest);
    return buf;
}

/* majuscule initiale compatible UTF-8 */
char *
fr_upstart(char *s)
{
    unsigned char *u = (unsigned char *) s;

    if (!s)
        return s;
    if (*u >= 'a' && *u <= 'z')
        *u = (unsigned char) (*u - ('a' - 'A'));
    else if (*u == 0xC3 && u[1] >= 0xA0 && u[1] <= 0xBE && u[1] != 0xB7)
        u[1] = (unsigned char) (u[1] - 0x20);
    else if (*u == 0xC5 && u[1] == 0x93)
        u[1] = 0x92; /* œ -> Œ */
    return s;
}

/*francais.c*/
