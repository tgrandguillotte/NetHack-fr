/* NetHack 5.0  francais.h  -- couche grammaticale de la version francaise */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef FRANCAIS_H
#define FRANCAIS_H

/*
 * Aides grammaticales pour la traduction francaise.  Toutes les fonctions
 * qui renvoient un char * utilisent les tampons tournants de objnam.c
 * (comme an() ou the()) : le resultat doit etre utilise ou copie
 * rapidement, ne jamais le liberer.
 *
 * Genre grammatical : FR_MASC ou FR_FEM.  Il est deduit du nom principal
 * de la chaine (premier nom apres l'article et les adjectifs antéposés),
 * a l'aide d'un dictionnaire (src/francais.c) puis de suffixes usuels.
 */

#define FR_MASC 0
#define FR_FEM  1

/* genre et nombre d'un groupe nominal ("une epee longue", "les flèches") */
extern int fr_genre(const char *);
extern boolean fr_pluriel(const char *);
/* la chaine commence-t-elle par une voyelle ou un h muet (elision) ? */
extern boolean fr_elision(const char *);

/* articles et contractions ; la chaine passee est SANS article.
 *   the("épée")  -> "l'épée"       the("flèches") -> "les flèches"
 *   an("épée")   -> "une épée"     an("flèches")  -> "des flèches"
 *   du("anneau") -> "de l'anneau"  du("dragon")   -> "du dragon"
 *   au("autel")  -> "à l'autel"    au("dragon")   -> "au dragon"
 *   de("or")     -> "d'or"         de("fer")      -> "de fer"
 * Les versions majuscules (The, An, Du, Au) mettent la 1re lettre en
 * capitale.  Si la chaine commence deja par un article ou un possessif
 * (le, la, l', les, un, une, des, votre, vos, son, sa, ses...), the() et
 * an() la renvoient telle quelle, et du()/au() contractent l'article
 * existant ("le chien" -> "du chien", "les chiens" -> "aux chiens").
 */
/* an(), An(), the(), The() sont declarees dans extern.h (objnam.c) */
extern char *du(const char *);
extern char *Du(const char *);
extern char *au(const char *);
extern char *Au(const char *);
extern char *de(const char *);

/* accord d'un adjectif ou d'un participe avec un groupe nominal :
 *   accord("l'épée")      -> "e"      accord("les flèches") -> "es"
 *   accord("le chien")    -> ""       accord("les chiens")  -> "s"
 * usage : pline("%s est maudit%s.", The(xname(obj)), accord(xname(obj)));
 */
extern const char *accord(const char *);
/* accord d'un adjectif complet : fr_adj("maudit", FR_FEM, TRUE) ->
   "maudites" ; gere -eux/-euse, -if/-ive, -el/-elle, -en/-enne,
   -er/-ère, -et/-ette, blanc/blanche, etc. */
extern char *fr_adj(const char *adj_masc_sing, int genre, boolean pluriel);
/* idem, en prenant genre et nombre d'un groupe nominal */
extern char *fr_adj_accord(const char *adj_masc_sing, const char *nom);

/* accord avec le heros (vouvoiement singulier) : "e" si heroine, sinon "" */
#define UE (flags.female ? "e" : "")
/* accord avec un monstre selon son sexe reel ("il"/"elle") */
#define MON_IL(m) ((m)->female ? "elle" : "il")
#define MON_E(m) ((m)->female ? "e" : "")

/* conjugaison au present de l'indicatif a partir de l'infinitif :
 *   fr_conj("briller", 3, FALSE) -> "brille"
 *   fr_conj("briller", 3, TRUE)  -> "brillent"
 *   fr_conj("être",    2, TRUE)  -> "êtes"   (vous)
 *   fr_conj("se briser", 3, TRUE) -> "se brisent"
 * personne 3 = il/elle/ils/elles ; personne 2 + pluriel = vous.
 * otense(obj, "briller"), vtense(sujet, "briller"), aobjnam(),
 * Tobjnam(), yobjnam() prennent maintenant un INFINITIF francais.
 */
extern char *fr_conj(const char *infinitif, int personne, boolean pluriel);

/* pluriel d'un nom ou groupe nominal : "épée longue" -> "épées longues",
   "potion de soins" -> "potions de soins" (makeplural() fait de meme) */
/* makeplural()/makesingular() sont dans extern.h */

/* mise en majuscule de la premiere lettre, compatible UTF-8 (é -> É) ;
   upstart() l'utilise desormais */
extern char *fr_upstart(char *);

/* nombres ordinaux : 1 -> "1er", 2 -> "2e" ; ordin() suit cette regle */

/* nom d'affichage francais d'un donjon (dungeon.c) */
extern const char *dname_fr(const char *);

/* noms anglais d'origine (src/noms_en.c), pour les recherches par nom */
extern const char *const en_obj_names[];
extern const char *const en_obj_descrs[];
extern const char *const en_defsym_expl[];
extern const char *const en_mon_names[][NUM_MGENDERS];

#endif /* FRANCAIS_H */
