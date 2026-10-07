-- NetHack quest.lua	$NHDT-Date: 1781994887 2026/06/20 22:34:47 $  $NHDT-Branch: NetHack-5.0 $:$NHDT-Revision: 1.11 $
-- Copyright (c) 2021 by Pasi Kallinen
-- NetHack may be freely redistributed.  See license for details.
-- TODO:
--  - output = "verbalize"
--  - export the quest string replacements to lua, instead of %H etc
--  - allow checking if hero is carrying item (see comments for %Cp Arc 00042)
--  - fold quest_portal, quest_portal_again, quest_portal_demand into one
--  - write tests to check questtext validity?
--  - qt_pager hack(?): if (qt_msg->delivery == 'p' && strcmp(windowprocs.name, "X11"))

-- Traduction française : les substitutions (%l, %n, %H, %i, %o...) produisent
-- des groupes nominaux AVEC article (« le Grand Maître », « le Collège
-- d'Archéologie ») ; on évite donc « de %l » / « à %l » (pas de contraction
-- possible) lorsque le nom n'est pas un nom propre, et on préfère l'apposition
-- (« votre maître, %l ») ou des prépositions sans contraction (dans, vers,
-- pour, par...).  Les accords avec le sexe du héros étant impossibles ici, les
-- textes emploient des tournures invariables.


-- text = "something"
-- Text is shown to the user.

-- synopsis = "something"
-- Synopsis is inserted into the message history.
--
-- output = "pline" | "menu" | "text"
-- The output can be manually set by using output = "menu"
-- Valid values for output are "pline", "text", and "menu, defaulting to
-- pline, unless the text contains newlines, or is too long to fit a message buffer,
-- then will be shown as a text window instead.



questtext = {
   -- If a role doesn't have a specific message, try a fallback
   msg_fallbacks = {
      goal_alt = "goal_next"
   },
   common = {
      TEST_PATTERN = {
         output = "text",
         text = [[%p:	return(plname);
 %c:	return(pl_character);
 %r:	return((char *)rank_of(u.ulevel));
 %R:	return((char *)rank_of(MIN_QUEST_LEVEL));
 %s:	return((flags.female) ? "sister" : "brother" );
 %S:	return((flags.female) ? "daughter" : "son" );
 %l:	return((char *)ldrname());
 %i:	return(intermed());
 %o:	return(artiname());
 %O:	return(shortened(artiname()));
 %n:	return((char *)neminame());
 %g:	return((char *)guardname());
 %G:	return((char *)align_gtitle(u.ualignbase[1]));
 %H:	return((char *)homebase());
 %a:	return(Alignnam(u.ualignbase[1]));
 %A:	return(Alignnam(u.ualign.type));
 %d:	return((char *)align_gname(u.ualignbase[1]));
 %D:	return((char *)align_gname(A_LAWFUL));
 %C:	return("chaotic");
 %N:	return("neutral");
 %L:	return("lawful");
 %x:	return((Blind) ? "sense" : "see");
 %Z:	return("The Dungeons of Doom");
 %%:	return(percent_sign);
 a suffix:	return an(root);
 A suffix:	return An(root);
 C suffix:	return capitalized(root);
 h suffix:	return pronoun(he_or_she, mon_of(root)); /* for %l,%n,%d,%o */
 H suffix:	return capitalized(pronoun(he_or_she, mon_of(root)));
 i suffix:	return pronoun(him_or_her, mon_of(root));
 I suffix:	return capitalized(pronoun(him_or_her, mon_of(root)));
 j suffix:	return pronoun(his_or_her, mon_of(root));
 J suffix:	return capitalized(pronoun(his_or_her, mon_of(root)));
 p suffix:	return makeplural(root);
 P suffix:	return makeplural(capitalized(root));
 s suffix:	return s_suffix(root);
 S suffix:	return s_suffix(capitalized(root));
 t suffix:	return strip_the_prefix(root);]],
      },
      angel_cuss = {
         "\"Repens-toi, et tu obtiendras le salut !\"",
         "\"Tu paieras pour ton insolence !\"",
         "\"Très bientôt, mon enfant, tu rencontreras ton créateur.\"",
         "\"Le grand %D m'a envoyé te faire payer tes péchés !\"",
         "\"Le courroux de %D s'abat à présent sur toi !\"",
         "\"Ta vie appartient désormais à %D !\"",
         "\"Désires-tu recevoir ta dernière bénédiction ?\"",
         "\"Tu n'es qu'un néant sans dieu.\"",
         "\"Tu n'es pas digne de chercher l'Amulette.\"",
         "\"Nul ne s'attend à l'Inquisition espagnole !\"",
         "\"Le jugement a été prononcé contre toi, %p.\"",
         "\"L'heure de rendre tes comptes est venue, %p.\"",
         "\"Tu seras traîné devant %D pour tes crimes !\"",
         "\"Avec %D pour témoin, je vais t'abattre.\"",
      },
      banished = {
         synopsis = "[Vous êtes chassé de votre foyer, %H, pour avoir trahi votre allégeance à %d.]",
         output = "text",
         text = [["Vous avez trahi tous ceux qui ont prêté allégeance à %d, comme
vous-même le fîtes jadis.  Mon allégeance à %d demeure inébranlable, et je
ne puis ni tolérer ni accepter ce que vous avez fait.

Quittez ces lieux.  Jamais plus vous ne remettrez les pieds dans %H.
Ce que vous cherchez est désormais perdu à jamais, car sans la Cloche
d'Ouverture, vous ne pourrez jamais pénétrer dans le lieu où réside
celui qui détient l'Amulette.

Partez maintenant !  Je vous bannis de ces lieux.]],
      },
      demon_cuss = {
         "\"Ta tête de pierre m'a d'abord fait croire à une statue.\"",
         "\"Tu viens souvent ici ?\"",
         "\"La douleur t'excite-t-elle ?  Préférerais-tu le fouet ?\"",
         "\"Crois-tu que cela te chatouillera quand je t'arracherai les poumons ?\"",
         "\"Mange de la vase et crève !\"",
         "\"Vas-y, va chercher ta maman !  J'attendrai.\"",
         "\"Va donc jouer à saute-mouton avec un troupeau de licornes !\"",
         "\"As-tu bu, ou es-tu toujours aussi gauche ?\"",
         "\"Pour cette fois, je me contenterai d'une fessée, mais que cela ne se reproduise pas.\"",
         "\"J'ai connu des blobs acides plus malins (et plus jolis).\"",
         "\"Regarde !  Ton lacet est défait !\"",
         "\"Pitié !  Veux-tu me faire mourir de rire ?\"",
         "\"Sauve-toi !  Survis pour fuir un autre jour !\"",
         "\"Tu ferais bien de te battre mieux que tu ne t'habilles !\"",
         "\"Entre ta cousine et toi, c'est encore Méduse la plus jolie.\"",
         "\"Il me semble que ce cadavre, là-bas, t'a mis dans un drôle d'état, hein, maraud ?\"",
         "\"Va te faire cuire un œuf, espèce de bœuf !\"",
         "\"En vérité, ton cadavre ne saurait sentir plus mauvais !\"",
         "\"Attends !  Je vais me métamorphoser en insecte de grille pour te laisser une chance !\"",
         "\"Pourquoi chercher l'Amulette ?  Tu ne ferais que la perdre, crétin.\"",
         "\"Tu devrais faire le comique, tes talents sont si risibles !\"",
         "\"Ton regard est si vide que j'ai cru voir un œil flottant !\"",
         "\"Même un flagelleur mental ne voudrait pas grignoter ta cervelle !\"",
         "\"Seul ton reflet pourrait t'aimer !\"",
         "\"As-tu songé à masquer ton odeur ?\"",
         "\"Halte !  Ton visage est une torture des plus exquises !\"",
         "\"Je devrais péter dans ta direction, mais cela risquerait d'améliorer ton odeur !\"",
      },
      legacy = {
         synopsis = "[%dC vous a confié la mission de récupérer l'Amulette de Yendor en son nom.]",
         output = "menu",
         text = [[Il est écrit dans le Livre de %d :

    Après la Création, le cruel dieu Moloch se rebella
    contre l'autorité de Marduk le Créateur.
    Moloch déroba à Marduk le plus puissant de tous
    les artefacts des dieux, l'Amulette de Yendor,
    et la cacha dans les sombres cavités de la Géhenne,
    le Monde d'En-Bas, où il se tapit désormais en
    attendant son heure.

Votre %G %d cherche à posséder l'Amulette, et à acquérir
grâce à elle une juste suprématie sur les autres dieux.

Vous venez d'achever votre formation de %r, et depuis votre
naissance, on vous annonce comme l'instrument de %d.  Votre destin
est de récupérer l'Amulette pour votre divinité, ou de périr
en essayant.  L'heure de votre destinée a sonné.  Pour le salut
de nous tous : allez avec courage, et que %d vous accompagne !]],
      },
-- starting with 'pauper' option set, last paragraph differs from normal legacy
      pauper_legacy = {
         synopsis = "[%dC vous a confié la mission de récupérer l'Amulette de Yendor en son nom.]",
         output = "menu",
         text = [[Il est écrit dans le Livre de %d :

    Après la Création, le cruel dieu Moloch se rebella
    contre l'autorité de Marduk le Créateur.
    Moloch déroba à Marduk le plus puissant de tous
    les artefacts des dieux, l'Amulette de Yendor,
    et la cacha dans les sombres cavités de la Géhenne,
    le Monde d'En-Bas, où il se tapit désormais en
    attendant son heure.

Votre %G %d cherche à posséder l'Amulette, et à acquérir
grâce à elle une juste suprématie sur les autres dieux.

Vous, %r sans formation, n'avez pu vous préparer
convenablement à devenir l'instrument de %d.  Néanmoins,
votre destin est de récupérer l'Amulette pour votre divinité,
ou de périr en essayant.  L'heure de votre destinée a sonné.
Pour le salut de nous tous : allez avec courage, et que %d
vous accompagne !]],
      },
      quest_complete_no_bell = {
         text = [["La cloche d'argent que gardait jalousement %n sera
indispensable pour localiser l'Amulette de Yendor."]],
      },
      quest_portal = {
         output = "pline",
         text = [[Vous recevez un faible message télépathique de votre mentor, %l :
On a besoin de votre aide de toute urgence dans %H !
Cherchez un transporteur ...ique.
Vous n'avez pas bien saisi la fin du message.]],
      },
      quest_portal_again = {
         text = "Vous sentez de nouveau %l implorer votre aide.",
      },
      quest_portal_demand = {
         text = "Vous sentez de nouveau %l exiger votre présence.",
      },
   },
   Arc = {
      assignquest = {
         synopsis = "[%nC a volé %o.  Trouvez %i, vainquez-%ni et rapportez %O.]",
         output = "text",
         text = [["Des temps bien sombres sont tombés sur le collège, car %n a
dérobé %o.  Sans cet objet, le conseil d'administration
de l'université n'aura bientôt plus d'autre choix que de supprimer nos
crédits de recherche.

"Vous devez trouver l'entrée menant vers %i.  À l'intérieur,
vous trouverez %n.

"Vous devrez ensuite vaincre %n et me rapporter
%o.

"Ce n'est qu'ainsi que nous pourrons empêcher les coupes budgétaires qui
risquent de faire fermer ce collège.

"Puisse la sagesse de %d vous guider."]],
      },
      badalign = {
         synopsis = "[\"%pC, vous avez quitté le chemin %a.  Purifiez-vous !\"]",
         output = "text",
         text = [["%pC !  J'ai entendu dire que vous employiez des méthodes bâclées.
Vos résultats récents sont à peine dignes de votre rang de %r !

"Comment avez-vous pu quitter le chemin %a ?  Partez d'ici, et ne revenez
que lorsque vous aurez purifié votre âme."]],
      },
      badlevel = {
         synopsis = "[%pC, avec votre rang de %r, vous manquez encore d'expérience.]",
         output = "text",
         text = [["%p, vous manquez encore d'expérience pour entreprendre une quête
aussi exigeante.  Avec votre modeste rang de %r, vous ne pourriez
affronter les épreuves qui vous attendent et survivre.  Partez, et
revenez ici lorsque vos aventures vous en auront appris davantage."]],
      },
      discourage = {
         "\"Faites de votre mieux, %p.  Vous ne pouvez me vaincre.\"",
         "\"Je vous arracherai la chair des os tandis que vous respirez encore !\"",
         "\"D'abord vous, %p, puis je détruirai votre mentor, %l.\"",
         "\"Déjà las, %p ?  Je tire ma puissance de mon maître et ne puis faiblir !\"",
         "\"J'arracherai ton âme de ton corps et je la dévorerai !\"",
         "\"Votre penchant %a est bien trop fort -- il vous affaiblit.  Vous mourrez en ce lieu.\"",
         "\"%d vous a tourné le dos !  Plus rien ne peut vous sauver !\"",
         "\"Avec votre rang de %r, vous ne pouvez espérer me vaincre !\"",
         "\"Si vous êtes ce que %l peut envoyer de mieux, je n'ai rien à craindre.\"",
         "\"Meurs, %c !  J'exposerai ta carcasse comme un trophée.\"",
      },
      encourage = {
         "\"Prenez garde, car %n est puissant et rusé.\"",
         "\"Pour trouver l'entrée menant vers %i, vous devrez franchir bien des pièges.\"",
         "\"%nC est peut-être vulnérable aux attaques de froid magique.\"",
         "\"Invoquez %d lorsque vous affronterez %n.\"",
         "\"Vous devez détruire %n.  Sinon, il vous poursuivra.\"",
         "\"%oC est un talisman puissant.  Grâce à lui, vous pourrez détruire %n.\"",
         "\"Partez avec la bénédiction de %d.\"",
         "\"Je demanderai à mes %gp de guetter votre retour.\"",
         "\"Souvenez-vous de ne pas vous écarter du vrai chemin %a.\"",
         "\"Vous pourrez peut-être sentir la présence de %o lorsque vous en serez proche.\"",
      },
      firsttime = {
         synopsis = "[Vous arrivez dans %H, mais tout ne va pas bien.]",
         output = "text",
         text = [[Vous vous retrouvez soudain dans un décor familier.  Les bâtiments au loin
semblent être ceux de votre chère alma mater, mais quelque chose ne va pas.
On dirait qu'une émeute a récemment eu lieu, ou que %H a
été assiégé.

Toutes les fenêtres sont condamnées par des planches, et des objets
jonchent les abords de l'entrée.

D'étranges formes menaçantes semblent se mouvoir au loin.]],
      },
      goal_alt = {
         text = "Vous voici de nouveau dans le repaire de votre ennemi, %n.",
      },
      goal_first = {
         synopsis = "[Cette étrange sensation doit être la présence de %o.]",
         output = "text",
         text = [[Une étrange sensation vous envahit, et vous repensez aux choses
apprises lors des nombreux cours de votre mentor, %l.

Vous comprenez que cette sensation doit être la présence de %o.]],
      },
      goal_next = {
         text = "La présence familière de %o flotte dans l'éther.",
      },
      gotit = {
         synopsis = "[Le pouvoir de %o parcourt votre corps !  Vous devez le rapporter à %l.]",
         output = "text",
         text = [[Le pouvoir de %o parcourt votre corps !  Vous avez
l'impression de pouvoir affronter le Sorcier de Yendor en personne et
l'emporter, mais vous savez que vous devez rapporter %o à %l.]],
      },
      guardtalk_after = {
         "\"Tu as vu Lash LaRue dans 'Song of Old Wyoming' l'autre soir ?\"",
         "\"Hé, mon vieux, t'aurais pas des potions d'hallucination à vendre ?\"",
         "\"Je suppose que ta titularisation est assurée, maintenant.\"",
         "\"Alors, qu'est-ce qui était le pire, %n ou tes examens d'entrée ?\"",
         "\"%oC est impressionnant, mais ça ne vaut pas les ossements que j'ai déterrés !\"",
      },
      guardtalk_before = {
         "\"Tu as vu Lash LaRue dans 'Song of Old Wyoming' l'autre soir ?\"",
         "\"Hé, mon vieux, t'aurais pas des potions d'hallucination à vendre ?\"",
         "\"Tu as vu l'artefact que %l a rapporté de la dernière fouille ?\"",
         "\"Alors, d'après *toi*, de quelle espèce descendons-nous ?\"",
         "\"Alors c'est toi, la fierté de %l !  Je me demande bien ce qu'il te trouve.\"",
      },
      hasamulet = {
         synopsis = "[Emportez l'Amulette sur le Plan Astral et sacrifiez-la sur l'autel de %d.]",
         output = "text",
         text = [["Félicitations, %p.  Je me demandais si quiconque pourrait triompher
du Sorcier et des serviteurs de Moloch.  À présent, vous devez vous lancer
dans une ultime aventure.

"Prenez l'Amulette, et trouvez le chemin du Plan Astral.
Là, vous devrez trouver l'autel de %d et y sacrifier
l'Amulette afin d'accomplir votre destinée.

"Souvenez-vous : votre chemin doit désormais toujours monter."]],
      },
      killed_nemesis = {
         text = "Le corps de %n se dissipe en un nuage de vapeurs nocives.",
      },
      leader_first = {
         synopsis = "[\"Vous voici de retour, %p, face à une tâche ardue.\"]",
         output = "text",
         text = [["Vous voici enfin de retour, %p.  De tous mes élèves,
vous avez toujours été le plus grand espoir.  Permettez-moi de voir si vous
êtes à la hauteur de la tâche la plus difficile de votre carrière."]],
      },
      leader_last = {
         synopsis = "[\"%pC, vous nous avez déçus.  Hors d'ici !\"]",
         output = "text",
         text = [["%p, vous nous avez déçus.  Toute ma patiente formation aura été
vaine.  Hors d'ici !  Votre titularisation dans ce collège est révoquée !

"Vous êtes la honte de la profession !"]],
      },
      leader_next = {
         text = [["De nouveau, %p, vous voici devant moi.
Voyons si vous avez acquis de l'expérience entre-temps."]],
      },
      leader_other = {
         text = [["Une fois de plus, %p, vous revenez du terrain.
Êtes-vous enfin à la hauteur de la tâche qui doit être accomplie ?"]],
      },
      locate_first = {
         synopsis = "[Cet édifice inquiétant doit cacher l'entrée menant vers %i.]",
         output = "text",
         text = [[Une plaine s'ouvre devant vous.  Au-delà se dresse un édifice inquiétant.

Vous avez le sentiment que vous trouverez bientôt l'entrée menant vers
%i.]],
      },
      locate_next = {
         text = "Une fois de plus, vous êtes près de l'entrée menant vers %i.",
      },
      nemesis_first = {
         synopsis = "[\"Viens, %p, je vais te détruire !\"]",
         output = "text",
         text = [["Ainsi, %p, tu crois pouvoir réussir à reprendre
%o, alors que ton maître, %l, a déjà échoué.

"Viens, fais de ton mieux !  Je te détruirai, et je rongerai tes os."]],
      },
      nemesis_next = {
         synopsis = "[\"Tu essaies encore de me battre, %p ?  Jamais tu ne reprendras %o.\"]",
         output = "text",
         text = [["Tu essaies encore de me battre, hein, %p ?  Eh bien, tu échoueras
encore.

"Jamais tu ne reprendras %o.

"J'emporterai ton âme jusqu'au Plan des Origines, pour le plaisir de mon
maître."]],
      },
      nemesis_other = {
         text = "\"Tu persistes encore, %p !  Bien.  À présent, tu vas mourir !\"",
      },
      nemesis_wantsit = {
         text = [["Je te reprendrai %o, %p, puis je me repaîtrai
de tes entrailles !"]],
      },
      nexttime = {
         text = "Une fois de plus, vous voici de retour dans %H.",
      },
      offeredit = {
         synopsis = "[%lC vous charge désormais de veiller sur %o.]",
         output = "text",
         text = [[%lC touche brièvement %o, l'observe attentivement,
puis vous sourit et déclare :

"Bien joué, %p.  Vous avez vaincu %n et
récupéré %o.  Mais je crains qu'il ne soit jamais en
sécurité ici.

Je vous en prie, emportez %o avec vous.  Vous, %p,
saurez désormais le protéger bien mieux que moi.

Que les bénédictions de %d vous suivent et vous protègent."]],
      },
      offeredit2 = {
         synopsis = "[\"Reprenez votre quête de l'Amulette au-delà du portail magique menant vers %Z.\"]",
         output = "text",
         text = [["Attention, %p !  %oC pourrait se briser, et ce serait
une perte tragique.  C'est à vous de veiller sur lui désormais, et l'heure est venue
de reprendre votre quête de l'Amulette.  %Z attendent votre
retour par le portail magique que vous avez emprunté pour venir ici."]],
      },
      othertime = {
         text = [[Vous voici de retour dans %H.
Vous avez l'étrange sentiment que vous venez peut-être ici pour la dernière fois.]],
      },
      posthanks = {
         synopsis = "[\"Avez-vous progressé dans votre quête de l'Amulette de Yendor pour %d ?\"]",
         output = "text",
         text = [["Bon retour parmi nous, %p.  Avez-vous progressé dans votre quête
pour reprendre l'Amulette de Yendor au nom de %d ?"]],
      },
   },
   Bar = {
      assignquest = {
         synopsis = "[\"Trouvez %n, vainquez-%ni et rapportez-nous %o.\"]",
         output = "text",
         text = [["Le monde a grand besoin de votre aide, %p.

"Il y a environ six mois, j'ai appris qu'un mystérieux sorcier, connu
sous le nom de %n, avait commencé à rassembler autour de lui une vaste
bande de coupe-jarrets et de brigands.

"À peu près au même moment, ces gens avec qui vous chevauchiez jadis ont
'libéré' un puissant talisman magique, %o, d'une caravane
touranienne.

"%nC et sa Horde Noire ont déferlé sur %i et vaincu
ceux qui s'y trouvaient, les chassant dans le désert.  Il s'est emparé de
%o, et cherche à le plier à sa volonté.  J'ai perçu
les subtils changements dans les courants du destin, et j'ai rejoint ces gens.
Puis je vous ai fait mander.

"Si %n parvient à plier %o à sa volonté, il deviendra
presque indestructible.  Il pourra alors asservir l'esprit des hommes
dans le monde entier.  Vous êtes notre seul espoir.  Les dieux vous sourient,
et avec %d à vos côtés, vous, et personne d'autre, pouvez vaincre %n.

"Vous devez vous rendre dans %i.  De là, vous pourrez traquer
%n, %ni vaincre, et nous rapporter %o.  Alors
seulement, le monde sera sauf."]],
      },
      badalign = {
         synopsis = "[\"Vous avez quitté le chemin %a.  Revenez lorsque vous aurez expié.\"]",
         output = "text",
         text = [["%pC !  Vous avez quitté le chemin %a !
Si vous tentez de terrasser %n dans cet état, il asservira
sûrement votre âme.  Votre seul espoir, et le nôtre, réside dans votre
purification.  Partez, et revenez lorsque vous vous sentirez à la hauteur."]],
      },
      badlevel = {
         synopsis = "[\"Vous manquez d'expérience.  Revenez lorsque vous aurez atteint le rang de %R.\"]",
         output = "text",
         text = [["%p, je crains que vous ne manquiez encore d'expérience pour affronter
%n.  Seul quelqu'un du rang de %R, avec l'aide de %d, pourrait
espérer %ni vaincre."]],
      },
      discourage = {
         "\"Mes bêtes se régaleront de ta carcasse ce soir !\"",
         "\"Tu fais honte au rang de %r.\"",
         "\"Fuis tant que tu le peux, %c.  Mon prochain sort sera ton dernier.\"",
         "\"Je me servirai de ta propre peau pour relier mon prochain grimoire.\"",
         "\"%d ne peut plus te protéger.  C'est ici que tu meurs.\"",
         "\"Ta nature %a te rend faible.  Tu ne peux me vaincre.\"",
         "\"Viens, %c.  Je vais te tuer, puis lâcher la horde sur ta tribu.\"",
         "\"Une fois ta mort venue, ma horde achèvera %l, et ta tribu avec.\"",
         "\"Bats-toi, %c, à moins que tu ne craignes le puissant %n ?\"",
         "\"Tu as échoué, %c.  À présent, ma victoire est totale.\"",
      },
      encourage = {
         "\"%nC est versé dans les arts obscurs, mais il ne résiste pas à l'acier froid.\"",
         "\"N'oubliez pas que %n est un grand sorcier.  Il vivait au temps de l'Atlantide.\"",
         "\"Si vous échouez, %p, je ne pourrai protéger ces gens bien longtemps.\"",
         "\"Pour entrer dans %i, vous devrez faire preuve d'une grande discrétion.  La horde montera la garde.\"",
         "\"Invoquez %d lorsque vous serez dans le besoin.\"",
         "\"Puisse %d vous protéger, et guider vos pas.\"",
         "\"Si vous mettez la main sur %o, gardez-le sur vous, il vous portera chance.\"",
         "\"Je ne puis résister à la sorcellerie de %n.  Mais %d vous aidera.\"",
         "\"Ne craignez pas %n.  Je sais que vous pouvez %ni vaincre.\"",
         "\"Une longue route vous attend, %p, mais seulement après avoir vaincu %n.\"",
      },
      firsttime = {
         synopsis = "[Vous approchez de votre foyer, %H, mais vous sentez une magie maléfique toute proche.]",
         output = "text",
         text = [[Avec méfiance, vous scrutez les alentours, tous vos sens aux aguets,
à l'affût du moindre danger.  Au loin, vous pouvez %x les silhouettes
familières de %H.

Mais pourquoi, vous demandez-vous, %l serait-il là ?

Soudain, les poils de votre nuque se hérissent : vous percevez dans l'air
l'aura d'une magie maléfique.

Sans réfléchir, vous apprêtez votre arme, et marmonnez entre vos dents :

    "Par %d, le sang coulera aujourd'hui."]],
      },
      goal_first = {
         synopsis = "[C'est sûrement le repaire de %n.]",
         output = "text",
         text = [[Les poils de votre nuque se dressent tandis que vous sentez une énergie
dans l'air même qui vous entoure.  Vous réprimez une panique primitive qui
vous pousse à tourner les talons et à fuir.  C'est sûrement le repaire de %n.]],
      },
      goal_next = {
         text = "Une fois de plus, vous sentez l'air autour de vous chargé d'une énergie magique malveillante.",
      },
      gotit = {
         synopsis = "[Vous sentez le pouvoir de %o couler entre vos mains.]",
         output = "text",
         text = [[Lorsque vous ramassez %o, vous sentez son pouvoir
couler entre vos mains.  Il semble se trouver en deux endroits ou plus
à la fois, bien que vous le teniez.]],
      },
      guardtalk_after = {
         "\"Les batailles ont été belles ici -- le sang de nos ennemis imbibe le sol !\"",
         "\"Souviens-toi : la gloire, c'est d'écraser ses ennemis sous ses pieds !\"",
         "\"Les temps seront de nouveau prospères, maintenant que la horde est vaincue.\"",
         "\"En vainquant %n, tu as apporté un grand honneur à notre clan.\"",
         "\"Tu seras digne de succéder à %l.\"",
      },
      guardtalk_before = {
         "\"Les batailles ont été belles ici -- le sang de nos ennemis imbibe le sol !\"",
         "\"Souviens-toi : la gloire, c'est d'écraser ses ennemis sous ses pieds !\"",
         "\"Il n'y a guère eu de trésor à piller depuis l'arrivée de la horde.\"",
         "\"La horde est nombreuse, mais ils n'ont guère de courage.\"",
         "\"%lC est un drôle de bonhomme, mais il nous a aidés à nous défendre.\"",
      },
      hasamulet = {
         synopsis = "[\"Portez l'Amulette jusqu'à l'autel de %d sur le Plan Astral et offrez-la.\"]",
         output = "text",
         text = [["C'est merveilleux, %p.  Je craignais que vous ne puissiez réussir
votre quête, et pourtant vous voici en possession de l'Amulette
de Yendor !

"J'ai étudié sans relâche les textes des mages depuis votre départ.  Dans
le Livre de Skelos, j'ai trouvé ceci :

    %d fera envoyer un enfant dans le monde.  Cet enfant
    sera endurci par l'épreuve de la bataille et de la magie, car
    telle est la volonté de %d.  Il est dit que l'enfant de %d
    reprendra l'Amulette de Yendor, qui fut dérobée au Créateur à
    l'aube des temps.

"Puisque vous possédez désormais l'amulette, %p, je soupçonne que le Livre
parle de vous.

    L'enfant de %d prendra l'Amulette, et voyagera jusqu'au Plan
    Astral, où se trouve le Grand Temple de %d.  L'Amulette
    sera sacrifiée à %d, là, sur son autel.  Alors l'enfant
    se tiendra aux côtés de %d, champion de tous les %cp
    pour l'éternité.

"C'est là tout ce que je sais, %p.  J'espère que cela vous aidera."]],
      },
      killed_nemesis = {
         synopsis = "[%nC vous maudit, mais vous sentez l'écrasante aura de magie se dissiper.]",
         output = "text",
         text = [[%nC s'effondre et vous lance une dernière malédiction.  Puis son
corps s'estompe lentement, semblant se disperser dans l'air autour de vous.
Peu à peu, vous prenez conscience que l'écrasante aura de magie dans l'air
commence à se dissiper.]],
      },
      leader_first = {
         synopsis = "[\"Vous voici enfin de retour.  Une grande quête vous attend.\"]",
         output = "text",
         text = [["Ah, %p.  Vous voici enfin de retour.  Le monde a cruellement
besoin de votre aide.  Une grande quête vous attend.

"Mais d'abord, je dois voir si vous êtes à la hauteur d'un tel défi."]],
      },
      leader_last = {
         synopsis = "[\"Vous avez trahi %d ; bientôt %n nous détruira.  Hors d'ici !\"]",
         output = "text",
         text = [["Peuh !  Vous avez trahi les dieux, %p.  Jamais vous n'atteindrez
la gloire à laquelle vous aspirez.  Pour n'avoir pas suivi le vrai chemin,
cet avenir vous est désormais fermé.

"Je protégerai ces gens du mieux que je pourrai, mais bientôt %n aura
raison de moi et détruira tous ceux qui jadis vous appelaient %s.
Maintenant, hors d'ici !"]],
      },
      leader_next = {
         text = "\"%p, vous voici de retour.  Êtes-vous maintenant à la hauteur du défi ?\"",
      },
      leader_other = {
         text = "\"De nouveau, vous voici devant moi, %p.  Vous avez sûrement fait vos préparatifs.\"",
      },
      locate_first = {
         synopsis = "[Vous avez trouvé %i.]",
         output = "text",
         text = [[La brise du désert vous apporte une odeur d'eau.  Vous savez que
vous avez trouvé %i.]],
      },
      locate_next = {
         text = "Une fois de plus, vous avez l'occasion de vous infiltrer dans %i.",
      },
      nemesis_first = {
         synopsis = "[%nC se vante d'avoir tué bien des gens.  \"Prépare-toi à mourir, %c.\"]",
         output = "text",
         text = [["Tiens.  Voilà donc ce que ce sorcier de second ordre, %l, envoie
exécuter ses volontés.  J'en ai tué bien d'autres avant toi.  Tu ne
m'offriras guère de divertissement.

"Prépare-toi à mourir, %c."]],
      },
      nemesis_next = {
         text = "\"J'ai déjà perdu trop de temps avec toi.  À présent, tu vas mourir.\"",
      },
      nemesis_other = {
         text = "\"Tu reviens encore, %c !  Viens-tu enfin chercher la mort ?\"",
      },
      nemesis_wantsit = {
         text = [["Je reprendrai %o, toi qui fais honte aux %cp.
Et ta vie avec."]],
      },
      nexttime = {
         text = [[Une fois de plus, vous approchez de %H.  Vous savez que %l
vous y attendra.]],
      },
      offeredit = {
         synopsis = "[%lC vous dit de veiller sur %o, et de revenir lorsque vous aurez triomphé.]",
         output = "text",
         text = [[Lorsque %l voit %o, il sourit et déclare :

    Bien joué, %p.  Vous avez sauvé le monde d'une perte certaine.
    Que faire, à présent, de %o ?

    Ces gens, aussi braves soient-ils, ne peuvent espérer le protéger
    des autres sorciers qui le détecteront, aussi sûrement que %n l'a fait.

    Prenez %o avec vous, %p.  Il vous protégera au
    cours de vos aventures, et c'est vous qui saurez le mieux le protéger.
    Vous vous lancez dans une quête bien plus grande que vous ne le croyez.

    Souvenez-vous de moi, %p, et revenez lorsque vous aurez triomphé.
    Je vous dirai alors ce que vous devrez faire.  Vous comprendrez le
    moment venu.]],
      },
      offeredit2 = {
         synopsis = "[\"Gardez %o.  Retournez dans %Z pour chercher l'Amulette.\"]",
         output = "text",
         text = [[%lC contemple %o avec révérence, puis vous regarde.

"C'est à vous de veiller sur lui désormais, et l'heure est venue de
reprendre votre quête de l'Amulette.  %Z attendent votre retour par
le portail magique que vous avez emprunté pour venir ici."]],
      },
      othertime = {
         text = [[De nouveau, et peut-être pour la dernière fois, pensez-vous, vous
approchez de %H.]],
      },
      posthanks = {
         text = "\"Dites-nous, %p, votre grande quête se déroule-t-elle bien ?\"",
      },
   },
   Cav = {
      assignquest = {
         synopsis = "[Trouvez et vainquez %n, reprenez %o et revenez avec.]",
         output = "text",
         text = [["Vous êtes en effet à la hauteur désormais, %p.  Je vais vous conter
les grandes souffrances qu'a endurées votre peuple :

"Peu après votre départ pour votre quête de vision, les cavernes ont été
envahies par les créatures que %n a lancées contre nous.

"Elle-même ne pouvait nous attaquer à cause de sa grande taille, mais ses
sbires nous harcèlent depuis lors.  Lors des premières attaques, beaucoup
sont morts, et les sbires de la bête sont parvenus à dérober %o.
Ils l'ont emporté dans %i, et aucun de nos
guerriers %gp ne s'est montré capable de s'y rendre.

"Vous devez trouver %i, et là, arracher
%o à la bête.  Elle le garde aussi
jalousement que tous les trésors qu'elle amasse.  Mais grâce à lui,
nous pourrons de nouveau vivre en sécurité dans nos cavernes.

"Je vous en prie, %p, reprenez %o pour nous, et rapportez-le ici."]],
      },
      badalign = {
         synopsis = "[\"Vous ne suivez plus le chemin %a.  Partez, et purifiez-vous.\"]",
         output = "text",
         text = [["%pC !  Vous avez trahi mes enseignements.  Vous ne
suivez plus le chemin %a comme vous le devriez.  Je vous bannis de ces
cavernes : partez et purifiez-vous.  Alors, peut-être, pourrez-vous
accomplir cette quête."]],
      },
      badlevel = {
         synopsis = "[\"Avec votre rang de %r, vous manquez d'expérience.  Revenez lorsque vous aurez progressé.\"]",
         output = "text",
         text = [["Hélas, %p, vous manquez encore d'expérience pour vous lancer dans une
quête aussi difficile que celle que je compte vous confier.

"Avec votre rang de %r, vous ne pourriez survivre aux épreuves qu'exige
la recherche de %i, sans même parler d'affronter %n en personne.

"Partez encore à l'aventure, et vous apprendrez les talents qui vous
seront nécessaires.  Ainsi l'a décrété %d."]],
      },
      discourage = {
         "\"Tu es faible, %c.  Aucun défi pour la Mère de tous les Dragons.\"",
         "\"J'ai faim, %r.  Tu m'as l'air d'un délicieux amuse-gueule !\"",
         "\"Tu dînes avec moi ?  Tu seras le plat de résistance, %c.\"",
         "\"Avec %o, je suis invincible !  Tu ne peux réussir.\"",
         "\"Ton mentor, %l, a échoué.  Tu n'as rien de redoutable.\"",
         "\"Tu mourras ici, %c.  Avec ton rang de %r, tu ne peux espérer me vaincre.\"",
         "\"Toi, simple %r, tu défies la puissance de %n ?  Ha !\"",
         "\"Je suis la Mère de tous les Dragons !  Tu ne peux espérer me vaincre.\"",
         "\"Mes griffes sont bien aiguisées.  Je vais te mettre en lambeaux !\"",
         "\"%d t'a tourné le dos, %c.  Ici, c'est mon domaine.\"",
      },
      encourage = {
         "\"%nC est immunisée contre ses propres souffles.  Usez contre elle d'une magie qu'elle n'emploie pas elle-même.\"",
         "\"Lorsque vous affronterez %n, invoquez l'aide de %d.\"",
         "\"Il n'y aura nulle part où se cacher dans le sanctuaire où se terre %n.\"",
         "\"Face à %n, votre meilleure chance sera de rester en mouvement.\"",
         "\"Ne vous laissez pas distraire par les grands trésors du repaire où se terre %n.  Concentrez-vous sur %o.\"",
         "\"%oC est le seul objet que %n craigne vraiment.\"",
         "\"Ne vous laissez pas tromper par sa taille : %n est rapide, et on dit qu'elle use de magie.\"",
         "\"Je vous ferais bien accompagner par une troupe de %gp, mais nous aurons besoin de toutes nos forces pour nous défendre.\"",
         "\"Souvenez-vous, suivez toujours le chemin %a.  C'est là votre force.\"",
         "\"Si seulement nous avions eu une amulette de réflexion, rien de tout cela ne serait arrivé.\"",
      },
      firsttime = {
         synopsis = "[Vous voici de retour dans %H, mais quelque chose ne va pas.]",
         output = "text",
         text = [[Vous descendez un escalier à peine familier, que %l vous avait
montré, vous vous en souvenez, au moment de partir pour votre quête de vision.

Vous voici de retour dans %H, mais quelque chose
semble clocher.  La fumée habituelle et la lueur des feux des cavernes
extérieures ont disparu, et un silence inquiet emplit l'air humide.]],
      },
      goal_first = {
         synopsis = "[Vous entrez dans une vaste caverne.  %nC est là.]",
         output = "text",
         text = [[Vous vous trouvez dans une vaste caverne aux parois soigneusement polies,
qui portent néanmoins des traces de brûlures.

Des ossements jonchent le sol, et des objets sont éparpillés partout.
L'air est lourd d'une puanteur de vapeurs sulfureuses.

%nC est bien visible, mais elle semble endormie.]],
      },
      goal_next = {
         text = "Une fois de plus, vous vous trouvez dans le repaire de %n.",
      },
      gotit = {
         synopsis = "[%oC vous emplit d'un sentiment de puissance.]",
         output = "text",
         text = [[Lorsque vous ramassez %o, il vous semble lourd au premier
abord, mais tandis que vous le tenez, la force afflue dans vos bras.

Une puissance soudaine vous envahit, comme si plus rien ne pouvait
se dresser sur votre chemin.]],
      },
      guardtalk_after = {
         "\"Les pluies sont revenues et la terre reverdit.\"",
         "\"La paix est revenue, rendons grâce à %d !\"",
         "\"Bon retour !  As-tu trouvé %o ?\"",
         "\"Alors, %p, raconte-nous ton combat contre %n.\"",
         "\"%lC se fait vieux.  Peut-être nous guideras-tu après son ascension.\"",
      },
      guardtalk_before = {
         "\"Nous n'avons pas pu amasser autant de nourriture depuis que les Géants nous ont coupé l'accès au monde extérieur.\"",
         "\"Depuis que %n a envoyé ses sbires, nous ne cessons de nous battre.\"",
         "\"J'ai entendu dire que ta quête de vision avait été un succès.  Est-ce vrai ?\"",
         "\"Alors, dis-moi, %p, comment cela s'est-il passé pour toi ?\"",
         "\"%lC se fait vieux.  Nous ne savons pas qui nous guidera après son ascension.\"",
      },
      hasamulet = {
         synopsis = "[\"Portez l'Amulette jusqu'à l'autel de %d sur le Plan Astral et offrez-la.\"]",
         output = "text",
         text = [["Vous avez réussi, à ce que je vois, %p.

"Maintenant que l'Amulette de Yendor est à vous, voici ce que vous devez faire :

"Montez jusqu'à l'air libre.  L'Amulette que vous portez vous emmènera
alors dans les Plans Astraux, d'où le Grand Temple de %d
répand son influence sur notre monde.

"Sacrifiez l'Amulette sur l'autel.  Ainsi %d régnera en maître !"]],
      },
      killed_nemesis = {
         text = [[%nC s'effondre, ses têtes battant l'air en tous sens.
Tandis qu'elle meurt, un nuage de vapeurs nocives s'élève autour d'elle.]],
      },
      leader_first = {
         synopsis = "[\"Vous voici de retour.  Nous avons cruellement besoin de votre aide.\"]",
         output = "text",
         text = [["Vous voici de retour de votre quête de vision, %p.  Grâces soient rendues à %d.

"Nous avons cruellement besoin de votre aide, %S de notre peuple.

"Mais d'abord, je dois voir si vous êtes déjà capable d'accomplir la quête
que je voudrais vous confier."]],
      },
      leader_last = {
         synopsis = "[\"Vous avez trahi le camp %L.  Hors d'ici !\"]",
         output = "text",
         text = [["%pC !  Vous avez scellé notre destin.  Vous semblez incapable de vous
amender, je dois donc choisir quelqu'un d'autre pour prendre votre place.

"Quittez %H !  Vous nous avez trahis en choisissant
le chemin %C plutôt que le vrai chemin %L.

"À nos yeux, vous n'existez plus."]],
      },
      leader_next = {
         text = "\"Vous nous revenez encore, %p.  Voyons si vous êtes à la hauteur à présent.\"",
      },
      leader_other = {
         text = "\"Ah, %p.  Êtes-vous enfin à la hauteur ?\"",
      },
      locate_first = {
         synopsis = "[Vous parvenez à %x de grandes traces de griffes ; une odeur de charogne flotte et des ossements jonchent le sol.]",
         output = "text",
         text = [[Vous parvenez à %x de nombreuses et grandes traces de griffes sur le sol.
Les tunnels devant vous sont plus larges que la plupart de ceux des réseaux
de cavernes que vous avez connus jusqu'ici.

Votre nez détecte une odeur de charogne venant de l'intérieur, et des
ossements jonchent les bords des tunnels.]],
      },
      locate_next = {
         text = "Une fois de plus, vous approchez de %i.",
      },
      nemesis_first = {
         synopsis = "[%nC menace de vous dévorer.]",
         output = "text",
         text = [["Ainsi, disciple de %l, tu cherches à envahir le repaire de
%n.  Seuls mes repas sont admis ici-bas.  Prépare-toi
à finir dans mon estomac !"]],
      },
      nemesis_next = {
         text = [["Ainsi, tu m'affrontes de nouveau, %c.  Nul ne m'a jamais échappé.
À présent, je vais te tuer."]],
      },
      nemesis_other = {
         text = "\"Tu commences à m'agacer, %c.  Prépare-toi à mourir.\"",
      },
      nemesis_wantsit = {
         text = "\"Je te reprendrai %o, %c.  Tu vas mourir.\"",
      },
      nexttime = {
         text = "Une fois de plus, vous voici de retour dans %H.",
      },
      offeredit = {
         synopsis = "[\"Emportez %o.  Il vous aidera dans votre quête de l'Amulette de Yendor.\"]",
         output = "text",
         text = [[%lC aperçoit %o en votre possession.
Il sourit et déclare :

    Vous avez réussi !  Nous sommes sauvés.  Mais je crains que %o
    ne soit toujours la cible des forces %C qui voudront s'en
    emparer.

    Pour éviter d'autres ennuis, j'aimerais que vous, %p,
    emportiez %o avec vous.  Il vous aidera dans votre
    quête de l'Amulette de Yendor.]],
      },
      offeredit2 = {
         synopsis = "[\"C'est à vous de veiller sur %o désormais.  Retournez dans %Z pour chercher l'Amulette.\"]",
         output = "text",
         text = [[%lC serre fièrement %o un instant, puis vous regarde.

"C'est à vous de veiller sur lui désormais, et l'heure est venue de
reprendre votre quête de l'Amulette.  %Z attendent votre retour par
le portail magique que vous avez emprunté pour venir ici."]],
      },
      othertime = {
         text = [[Pour une raison ou une autre, vous pensez que c'est peut-être la dernière
fois que vous entrez dans %H.]],
      },
      posthanks = {
         text = [["%pC !  Bon retour parmi nous.
Comment se passe votre quête pour reprendre l'Amulette au nom de %d ?"]],
      },
   },
   Hea = {
      assignquest = {
         synopsis = "[Rendez-vous dans %i pour aller reprendre %o à %n.]",
         output = "text",
         text = [[Pour la première fois, vous devinez un sourire sur le visage de %l.

    "Vous avez en effet appris tout ce que nous pouvions vous enseigner
    pour vous préparer à cette tâche.  Laissez-moi vous dire ce que je sais
    des symptômes, en espérant que vous saurez trouver un remède.

    "Il y a peu, les dieux ont trompé %n, que tous redoutent, en
    lui faisant croire qu'%nh pourrait se servir de %o pour trouver
    un remède à la vieillesse.  Imaginez : la jeunesse éternelle !  Mais sa
    bonne santé, il ne l'obtient qu'en puisant celle de tous ceux qui
    l'entourent.

    "Il a épuisé sa propre réserve de gens bien portants, et cherche
    désormais à étendre son influence sur notre monde.  Vous devez lui
    reprendre %o et briser le sortilège.

    "Vous devez traverser les marais jusqu'à %i, et de là
    suivre la piste menant à l'île où se terre %n.  Prenez garde."]],
      },
      badalign = {
         synopsis = "[Revenez lorsque votre conduite sera plus conforme au chemin %a.]",
         output = "text",
         text = [["Vous avez beaucoup appris des remèdes qui soulagent, mais vous devez
aussi savoir quelle médecine convient à quel mal.  C'est pourquoi les
enseignements de %d font partie de votre formation.

"Revenez parmi nous lorsque vous aurez guéri votre propre âme."]],
      },
      badlevel = {
         synopsis = "[Vous manquez d'expérience.  Revenez lorsque vous aurez atteint le rang de %R.]",
         output = "text",
         text = [["Hélas, %p, vous manquez encore d'expérience pour affronter les rigueurs
d'une telle tâche.  Vous devez maîtriser la botanique, l'alchimie et l'art
vétérinaire avant que je puisse en bonne conscience vous envoyer
accomplir cette quête.

"Revenez lorsque vous porterez le caducée de %R."]],
      },
      discourage = {
         "\"Autant confier des scalpels à des magiciens que de te laisser te servir de %o !\"",
         "\"Si j'ai pu frapper %l, entouré de ses %gp, imagine ce que je peux te faire ici, à toi qui es sans secours.\"",
         "\"Je mettrai mes %Rp au travail pour tirer un remède de tes cendres.\"",
         "\"À l'heure où nous parlons, Hadès rassemble tes patients pour qu'ils te rejoignent.\"",
         "\"Quand j'en aurai fini avec toi, je détruirai %l aussi.\"",
         "\"Il te faudra me tuer si tu espères jamais quitter ce lieu.\"",
         "\"J'empalerai ta tête sur mon caducée pour que tous la voient.\"",
         "\"Il n'est dans ton sac aucune materia medica qui puisse te guérir de moi !\"",
         "\"Ne te débats pas trop, je veux ton âme forte, pas affaiblie !\"",
         "\"Tu aurais dû t'arrêter à l'école vétérinaire.\"",
      },
      encourage = {
         "\"N'oubliez pas, %p, de toujours vous laver les mains avant d'opérer.\"",
         "\"%nC ne possède aucune véritable magie qui lui soit propre.  C'est là sa faiblesse.\"",
         "\"Si votre fidélité envers %d est demeurée intacte, vous pourrez puiser dans le pouvoir de %o.\"",
         "\"Emportez avec vous des antidotes contre les poisons.\"",
         "\"Souvenez-vous que %n peut détourner les pouvoirs de %o pour blesser au lieu de guérir.\"",
         "\"J'ai fait mander Chiron, mais je crains qu'il n'arrive trop tard.\"",
         "\"Peut-être qu'à votre retour, les serpents recommenceront à muer.\"",
         "\"La peste empire à l'heure où nous parlons.  Hâtez-vous, %p !\"",
         "\"Maintes fois %n a semé le trouble sur ces terres.  Il est temps de l'éradiquer comme les maladies qu'%nh a répandues.\"",
         "\"N'ayant qu'un œil, %n devrait être facile à aveugler.  Souvenez-vous-en.\"",
      },
      firsttime = {
         synopsis = "[Vous voici de retour dans %H et devez trouver %l.]",
         output = "text",
         text = [[Quelle sorcellerie vous a fait revenir dans %H ?  L'odeur
de bûchers funéraires récents vous dit que quelque chose ne va pas chez
les guérisseurs qui exerçaient ici autrefois.

Aucun rhizotomiste ne soigne les jardins de materia medica, et où sont
donc les gens du peuple qui venaient autrefois se faire soigner ?

Vous savez que vous devez vous rendre au plus vite au collegium, à
l'iatreion de %l, pour découvrir ce qui s'est passé en votre absence.]],
      },
      goal_first = {
         synopsis = "[Vous avez atteint le repaire de %n.  Reprenez-lui %o.]",
         output = "text",
         text = [[Vous voici en vue de l'île tristement célèbre où règne %n.
Rien dans les paroles de %l ne laissait présager cela.

Vous blindant contre les gémissements des malades qui vous percent les
oreilles, vous pressez le pas pour accomplir votre tâche.  Peut-être
pourrez-vous les guérir à votre retour grâce à %o, mais pas maintenant.]],
      },
      goal_next = {
         text = "Une fois de plus, vous parvenez à %x au loin l'île où règne %n.",
      },
      gotit = {
         synopsis = "[Vous sentez le pouvoir guérisseur de %o ; vous devriez le rapporter à %l.]",
         output = "text",
         text = [[Lorsque vous ramassez %o, vous sentez son pouvoir guérisseur
commencer à réchauffer votre âme.  Vous maudissez Zeus de l'avoir pris à son
légitime propriétaire, mais au moins espérez-vous que %l pourra de
nouveau en faire bon usage.]],
      },
      guardtalk_after = {
         "\"As-tu lu ce nouveau traité sur l'usage thérapeutique des sangsues ?\"",
         "\"Peins un caducée rouge sur ton bouclier, et les monstres ne te frapperont pas.\"",
         "\"Comment te sens-tu ?  Une bonne saignée te remonterait peut-être le moral.\"",
         "\"As-tu entendu cette nouvelle théorie absurde selon laquelle les maladies seraient causées par des organismes microscopiques, et non par des humeurs viciées ?\"",
         "\"Je vois que tu rapportes %o, tu vas enfin pouvoir guérir cette peste !\"",
      },
      guardtalk_before = {
         "\"As-tu lu ce nouveau traité sur l'usage thérapeutique des sangsues ?\"",
         "\"Peins un caducée rouge sur ton bouclier, et les monstres ne te frapperont pas.\"",
         "\"J'ai réussi l'épreuve d'écriture lisible, alors ils me rétrogradent d'un rang.\"",
         "\"J'ai entendu dire que même %l n'a pas réussi à guérir Chiron.\"",
         "\"Nous pensons que %n s'est servi de ses alchimistes, et de %o, pour répandre sur la Géhenne une nouvelle maladie que nous appelons 'le rhume'.\"",
      },
      hasamulet = {
         synopsis = "[\"Vous avez repris l'Amulette.  Gagnez le Plan Astral et rendez-la à %d.\"]",
         output = "text",
         text = [["Ah, vous avez repris l'Amulette, %p.  Bien joué !

"À présent, sachez que vous devez traverser les Plans Élémentaires
jusqu'au Plan Astral, et là, rendre l'Amulette à %d.  Partez, et
puissent nos prières vous pousser comme un vent favorable."]],
      },
      killed_nemesis = {
         synopsis = "[%nC vous maudit en mourant.]",
         output = "text",
         text = [[Le corps meurtri de %n s'affaisse sur le sol, et dans un dernier
souffle, il lance une ultime malédiction :

    "Tu m'as vaincu, %p, mais j'aurai ma vengeance.
    Comment, je ne le dirai pas, mais cette malédiction te rongera
    comme un cancer."

Sur ces mots, %n meurt.]],
      },
      leader_first = {
         synopsis = "[%lC est affaibli par sa lutte contre %n.  %lH veut vous examiner.]",
         output = "text",
         text = [[Faiblement, %l relève la tête pour vous regarder.

"Je suis heureux de vous revoir, %p.  Je lis l'inquiétude dans vos
yeux, mais ne vous en faites pas pour moi.  Je ne suis pas encore mûr pour
Hadès.  Nous avons épuisé une grande partie de nos pouvoirs de guérison
à contenir %n.  J'ai besoin de vos forces neuves pour poursuivre notre
œuvre.

"Approchez, laissez-moi vous imposer les mains, et déterminer si vous
possédez les talents nécessaires pour accomplir cette mission."]],
      },
      leader_last = {
         synopsis = "[Vous êtes la honte des guérisseurs.]",
         output = "text",
         text = [["Vous nous avez déçus, %p.  Vous n'êtes qu'un rebouteux !  Un charlatan !

"Hadès sera ravi d'apprendre que vous exercez de nouveau votre art sur
des innocents."]],
      },
      leader_next = {
         text = [["Vous me revenez encore, %p.  Je sens qu'à chaque retour, la pleurésie
et les maux de notre terre commencent à vous infecter.  Espérons, et
prions %d, que vous soyez à la hauteur de votre tâche avant de succomber
aux humeurs viciées."]],
      },
      leader_other = {
         text = [["Chiron est tombé, Hermès est tombé, que dois-je vous dire de plus pour
vous faire comprendre l'importance de votre mission !  J'espère que
vous avez fait vos préparatifs, cette fois."]],
      },
      locate_first = {
         synopsis = "[Vous avez atteint %i, mais tout ne va pas bien.]",
         output = "text",
         text = [[Vous vous tenez devant l'entrée menant vers %i.  D'étranges
grattements proviennent de l'intérieur du bâtiment.

Le sol marécageux autour de vous semble empester la maladie.]],
      },
      locate_next = {
         text = "Une fois de plus, vous vous tenez à l'entrée menant vers %i.",
      },
      nemesis_first = {
         synopsis = "[\"Je vais prendre ta vie, puis vaincre %l.\"]",
         output = "text",
         text = [["Ils ont commis une erreur en t'envoyant, %p.

"Quand j'aurai ajouté ta jeunesse à la mienne, il me sera d'autant plus
facile de vaincre %l."]],
      },
      nemesis_next = {
         text = "\"Contrairement à tes patients, toi, tu sembles toujours revenir, %p !\"",
      },
      nemesis_other = {
         text = "\"Que préfères-tu, %p ?  Furoncles, pleurésie, convulsions ?\"",
      },
      nemesis_wantsit = {
         text = [["Je te reprendrai %o, %r.  Tu ne quitteras
pas ce lieu en vie."]],
      },
      nexttime = {
         text = [[Après votre dernière visite, vous vous attendiez à revenir ici, mais certainement
pas à trouver une situation aussi dégradée.  Cette fois, vous devez réussir.]],
      },
      offeredit = {
         synopsis = "[%lC touche %o et demande à ses %gp d'en faire autant, puis vous dit de l'emporter.]",
         output = "text",
         text = [[Dès que %l aperçoit %o, %lh convoque ses
%gp.

Doucement, %l tend la main et touche %o.
Il demande à chacun des présents d'en faire autant.  Lorsque tous ont
terminé, %lh s'adresse à vous.

    "Maintenant que nos forces sont restaurées, nous pouvons vaincre cette
    peste.  Vous devez emporter %o et restaurer les mondes que
    vous devrez parcourir ensuite.  J'aimerais que Chiron puisse
    vous porter jusqu'au terme de votre voyage, mais j'ai besoin de lui
    pour m'aider à répandre le remède.  Partez à présent, et poursuivez
    votre voyage."]],
      },
      offeredit2 = {
         synopsis = "[%lC vous dit de garder %o et de retourner dans %Z pour chercher l'Amulette.]",
         output = "text",
         text = [[%lC manipule %o avec précaution tout en vous observant.

"C'est à vous de veiller sur lui désormais, et l'heure est venue de
reprendre votre quête de l'Amulette.  %Z attendent votre retour par
le portail magique que vous avez emprunté pour venir ici."]],
      },
      othertime = {
         text = [[De nouveau, vous parvenez à %x %H au loin.

Une odeur de mort et de maladie imprègne l'air.  Nul besoin d'avoir le
rang de %R pour savoir que %n est sur le point de triompher.]],
      },
      posthanks = {
         text = [["Vous nous revenez encore, %p.  Nous nous sommes bien débrouillés en votre
absence, n'est-ce pas ?  Comment se passe votre quête de l'Amulette ?"]],
      },
   },
   Kni = {
      assignquest = {
         synopsis = "[Traversez %i pour atteindre %n.  Détruisez-%ni et revenez avec %o.]",
         output = "text",
         text = [["Ah, %p.  Vous êtes véritablement à la hauteur, comme nul %c
avant vous ne le fut.  Oyez à présent Nos paroles :

"Ainsi que vous l'avez remarqué en approchant de %H, une grande
bataille s'est livrée naguère en ces champs.  Sachez que Merlin en personne
vint Nous prêter main-forte tandis que Nous combattions l'immonde %n.
Au plus fort de la bataille, %n porta à Merlin un coup terrible, qui
le terrassa.  Puis, tandis que Nos troupes étaient repoussées, %n
déroba %o.

"Nous finîmes par renverser le cours de la bataille, mais au prix de
nombreux %cp.  Merlin fut emporté par son apprenti, mais ne s'est point
rétabli.  L'on Nous a dit que tant que %n posséderait %o,
Merlin ne recouvrerait point la santé.

"Par la présente, Nous vous confions ce devoir, le plus important de tous :

"Partez d'ici vers les marais, et là, vous trouverez
%i.  De là, il vous faudra traquer %n.  Détruisez la
bête, et rapportez-Nous %o.  Alors seulement,
Nous pourrons rendre la santé à Merlin."]],
      },
      badalign = {
         synopsis = "[Allez faire pénitence.  Revenez lorsque vous suivrez de nouveau le chemin %a.]",
         output = "text",
         text = [["Vous Nous déshonorez, %p !  Vous avez quitté le chemin de la
chevalerie !  Ôtez-vous de Notre présence et faites pénitence.  Ce n'est
que lorsque votre cœur sera de nouveau pur que vous pourrez revenir céans."]],
      },
      badlevel = {
         synopsis = "[Vous n'êtes pas de taille à affronter %n.  Revenez lorsque vous aurez atteint le rang de %R.]",
         output = "text",
         text = [["En vérité, %p, vous vous êtes bien conduit.  Avoir survécu jusqu'ici
fait honneur à votre vaillance, mais vous n'êtes point encore apte à
répondre aux exigences qu'impose le rôle de Notre Champion.  Avec le rang
de %r, nul, si pur fût-il, ne saurait espérer vaincre l'immonde %n.

"Partez d'ici, et affûtez vos talents.  Revenez en Notre
présence lorsque vous aurez atteint le noble titre de %R."]],
      },
      discourage = {
         "\"Toi, simple %r, tu ne saurais me résister !\"",
         "\"Je vais te tuer sur-le-champ, et festoyer !\"",
         "\"Chétif %c.  Quelle sorte de mort souhaites-tu ?\"",
         "\"Toi d'abord, %p, puis je me repaîtrai de %l.\"",
         "\"Ha !  Tu as échoué, %r.  À présent, tu vas mourir.\"",
         "\"Meurs, %c.  Tu n'es rien face à ma puissance.\"",
         "\"Je sucerai la moelle de tes os, %c.\"",
         "\"Voyons...  Au four ?  Non.  Frit ?  Point.  Grillé ?  Oui, en vérité, c'est ainsi que j'aime le %c pour mon dîner.\"",
         "\"Ta force décline, %p.  L'heure de ta mort approche.\"",
         "\"Invoque donc ton précieux %d, %p.  Cela ne te servira de rien.\"",
      },
      encourage = {
         "\"Souvenez-vous, %p, suivez toujours le chemin de %d.\"",
         "\"Bien que %n soit en vérité un puissant ennemi, Nous croyons en votre victoire.\"",
         "\"Prenez garde, car %n s'est entouré de hordes de créatures immondes.\"",
         "\"Un grand trésor, dit-on, est amassé dans le repaire de %n.\"",
         "\"Si vous possédez %o, %p, la magie de %n en sera déjouée.\"",
         "\"Les portes de %i sont gardées par des forces invisibles, %p.  Avancez prudemment.\"",
         "\"Rapportez-Nous promptement %o, %p.\"",
         "\"Détruisez %n, %p, ou %H tombera assurément.\"",
         "\"Invoquez %d lorsque vous serez dans le besoin.\"",
         "\"Pour trouver %i, il vous faudra garder le cœur pur.\"",
      },
      firsttime = {
         synopsis = "[Parmi les traces de bataille, de longues entailles marquent les murs de %H.]",
         output = "text",
         text = [[Vous vous matérialisez dans l'ombre de %H.  Aussitôt, vous remarquez
que quelque chose ne va pas.  Les champs autour du château sont piétinés et
flétris, comme si une grande bataille s'y était livrée récemment.

En explorant plus avant, vous parvenez à %x de longues entailles dans les
murs de %H.  Vous ne connaissez qu'une seule créature capable de laisser
de telles marques...]],
      },
      goal_first = {
         synopsis = "[Vous parvenez à %x l'entrée d'une caverne au flanc d'une colline.]",
         output = "text",
         text = [[En sortant des marais, vous parvenez à %x devant vous un énorme trou béant
au flanc d'une colline.  De l'intérieur monte l'immonde puanteur de la charogne.

Les mares de part et d'autre de l'entrée sont souillées de sang, et des
morceaux de métal rouillé et d'armes brisées affleurent à leur surface.]],
      },
      goal_next = {
         text = "De nouveau, vous vous tenez à l'entrée du repaire de %n.",
      },
      gotit = {
         synopsis = "[Vous sentez la magie de %o.]",
         output = "text",
         text = [[Lorsque vous ramassez %o, vous sentez ses champs protecteurs
se former autour de votre corps.  Vous sentez aussi un léger frémissement
dans votre esprit, comme si vous vous trouviez en deux endroits à la fois,
et que dans le second, vous vous éveilliez d'un long sommeil.]],
      },
      guardtalk_after = {
         "\"Salut à vous, %p !  En vérité, vous avez fort bonne mine.\"",
         "\"Alors, %p, avez-vous trouvé %n dans les marais près de %i ?\"",
         "\"Noble %p, avez-vous prouvé la droiture de votre cause sur la dépouille de %n ?\"",
         "\"En vérité, %l ne saurait avoir meilleur champion, %p.\"",
         "\"Avez-vous vraiment repris %o ?\"",
      },
      guardtalk_before = {
         "\"Salut à vous, %p !  En vérité, vous avez fort bonne mine.\"",
         "\"Le bruit court, %p, que %n aurait été aperçu dans les marais près de %i.\"",
         "\"Vous êtes désormais notre seul espoir, %p.\"",
         "\"En vérité, %l ne saurait avoir meilleur champion, %p.\"",
         "\"Bien des braves %cp ont péri lors de l'attaque de %n.\"",
      },
      hasamulet = {
         synopsis = "[Portez l'Amulette sur le Plan Astral et remettez-la à %d.]",
         output = "text",
         text = [["Vous avez réussi, à ce que Nous voyons, %p !  À présent, Nous vous
ordonnons de porter l'Amulette jusqu'au Plan Astral pour qu'elle y soit
sacrifiée à %d.

"Merlin Nous a fait savoir que, pour atteindre ce but, vous devrez toujours
monter à travers les Plans des Éléments.

"Allez avec %d, %p."]],
      },
      killed_nemesis = {
         synopsis = "[%nC vous maudit en mourant.]",
         output = "text",
         text = [[Tandis que %n s'effondre, le sang jaillissant de sa gueule béante, %nh
vous maudit avec défi, vous et %l :

    "Tu n'as pas encore gagné, %r.  Par les dieux, je reviendrai
    et je te traquerai jusqu'à la tombe !"

La queue battant furieusement, %n tente de ramper vers vous, mais s'affaisse
sur le sol et meurt dans une mare de son propre sang.]],
      },
      leader_first = {
         synopsis = "[%lC vérifie si vous êtes à la hauteur d'une grande entreprise.]",
         output = "text",
         text = [["Ah, %p.  Nous voyons que Notre convocation vous est parvenue.
Nous avons grand besoin de votre vaillance.  Mais d'abord, il Nous faut
décider si vous êtes à la hauteur de cette grande entreprise."]],
      },
      leader_last = {
         synopsis = "[Vous êtes la honte des %cp.]",
         output = "text",
         text = [["Votre présence impure déshonore cette noble cour.  Nous avons été
indulgent envers vous, mais c'en est fini.  Votre nom ne sera plus jamais
prononcé.  Par la présente, Nous vous déchoyons de votre titre, de vos terres
et de votre rang parmi les %cp.
Hors de Notre vue !"]],
      },
      leader_next = {
         text = "\"Nous vous accueillons de nouveau, %p.  Nous espérons que vous êtes désormais à la hauteur.\"",
      },
      leader_other = {
         text = "\"Une fois de plus, vous vous tenez devant Nous, %p.  Êtes-vous désormais à la hauteur ?\"",
      },
      locate_first = {
         synopsis = "[Vous avez atteint %i et pouvez %x un sanctuaire.]",
         output = "text",
         text = [[Vous vous tenez au pied de %i.  À son sommet, vous pouvez %x un sanctuaire.
D'étranges énergies semblent converger ici, et les poils de votre nuque
se hérissent.]],
      },
      locate_next = {
         text = "De nouveau, vous vous tenez au pied de %i.",
      },
      nemesis_first = {
         synopsis = "[%nC vous nargue et profère une menace contre %H.]",
         output = "text",
         text = [["Ha !  Encore un chétif %c en quête de la mort.  Je dînerai bien ce soir,
et demain, %H tombera !"]],
      },
      nemesis_next = {
         text = "\"Tu me défies encore, %r ?  Soit.  Tu mourras ici.\"",
      },
      nemesis_other = {
         text = "\"Tu es vraiment insensé, %r.  Je vais t'expédier sur l'heure.\"",
      },
      nemesis_wantsit = {
         text = [["Ainsi, tu oses toucher à MON bien !  Je vais reprendre cette babiole,
chétif %r.  Tu mourras dans d'atroces souffrances !"]],
      },
      nexttime = {
         text = "Une fois de plus, vous vous tenez dans l'ombre de %H.",
      },
      offeredit = {
         synopsis = "[%oC est désormais à vous.  Il vous aidera dans votre quête de l'Amulette.]",
         output = "text",
         text = [[Lorsque vous approchez de %l, %lh vous adresse un sourire radieux et déclare :

    "Bien joué !  Vous êtes véritablement le Champion de %H.  Nous
    avons appris que Merlin se rétablit, et qu'il Nous rejoindra
    bientôt.

    "Il Nous a fait savoir que c'est à vous désormais qu'il revient de
    veiller sur %o.  Il pense que vous pourriez avoir besoin
    de ses pouvoirs au cours de vos aventures.  Nous souhaitons que vous
    gardiez %o avec vous tandis que vous chercherez la
    légendaire Amulette de Yendor."]],
      },
      offeredit2 = {
         synopsis = "[C'est à vous de veiller sur %o.  Retournez dans %Z et trouvez l'Amulette.]",
         output = "text",
         text = [["Prenez garde, %p !  %oC pourrait se briser, et ce serait
une perte tragique.  C'est à vous de veiller sur lui désormais, et l'heure
est venue de reprendre votre quête de l'Amulette.  %Z attendent votre
retour par le portail magique que vous avez emprunté pour venir céans."]],
      },
      othertime = {
         text = [[De nouveau, vous vous tenez devant %H.  Vous sentez confusément que c'est
peut-être la dernière fois que vous vous tenez devant %l.]],
      },
      posthanks = {
         text = "\"Heureuse rencontre, %p.  Comment se déroule votre quête de l'Amulette de Yendor ?\"",
      },
   },
   Mon = {
      assignquest = {
         synopsis = "[Trouvez %i, puis gagnez le repaire de %n.  Vainquez-%ni et revenez avec %o.]",
         output = "text",
         text = [["Oui, %p.  Vous êtes véritablement à la hauteur désormais.  Écoutez-moi,
et je vais vous conter ce qui s'est passé :

"Lors de l'une des Grandes Méditations, il y a peu, %n et
une légion d'élémentaires ont envahi %H.  De nombreux %gp
ont été tués, dont celui qui portait %o.

À présent, il reste à peine assez de %gp pour tenir les
élémentaires en respect.

"Nous avons besoin que vous trouviez %i, puis que, de là,
vous gagniez le repaire de %n.  Si vous parvenez à vaincre %n et à
rapporter ici %o, nous pourrons alors repousser les légions
d'élémentaires qui massacrent nos élèves.

"Allez, et que %d soit votre guide, %p."]],
      },
      badalign = {
         synopsis = "[Vous devez expier.  Revenez lorsque vous serez digne de %d.]",
         output = "text",
         text = [["C'est terrible, %p.  Vous avez quitté le vrai chemin !
Vous savez que %d exige de cet ordre la plus ardente dévotion.
Chaque %s de notre ordre doit incarner la plus haute piété.

"Partez d'ici, expiez vos péchés envers %d.  Ne revenez que
lorsque vous aurez purifié votre âme."]],
      },
      badlevel = {
         synopsis = "[Vous n'êtes pas de taille à affronter %n.  Revenez lorsque vous aurez atteint le rang de %R.]",
         output = "text",
         text = [["Hélas, %p, ce n'est pas encore le moment.  Avec votre simple rang
de %r, vous ne sauriez résister à la puissance de %n.  Repartez de par
le monde, et revenez lorsque vous aurez atteint le rang de %R."]],
      },
      discourage = {
         "\"Soumets-toi à ma volonté, %c, et je t'épargnerai.\"",
         "\"Tes pouvoirs dérisoires ne font pas le poids face à moi, %c.\"",
         "\"Je te ferai changer en zombie pour mon plaisir !\"",
         "\"Désespère, %r.  %d ne peut rien pour toi.\"",
         "\"Je me repaîtrai de ton âme pendant bien des jours, %c.\"",
         "\"Ta mort sera lente et douloureuse.  Ça, je te le promets !\"",
         "\"Tu ne peux vaincre %n, pauvre fou.  Je vais te tuer sur-le-champ.\"",
         "\"Ton précieux maître, %l, sera ma prochaine victime.\"",
         "\"Je sens tes pouvoirs t'abandonner, %r.  Tu vas mourir à présent.\"",
         "\"Avec %o, rien ne peut se dresser sur mon chemin.\"",
      },
      encourage = {
         "\"Vous pouvez l'emporter, si vous vous en remettez à %d.\"",
         "\"Souvenez-vous que %n dispose d'une grande magie.\"",
         "\"Gardez votre cœur pur, %S de notre ordre.\"",
         "\"Prenez garde, %i est entouré de hordes d'élémentaires de terre.\"",
         "\"Souvenez-vous de vos études, et vous l'emporterez !\"",
         "\"Procurez-vous %o et portez-les si vous le pouvez.  Ils vous aideront contre %n.\"",
         "\"Invoquez %d lorsque votre besoin sera le plus grand.  Il vous sera répondu.\"",
         "\"N'oubliez pas de retourner la force des élémentaires contre eux !\"",
         "\"Ne perdez pas la foi, %p.  Sinon, %n deviendra plus fort.\"",
         "\"Portez %o.  Ils vous assisteront dans vos efforts.\"",
      },
      firsttime = {
         synopsis = "[Vous avez atteint %H, mais quelque chose ne va pas.  %lC a besoin de votre aide.]",
         output = "text",
         text = [[Vous vous trouvez en vue de %H.
De toute évidence, quelque chose ne va pas.  D'étranges formes rôdent
lourdement aux abords de %H !

Vous comprenez que %l a besoin de votre aide !]],
      },
      goal_first = {
         synopsis = "[Vous êtes cerné par le soufre, la lave et les élémentaires.]",
         output = "text",
         text = [[La puanteur du soufre vous enveloppe, et les élémentaires se rapprochent
de tous côtés !

Devant vous, une petite clairière s'ouvre parmi les fosses de lave bouillonnante...]],
      },
      goal_next = {
         text = "De nouveau, vous avez envahi le domaine de %n.",
      },
      gotit = {
         synopsis = "[Vous sentez l'essence de %d et comprenez que vous devez rapporter %o à votre maître, %l.]",
         output = "text",
         text = [[Lorsque vous ramassez %o, vous sentez l'essence de
%d emplir votre âme.  Vous savez maintenant pourquoi %n les a dérobés
dans %H : grâce à eux, un %c au service de %d pourrait
aisément déjouer ses plans.

Vous percevez un message de %d.  Bien qu'il ne soit pas formulé en
paroles, vous avez l'impression de devoir retourner au plus vite auprès
de votre maître, %l.]],
      },
      guardtalk_after = {
         "\"Salutations, honorable %r.  Je suis heureux de vous revoir.\"",
         "\"Ah, %p !  Toute notre gratitude pour votre aide.\"",
         "\"Salutations, %s.  Peut-être prendrez-vous le temps de méditer avec nous ?\"",
         "\"Maintenant que cette épreuve est derrière vous, puisse %d vous apporter l'illumination.\"",
         "\"Que %d soit avec vous, %s.\"",
      },
      guardtalk_before = {
         "\"Salutations, honorable %r.  Je suis heureux de vous voir.\"",
         "\"Ah, %p !  Vous pourrez sûrement nous aider en cette heure difficile.\"",
         "\"Salutations, %s.  %lC a grand besoin de votre aide.\"",
         "\"Hélas, on dirait que même %d nous a abandonnés.\"",
         "\"Que %d soit avec vous, %s.\"",
      },
      hasamulet = {
         synopsis = "[Portez l'Amulette sur le Plan Astral et remettez-la à %d.]",
         output = "text",
         text = [["Vous avez triomphé, %p !  %d est sûrement avec vous.  À présent,
vous devez prendre l'Amulette, et la sacrifier sur l'autel de %d sur
le Plan Astral.  Je pense que je ne vous reverrai jamais en cette
vie, mais j'espère vous revoir aux pieds de %d."]],
      },
      killed_nemesis = {
         synopsis = "[En mourant, %n menace de revenir.]",
         output = "text",
         text = [[%nC halète :

    "Tu n'as vaincu que ce corps mortel.  Sache-le : mon esprit
    est fort.  Je reviendrai réclamer ce qui m'appartient !"

Sur ces mots, %n expire.]],
      },
      leader_first = {
         synopsis = "[%lC vérifie si vous êtes à la hauteur de ce grand défi.]",
         output = "text",
         text = [["Ah, %p, %S de notre ordre.  Vous voici enfin de retour parmi nous.
Un grand malheur a frappé notre ordre ; peut-être pourrez-vous nous aider.
Mais d'abord, je dois déterminer si vous êtes à la hauteur de ce
grand défi."]],
      },
      leader_last = {
         synopsis = "[Hérésie !  Vous avez totalement échoué.]",
         output = "text",
         text = [["Hérésie, %p !  Comment pouvez-vous, vous qui êtes %r, vous écarter
ainsi des enseignements de %d ?  Quittez ce temple.  Vous n'êtes plus
%s de cet ordre.  Nous prierons %d de nous envoyer une autre aide,
car vous nous avez totalement déçus."]],
      },
      leader_next = {
         text = "\"De nouveau, %S de notre ordre, vous voici devant moi.  Êtes-vous désormais à même de nous aider ?\"",
      },
      leader_other = {
         text = "\"Une fois de plus, %p, vous voici dans le sanctuaire.  Êtes-vous désormais à la hauteur ?\"",
      },
      locate_first = {
         synopsis = "[Vous avez atteint %i.  %nC se tapit plus loin.]",
         output = "text",
         text = [[Vous vous souvenez des descriptions de %i que %l
vous a données.  C'est plus loin que vous trouverez
la piste de %n.]],
      },
      locate_next = {
         text = "De nouveau, vous vous tenez devant %i.",
      },
      nemesis_first = {
         synopsis = "[Tu n'es pas %g.  Jamais tu ne reprendras %o.]",
         output = "text",
         text = [["Ah, ainsi %l a envoyé un autre %g récupérer
%o.

"Non, je vois que tu n'es pas %g.  Peut-être vais-je m'amuser un peu
aujourd'hui, finalement.  Prépare-toi à mourir, %r !  Jamais tu ne
reprendras %o."]],
      },
      nemesis_next = {
         text = "\"Ainsi, %r.  Tu me défies de nouveau.\"",
      },
      nemesis_other = {
         text = "\"Meurs à présent, %r.  %d n'a ici aucun pouvoir pour t'aider.\"",
      },
      nemesis_wantsit = {
         text = "\"Tu vas mourir, %r, et je reprendrai %o.\"",
      },
      nexttime = {
         text = "Une fois de plus, vous vous tenez devant %H.",
      },
      offeredit = {
         synopsis = "[Gardez %o.  Ils vous aideront à reprendre l'Amulette de Yendor.]",
         output = "text",
         text = [["Vous voici de retour, %p.  Et avec %o, à ce que je vois.
Félicitations.

"Je me suis plongé dans la méditation, et j'ai reçu des instructions
d'un serviteur de %d.  %d ordonne que vous conserviez
%o.  Grâce à eux, vous devrez reprendre l'Amulette
de Yendor.

"Partez, et que %d guide vos pas."]],
      },
      offeredit2 = {
         synopsis = "[Gardez %o et retournez dans %Z pour chercher l'Amulette.]",
         output = "text",
         text = [[%lC examine %o un instant,
puis pose de nouveau son regard sur vous.

"%oC doivent rester avec vous.  Servez-vous-en
lorsque vous reprendrez votre quête de l'Amulette.
%Z attendent votre retour par le portail magique
que vous avez emprunté pour venir ici."]],
      },
      othertime = {
         text = [[De nouveau, vous faites face à %H.  Votre intuition vous souffle que
c'est peut-être la dernière fois que vous venez ici.]],
      },
      posthanks = {
         text = "\"Bon retour parmi nous, %p.  Comment se passe votre quête de l'Amulette ?\"",
      },
   },
   Pri = {
      assignquest = {
         synopsis = "[%nC a envahi %H et s'est emparé de %o.  Vainquez-%ni et reprenez-la.]",
         output = "text",
         text = [["Oui, %p.  Vous êtes véritablement à la hauteur désormais.  Écoutez-moi,
et je vais vous conter ce qui s'est passé :

"Lors de l'une des Grandes Fêtes, il y a peu, %n et une légion
de morts-vivants ont envahi %H.  De nombreux %gp ont été tués, dont
celui qui portait %o.

"Dans un ultime acte de vengeance, %n a profané l'autel de ce temple.
Sans lui, nous n'avons pu monter de contre-attaque.  À présent, il reste
à peine assez de %gp pour tenir les morts-vivants en respect.

"Nous avons besoin que vous trouviez %i, puis que, de là, vous
gagniez le repaire de %n.  Si vous parvenez à vaincre %n et à rapporter
ici %o, nous pourrons alors repousser les légions de
morts-vivants qui souillent ces terres.

"Allez, et que %d soit votre guide, %p."]],
      },
      badalign = {
         synopsis = "[Vous avez quitté le chemin.  Revenez lorsque vous aurez purifié votre âme.]",
         output = "text",
         text = [["C'est terrible, %p.  Vous avez quitté le vrai chemin !
Vous savez que %d exige de cet ordre la plus ardente dévotion.
Chaque %s de notre ordre doit incarner la plus haute piété.

"Partez d'ici, expiez vos péchés envers %d.  Ne revenez que
lorsque vous aurez purifié votre âme."]],
      },
      badlevel = {
         synopsis = "[Avec votre rang de %r, vous ne sauriez résister à %n.  Revenez lorsque vous aurez atteint le rang de %R.]",
         output = "text",
         text = [["Hélas, %p, ce n'est pas encore le moment.  Avec votre simple rang
de %r, vous ne sauriez résister à la puissance de %n.  Repartez de par
le monde, et revenez lorsque vous aurez atteint le rang de %R."]],
      },
      discourage = {
         "\"Soumets-toi à ma volonté, %c, et je t'épargnerai.\"",
         "\"Tes pouvoirs dérisoires ne font pas le poids face à moi, %c.\"",
         "\"Je te ferai changer en zombie pour mon plaisir !\"",
         "\"Désespère, %r.  %d ne peut rien pour toi.\"",
         "\"Je me repaîtrai de ton âme pendant bien des jours, %c.\"",
         "\"Ta mort sera lente et douloureuse.  Ça, je te le promets !\"",
         "\"Tu ne peux vaincre %n, pauvre fou.  Je vais te tuer sur-le-champ.\"",
         "\"Ton précieux maître, %l, sera ma prochaine victime.\"",
         "\"Je sens tes pouvoirs t'abandonner, %r.  Tu vas mourir à présent.\"",
         "\"Avec %o, rien ne peut se dresser sur mon chemin.\"",
      },
      encourage = {
         "\"Vous pouvez l'emporter, si vous vous en remettez à %d.\"",
         "\"Souvenez-vous que %n dispose d'une grande magie.\"",
         "\"Gardez votre cœur pur, %S de notre ordre.\"",
         "\"Prenez garde, %i est entouré d'un vaste cimetière.\"",
         "\"Le froid magique pourrait peut-être affecter %n.\"",
         "\"Procurez-vous %o et portez-la si vous le pouvez.  Elle vous aidera contre %n.\"",
         "\"Invoquez %d lorsque votre besoin sera le plus grand.  Il vous sera répondu.\"",
         "\"Les légions de morts-vivants sont plus faibles pendant les heures du jour.\"",
         "\"Ne perdez pas la foi, %p.  Sinon, %n deviendra plus fort.\"",
         "\"Portez %o.  Elle vous assistera contre les morts-vivants.\"",
      },
      firsttime = {
         synopsis = "[Vous êtes devant %H ; les portes sont closes.  %lC a besoin de votre aide !]",
         output = "text",
         text = [[Vous vous trouvez en vue de %H.  De toute
évidence, quelque chose ne va pas.  Les portes de %H, qui
d'ordinaire restent ouvertes, sont closes.  D'étranges formes humaines
errent d'un pas traînant aux alentours.

Vous comprenez que %l a besoin de votre aide !]],
      },
      goal_first = {
         synopsis = "[La puanteur du soufre vous enveloppe, les hurlements et les gémissements sont sans fin.]",
         output = "text",
         text = [[La puanteur du soufre vous enveloppe, et les hurlements et les gémissements
des âmes torturées assaillent votre esprit.

Devant vous, une petite clairière s'ouvre parmi les fosses de lave bouillonnante...]],
      },
      goal_next = {
         text = "De nouveau, vous avez envahi le domaine de %n.",
      },
      gotit = {
         synopsis = "[Vous sentez %d en ramassant %o ; rapportez-la à votre maître, %l.]",
         output = "text",
         text = [[Lorsque vous ramassez %o, vous sentez l'essence de
%d emplir votre âme.  Vous savez maintenant pourquoi %n l'a dérobée
dans %H : grâce à elle, un %c au service de %d pourrait
aisément déjouer ses plans.

Vous percevez un message de %d.  Bien qu'il ne soit pas formulé en
paroles, vous avez l'impression de devoir retourner au plus vite auprès
de votre maître, %l.]],
      },
      guardtalk_after = {
         "\"Salutations, %r.  Je suis heureux de vous revoir.\"",
         "\"Ah, %p !  Toute notre gratitude pour votre aide.\"",
         "\"Bon retour, %s !  Avec %o, aucun mort-vivant ne peut nous résister.\"",
         "\"Loué soit %d, qui nous a délivrés de %n.\"",
         "\"Que %d soit avec vous, %s.\"",
      },
      guardtalk_before = {
         "\"Salutations, honorable %r.  Je suis heureux de vous voir.\"",
         "\"Ah, %p !  Vous pourrez sûrement nous aider en cette heure difficile.\"",
         "\"Salutations, %s.  %lC a grand besoin de votre aide.\"",
         "\"Hélas, on dirait que même %d nous a abandonnés.\"",
         "\"Que %d soit avec vous, %s.\"",
      },
      hasamulet = {
         synopsis = "[Portez l'Amulette sur le Plan Astral et offrez-la sur l'autel de %d.]",
         output = "text",
         text = [["Vous avez triomphé, %p !  %d est sûrement avec vous.  À présent,
vous devez prendre l'amulette, et la sacrifier sur l'autel de %d sur
le Plan Astral.  Je pense que je ne vous reverrai jamais en cette
vie, mais j'espère vous revoir aux pieds de %d."]],
      },
      killed_nemesis = {
         synopsis = "[%nC meurt.  Moloch sait que vous existez et en veut à %n.]",
         output = "text",
         text = [[Vous sentez un déchirement brutal dans l'éther tandis que le corps de %n
se dissout en un nuage de gaz nocif.

Soudain, une voix tonne :

    "Tu as vaincu le moindre de mes serviteurs, %r.
    Sache maintenant que Moloch a conscience de ta présence.
    Quant à toi, %n, je m'occuperai de ton échec
    à loisir."

Vous entendez alors la voix de %n, hurlant de terreur...]],
      },
      leader_first = {
         synopsis = "[Vous voici de retour et nous avons besoin de votre aide.  Êtes-vous à la hauteur ?]",
         output = "text",
         text = [["Ah, %p, %S de notre ordre.  Vous voici enfin de retour parmi nous.
Un grand malheur a frappé notre ordre ; peut-être pourrez-vous nous aider.
Mais d'abord, je dois déterminer si vous êtes à la hauteur de ce
grand défi."]],
      },
      leader_last = {
         synopsis = "[Hérésie !  Vous avez trahi les enseignements de %d.]",
         output = "text",
         text = [["Hérésie, %p !  Comment pouvez-vous, vous qui êtes %r, vous écarter
ainsi des enseignements de %d ?  Quittez ce temple.  Vous n'êtes plus
%s de cet ordre.  Nous prierons %d de nous envoyer une autre aide,
car vous nous avez totalement déçus."]],
      },
      leader_next = {
         text = "\"De nouveau, %S de notre ordre, vous voici devant moi.  Êtes-vous désormais à même de nous aider ?\"",
      },
      leader_other = {
         text = "\"Une fois de plus, %p, vous voici dans le sanctuaire.  Êtes-vous désormais à la hauteur ?\"",
      },
      locate_first = {
         synopsis = "[Vous avez trouvé %i.  La piste de %n s'étend devant vous.]",
         output = "text",
         text = [[Vous faites face à un vaste cimetière.  Le ciel au-dessus est empli de nuages
qui semblent s'épaissir vers le centre.  Vous sentez la présence de
morts-vivants plus nombreux que tous ceux que vous avez jamais rencontrés.

Vous vous souvenez des descriptions de %i que %l
vous a données.  C'est plus loin que vous trouverez la piste de %n.]],
      },
      locate_next = {
         text = "De nouveau, vous vous tenez devant %i.",
      },
      nemesis_first = {
         synopsis = "[%lC t'a envoyé, mais tu n'es pas %g.  Je vais te détruire.]",
         output = "text",
         text = [["Ah, ainsi %l a envoyé un autre %g récupérer
%o.

"Non, je vois que tu n'es pas %g.  Peut-être vais-je m'amuser un peu
aujourd'hui, finalement.  Prépare-toi à mourir, %r !  Jamais tu ne
reprendras %o."]],
      },
      nemesis_next = {
         text = "\"Ainsi, %r.  Tu me défies de nouveau.\"",
      },
      nemesis_other = {
         text = "\"Meurs à présent, %r.  %d n'a ici aucun pouvoir pour t'aider.\"",
      },
      nemesis_wantsit = {
         text = "\"Tu vas mourir, %r, et je reprendrai %o.\"",
      },
      nexttime = {
         text = "Une fois de plus, vous vous tenez devant %H.",
      },
      offeredit = {
         synopsis = "[Félicitations, %p.  Gardez %o ; allez reprendre l'Amulette.]",
         output = "text",
         text = [["Vous voici de retour, %p.  Et avec %o, à ce que je vois.
Félicitations.

"Je me suis plongé dans la méditation, et j'ai reçu des instructions
d'un serviteur de %d.  %d ordonne que vous conserviez
%o.  Grâce à elle, vous devrez reprendre l'Amulette
de Yendor.

"Partez, et que %d guide vos pas."]],
      },
      offeredit2 = {
         synopsis = "[%oC est désormais à vous.  Retournez dans %Z et trouvez l'Amulette.]",
         output = "text",
         text = [[%lC vous répète que %o est désormais à vous.

"L'heure est venue de reprendre votre quête de l'Amulette.
%Z attendent votre retour par le portail magique
que vous avez emprunté pour venir ici."]],
      },
      othertime = {
         text = [[De nouveau, vous faites face à %H.  Votre intuition vous souffle que
c'est peut-être la dernière fois que vous venez ici.]],
      },
      posthanks = {
         text = "\"Bon retour parmi nous, %p.  Comment se passe votre quête de l'Amulette ?\"",
      },
   },
   Ran = {
      assignquest = {
         synopsis = "[%nC a volé %o.  Infiltrez %i et reprenez-le pour nous.]",
         output = "text",
         text = [["Vous êtes en effet à la hauteur, %p.  Je vais vous dire ce qui s'est passé,
et pourquoi nous avons si désespérément besoin de votre aide :

"Il y a peu, les centaures des montagnes, à l'est, ont envahi
et asservi les centaures des plaines de cette région.  Leur chef
n'est plus qu'un homme de paille, au service de %n.

"Lors de notre dernière assemblée de culte ici, nous avons été assaillis
par des hordes de centaures hostiles, comme vous l'avez constaté.  Lors du
premier assaut, un groupe mené par %n en personne a réussi à forcer
l'entrée du bosquet et à dérober %o.

"Depuis, nous sommes assiégés.  Nous ne savons pas combien de temps encore
nous pourrons maintenir nos barrières magiques.

"Si nous voulons survivre, vous, %p, devez infiltrer
%i.  Là, vous trouverez un passage descendant vers
la caverne souterraine de %n.  Il a toujours convoité
%o, et le gardera sûrement.

"Reprenez %o pour nous, %p !  Alors seulement, %d sera en sécurité."]],
      },
      badalign = {
         synopsis = "[Vous ne suivez pas assez fidèlement le chemin %a.  Revenez lorsque vous aurez purifié votre âme.]",
         output = "text",
         text = [["Vous avez perdu votre chemin, %p !  Vous savez que %d exige que
nous restions purement dévoués au chemin %a !

"Vous devez nous quitter.  Revenez lorsque vous aurez purifié votre âme."]],
      },
      badlevel = {
         synopsis = "[Vous manquez d'expérience.  Revenez lorsque vous aurez atteint le rang de %R.]",
         output = "text",
         text = [["%p, vous manquez encore d'expérience pour faire face aux exigences de
ce que nous attendons de vous.  Quelqu'un du rang de %R pourrait peut-être
y parvenir.

"Revenez parmi nous lorsque vous en saurez davantage, %S de notre peuple."]],
      },
      discourage = {
         "\"Ton %d n'est rien, %c.  Tu es à moi maintenant !\"",
         "\"Sauve-toi, petit %c !  Tu ne peux espérer vaincre %n !\"",
         "\"Mes serviteurs vont te mettre en lambeaux !\"",
         "\"J'exposerai ta tête comme un trophée.  Que penses-tu de ce mur-là ?\"",
         "\"Je saccagerai le bosquet de %l, et je détruirai tous les %gp !\"",
         "\"%d t'a tourné le dos, %c.  Ton sort est scellé.\"",
         "\"Un %r ?  %lC envoie contre moi un simple %r ?  Ha !\"",
         "\"%lC a échoué, %c.  %oC ne quittera jamais ce lieu.\"",
         "\"Tu crois vraiment pouvoir me vaincre, hein, %c ?  Tu te trompes !\"",
         "\"Tu faiblis, %c.  Je vais te tuer à présent.\"",
      },
      encourage = {
         "\"On dit que les Centaures des Forêts et des Montagnes ont mis fin à leur antique querelle et s'allient désormais contre nous.\"",
         "\"%nC est fort, et très malin.\"",
         "\"Servez-vous de %o lorsque vous le trouverez.  Il vous aidera à survivre jusqu'à votre retour parmi nous.\"",
         "\"Souvenez-vous, que %d soit votre guide.\"",
         "\"Invoquez %d lorsque vous affronterez %n.  Ce seul geste le rendra furieux, et vous donnera l'avantage.\"",
         "\"%nC et les siens nous ont toujours haïs.\"",
         "\"Nous ne pourrons plus tenir le bosquet bien longtemps, %p.  Hâtez-vous !\"",
         "\"Pour infiltrer %i, vous devrez faire preuve d'une grande discrétion.\"",
         "\"Souvenez-vous que %n est un vantard.  Ne vous fiez pas à ce qu'il dit.\"",
         "\"Vous pouvez triompher, %p, si vous vous fiez à %d.\"",
      },
      firsttime = {
         synopsis = "[L'antique bosquet sylvestre est encerclé par des centaures.]",
         output = "text",
         text = [[Vous arrivez dans un décor familier.  Au loin, vous parvenez à %x
l'antique bosquet sylvestre, lieu de culte de %d.

Pourtant, quelque chose ne va pas.  Des centaures encerclent le bosquet !
Et ils ont remarqué votre présence !]],
      },
      goal_first = {
         synopsis = "[Vous descendez dans un complexe souterrain.  Des sabots claquent au loin.]",
         output = "text",
         text = [[Vous descendez dans un lieu étrange, où des parois grossièrement taillées
comme celles d'une caverne rejoignent des murs lisses et achevés, comme si
quelqu'un était en train de terminer la construction d'un complexe souterrain.

Au loin, vous entendez comme le claquement d'innombrables sabots
sur la roche.]],
      },
      goal_next = {
         text = "Une fois de plus, vous pénétrez dans le château difforme de %n.",
      },
      gotit = {
         synopsis = "[Vous ramassez %o et sentez sa puissance.  Il est temps de le rapporter à %l.]",
         output = "text",
         text = [[Lorsque vous ramassez %o, il semble luire, et une chaleur
vous envahit tout le corps.  Vous comprenez que c'est son pouvoir qui a
protégé si longtemps vos %sp contre leurs ennemis.

Vous devez maintenant le rapporter sans tarder à %l -- leur vie dépend
de votre rapidité.]],
      },
      guardtalk_after = {
         "\"%pC !  Cela fait bien des lunes !  Comment vas-tu ?\"",
         "\"Le chant des oiseaux est revenu dans le bosquet ; cela veut sûrement dire que tu as vaincu %n.\"",
         "\"%lC semble avoir recouvré une partie de ses forces.\"",
         "\"Alors, raconte-nous comment tu as pénétré dans %i, au cas où un nouveau mal y surgirait.\"",
         "\"Est-ce vraiment %o que je te vois porter ?\"",
      },
      guardtalk_before = {
         "\"%pC !  Cela fait bien des lunes !  Comment vas-tu ?\"",
         "\"%nC continue de menacer le bosquet.  Mais nous tenons bon.\"",
         "\"%lC s'affaiblit.  La magie nécessaire pour défendre le bosquet nous épuise.\"",
         "\"Souviens-toi qu'il est difficile d'entrer dans %i.  Méfie-toi des chauves-souris qui détournent l'attention.\"",
         "\"Nous devons reprendre %o.  Sans lui, nous serons submergés.\"",
      },
      hasamulet = {
         synopsis = "[Vous avez l'Amulette !  Portez-la sur le Plan Astral et offrez-la à %d.]",
         output = "text",
         text = [["Vous l'avez !  Vous avez repris l'Amulette de Yendor !
Écoutez-moi à présent, %p, et je vais vous dire ce qu'il faut faire :

"L'Amulette renferme une magie capable de vous transporter jusqu'au
Plan Astral, où réside le premier cercle de %d.

"Pour activer cette magie, vous devez monter aussi haut que possible.
Lorsque vous atteindrez le temple, sacrifiez l'Amulette à %d.

"Ainsi accomplirez-vous votre destinée."]],
      },
      killed_nemesis = {
         synopsis = "[%nC vous maudit en mourant.]",
         output = "text",
         text = [[%nC s'effondre, vous maudissant, vous et %l, puis déclare :

    "Tu m'as vaincu, %r !  Mais je te maudis une dernière fois, de
    mon dernier souffle !  Tu mourras avant d'avoir quitté mon château !"]],
      },
      leader_first = {
         synopsis = "[Vous voici de retour, %p.  Nous avons besoin de votre aide.  Êtes-vous à la hauteur ?]",
         output = "text",
         text = [["%pC !  Vous voici de retour !  Grâces soient rendues à %d.

"Nous avons grand besoin de vous.  Mais d'abord, je dois voir si vous
avez les capacités requises pour assumer cette responsabilité."]],
      },
      leader_last = {
         synopsis = "[Vous ne suivez pas assez fidèlement le chemin %a.  Nous renions le lien qui faisait de vous notre %s.]",
         output = "text",
         text = [["%pC !  Vous nous avez tous condamnés.  Vous rayonnez littéralement
d'influences %L, et vous affaiblissez ainsi le pouvoir que nous avons
élevé dans ce bosquet !

"Hors d'ici !  Nous renions le lien qui faisait de vous notre %s !
Vous êtes désormais un paria !"]],
      },
      leader_next = {
         text = "\"Une fois de plus, %p, vous voici parmi nous.  Êtes-vous désormais à la hauteur ?\"",
      },
      leader_other = {
         text = "\"Ah, vous voici de nouveau, %p.  Permettez-moi de déterminer si vous êtes à la hauteur...\"",
      },
      locate_first = {
         synopsis = "[Voici %i.  Des chauves-souris sont proches.  Méfiez-vous du wumpus !]",
         output = "text",
         text = [[Ce doit être %i.

Vous êtes dans une grotte composée de nombreuses salles, toutes reliées
par des tunnels.  Votre mission est de trouver et d'abattre le terrible
wumpus qui se terre quelque part dans la grotte, sans tomber dans un puits
sans fond ni épuiser votre réserve limitée de flèches.  Bonne chance.

Vous êtes dans la salle 9 de la grotte.  Des tunnels mènent aux salles
5, 8 et 10.
*froufrou* *froufrou* (des chauves-souris doivent être proches.)
*reniflement* (Je sens l'odeur du terrible wumpus tout près !)]],
      },
      locate_next = {
         synopsis = "[Vous êtes dans %i.  Il y a des puits.  Des chauves-souris sont proches.]",
         output = "text",
         text = [[Une fois de plus, vous descendez dans %i.

*fffuiit* (Je sens un courant d'air venant de puits.)
*froufrou* *froufrou* (des chauves-souris doivent être proches.)]],
      },
      nemesis_first = {
         synopsis = "[Tu viens reprendre %o, mais je vais le garder et tu vas mourir.]",
         output = "text",
         text = [["Ainsi, %c.  %lC te charge de reprendre %o.

"Eh bien, je vais garder cette babiole.  Elle me plaît.  Toi, %c, tu vas mourir."]],
      },
      nemesis_next = {
         text = "\"Encore toi, hein ?  Eh bien, un simple %r ne me fait pas peur !  Meurs, %c !\"",
      },
      nemesis_other = {
         text = "\"Tu n'as pas retenu la leçon, %c.  Tu ne peux pas me tuer !  Tu vas mourir à présent.\"",
      },
      nemesis_wantsit = {
         text = [["Je te reprendrai %o, %r.  Puis je te
tuerai."]],
      },
      nexttime = {
         text = "Une fois de plus, vous vous tenez devant %H.",
      },
      offeredit = {
         synopsis = "[Vous avez réussi.  Emportez %o dans votre quête de l'Amulette.]",
         output = "text",
         text = [["%pC !  Vous avez réussi !  Je craignais que ce ne soit impossible !

"Vous nous rapportez %o !

"Je crains à présent que les Centaures ne se regroupent pour préparer un
nouveau raid.  Cela prendra du temps, mais si vous pouvez reprendre
l'Amulette de Yendor pour %d d'ici là, nous serons à jamais en sécurité.

"Emportez %o avec vous.  Il vous aidera dans votre quête
de l'Amulette."]],
      },
      offeredit2 = {
         synopsis = "[C'est à vous de veiller sur %o désormais.  Allez trouver l'Amulette.]",
         output = "text",
         text = [[%lC bande %o avec révérence.

"Avec cet arc merveilleux, on ne manque jamais de flèches.
C'est à vous de veiller sur lui désormais, et l'heure est venue de reprendre
votre quête de l'Amulette.  %Z attendent votre retour
par le portail magique que vous avez emprunté pour venir ici."]],
      },
      othertime = {
         text = [[Vous avez l'étrange sentiment que c'est peut-être la dernière fois que
vous entrez dans %H.]],
      },
      posthanks = {
         text = [["Bienvenue, %p.  Comment se passe votre quête de l'Amulette
de Yendor ?"]],
      },
   },
   Rog = {
      assignquest = {
         synopsis = "[Reprenez %o à %n et rapportez-le à votre patron, %l.]",
         output = "text",
         text = [["Que tous ceux qui ne veulent pas aller récupérer %o chez
cet abruti, %n, fassent un pas en arrière.  Bon choix,
%p, parce que de toute façon, c'est vous que j'allais envoyer.  Mes autres
%gp me sont trop précieux.

"Voilà le topo.  Je veux %o, %n
a %o.  Vous allez récupérer %o
et me le rapporter.  Une mission si simple que même vous pouvez la
comprendre."]],
      },
      badalign = {
         synopsis = "[Revenez lorsque vous suivrez vraiment le chemin %a.]",
         output = "text",
         text = [["Je devrais peut-être vous enchaîner un moment à mon perchoir.  Voir à
l'œuvre de vrais professionnels, fidèles au chemin %a, vous remettrait
peut-être du plomb dans la cervelle.  Mais je ne crois pas que je pourrais
supporter votre vue aussi longtemps.  Revenez quand on pourra vous faire
confiance pour vous conduire correctement."]],
      },
      badlevel = {
         synopsis = "[Avec votre rang de %r, vous n'êtes pas assez formé pour ce boulot.]",
         output = "text",
         text = [["Pendant toute votre absence, vous n'avez réussi qu'à
maîtriser les arts du rang de %r ?  Dans le même temps, j'ai formé dix fois
plus de %Rp.  Je devrais peut-être envoyer l'un d'eux, non ?  Et vous,
%p, qu'est-ce que je ferais de vous ?  Ah oui, je me souviens, j'allais vous tuer !"]],
      },
      discourage = {
         "\"Puis-je suggérer un compromis ?  L'or ou les gemmes vous intéressent-ils ?\"",
         "\"Je vous en prie, ne m'obligez pas à vous tuer.\"",
         "\"Des temps bien sombres sont sur nous tous.  N'entendrez-vous pas raison ?\"",
         "\"J'ai connu %l, et heureusement, vous ne lui ressemblez en rien.\"",
         "\"Quel dommage que nous ne nous rencontrions pas en des circonstances plus agréables.\"",
         "\"J'étais jadis comme vous, %p.  Croyez-moi -- notre voie est la meilleure.\"",
         "\"Restez avec moi, et je ferai de vous celui qui veille sur %o.\"",
         "\"Quand vous reviendrez, avec ou sans %o, %l vous fera tuer.\"",
         "\"Ne vous y trompez pas ; je suis prêt à tuer pour défendre %o.\"",
         "\"Je peux vous réunir avec les Deux.  Ah, toutes les histoires que vous pourriez échanger.\"",
      },
      encourage = {
         "\"Vous n'avez pas l'air de comprendre : %o n'est pas ici, alors vous non plus, vous ne devriez pas y être !\"",
         "\"Puisse %d vous maudire et vous donner des doigts de plomb.  Filez !\"",
         "\"On n'a pas toute l'année.  FILEZ !\"",
         "\"Ça vous dirait, un collier de cicatrices ?  Je suis le joaillier qu'il vous faut !\"",
         "\"Sale fainéant.  Je devrais peut-être faire appel à quelqu'un d'autre...\"",
         "\"Je devrais peut-être vous ouvrir le crâne pour voir si mes instructions sont dedans ?\"",
         "\"Ce n'est pas une tâche qu'on peut accomplir dans l'au-delà, vous savez.\"",
         "\"En chaque vivant, il y a un mort qui essaie de sortir, et j'ai votre clé !\"",
         "\"On n'a presque plus de pâtée pour les molosses infernaux, alors bougez-vous donc !\"",
         "\"Vous savez, %o ne viendra pas en sifflant.  Il faut aller le chercher vous-même.\"",
      },
      firsttime = {
         synopsis = "[Vous êtes à Ransmannsby, où vous avez appris le métier.  Trouvez %l.]",
         output = "text",
         text = [[Contre toute attente, vous vous retrouvez à Ransmannsby, où vous avez
appris le métier de voleur.  Vous faites vite le signe de la guilde, en
espérant que vous ET la nouvelle de votre arrivée parviendrez jusqu'au
repaire de votre patron, %l.]],
      },
      goal_first = {
         synopsis = "[Vous sentez la présence de %o.]",
         output = "text",
         text = [[Vous sentez monter en vous un grand élan de courage en percevant la
présence de %o.  À moins que ce ne soit de la peur ?]],
      },
      goal_next = {
         text = "Les poils de votre nuque vous le murmurent -- c'est de la peur.",
      },
      gotit = {
         synopsis = "[Vous ramassez %o et savez que %l ne doit pas l'avoir.]",
         output = "text",
         text = [[Lorsque vous ramassez %o, les poils de votre nuque
tombent.  Vous comprenez aussitôt pourquoi %n était
prêt à mourir pour le soustraire à la convoitise de %l.  D'une
manière ou d'une autre, vous savez que vous devez faire de même.]],
      },
      guardtalk_after = {
         "\"Je me suis bien trompé sur la maison de Dame Tyvefelle ; je m'en suis tiré de justesse, et j'y ai perdu mon crochet.\"",
         "\"Te revoilà ?  Même les Deux ne reviennent plus.\"",
         "\"T'aurais pas un zorkmid pour un vieux coupe-bourse qui veut s'offrir un grog ?\"",
         "\"Fritz a essayé de passer dans l'autre camp, et maintenant, c'est de la pâtée pour molosse infernal.\"",
         "\"Fais gaffe à ce que tu voles, il paraît que le patron a mis au point un truc pour changer les cailloux en bouts de verre sans valeur.\"",
      },
      guardtalk_before = {
         "\"Il paraît que la maison de Dame Tyvefelle est mal gardée.\"",
         "\"Te revoilà ?  Même les Deux ne reviennent plus.\"",
         "\"T'aurais pas un zorkmid pour un vieux coupe-bourse qui veut s'offrir un grog ?\"",
         "\"Fritz a essayé de passer dans l'autre camp, et maintenant, c'est de la pâtée pour molosse infernal.\"",
         "\"Fais gaffe à ce que tu voles, il paraît que le patron a mis au point un truc pour changer les cailloux en bouts de verre sans valeur.\"",
      },
      hasamulet = {
         synopsis = "[Portez l'Amulette sur le Plan Astral et trouvez le temple de %d.]",
         output = "text",
         text = [["Je vois qu'avec vos talents et ma cervelle, nous pourrions régner sur ce monde.

"Tout ce qu'il nous faudrait pour être tout-puissants, c'est que vous
emportiez cette petite babiole jusqu'au Plan Astral.  Là-bas, %d vous
montrera ce qu'il faut en faire.  Une fois que ce sera fait, nous serons
invincibles !"]],
      },
      killed_nemesis = {
         synopsis = "[Avant de mourir, %n vous demande de faire bon usage de %o.]",
         output = "text",
         text = [["Je sais ce que vous pensez, %p.  Il n'est pas trop tard pour faire
bon usage de %o.  Pour l'amour de vos %sp
de la guilde, faites ce qui est juste."

Vous vous asseyez et attendez que la mort vienne chercher %n, puis
vous vous préparez à votre prochaine rencontre avec %l !]],
      },
      leader_first = {
         synopsis = "[Vous devez des cotisations à votre guilde.  Vous pouvez les rembourser si vous êtes à la hauteur du boulot.]",
         output = "text",
         text = [["Eh bien, regardez qui voilà, les gars -- %p revient au bercail.  On
dirait que vous avez pris du retard dans vos cotisations.  Je devrais vous
tuer pour l'exemple, devant ces autres coupe-bourses bons à rien, mais j'ai
une meilleure idée.  Si vous êtes à la hauteur, vous pourriez peut-être
éponger votre ardoise en me rendant un petit service.  Voyons voir si vous
êtes à la hauteur..."]],
      },
      leader_last = {
         synopsis = "[Vous devez partir.]",
         output = "text",
         text = [["Eh bien, les %gp, on dirait que notre ami a oublié qui est le patron
ici.  Notre ami semble croire que ce sont les %rp qui commandent.
Erreur.  ERREUR FATALE !"

Le brusque changement de décor vous empêche d'entendre la fin des
imprécations que profère %l.]],
      },
      leader_next = {
         synopsis = "[Êtes-vous stupide, ou êtes-vous à la hauteur ?]",
         output = "text",
         text = [["Tiens, je ne m'attendais pas à vous revoir.  Ça prouve que soit vous êtes
stupide, soit vous voulez enfin accepter mon offre.  Espérons pour
vous que ce n'est pas la stupidité qui vous ramène."]],
      },
      leader_other = {
         text = [["Me prendriez-vous par hasard pour quelqu'un d'autre que %l ?  Vous devez me
croire aussi stupide que votre conduite.  Je vous préviens : ne mettez pas
ma patience à l'épreuve."]],
      },
      locate_first = {
         text = "Ces satanés petits poils vous disent que vous vous rapprochez de %o.",
      },
      locate_next = {
         text = "Ne voulant pas affronter %l sans avoir volé %o, vous poursuivez votre chemin.",
      },
      nemesis_first = {
         text = "\"Ah !  Vous devez être le... euh, 'héros' qu'envoie %l.  Enchanté de faire votre connaissance.\"",
      },
      nemesis_next = {
         text = "\"Nous nous revoyons.  Je vous en prie, reconsidérez vos actes.\"",
      },
      nemesis_other = {
         synopsis = "[Vous ne pouvez pas faire confiance à %l.]",
         output = "text",
         text = [["Vous avez sûrement appris, %p, qu'on ne peut se fier à aucun des marchés
que %l a conclus.  Je peux vous montrer comment poursuivre
votre quête sans avoir à le croiser de nouveau."]],
      },
      nemesis_wantsit = {
         synopsis = "[%lC ne doit pas avoir %o.]",
         output = "text",
         text = [["Je vous en prie, réfléchissez un instant à ce que vous faites.  Croyez-vous
vraiment que %d voudrait que %l possède
%o ?"]],
      },
      nexttime = {
         text = [[Une fois de plus, vous vous retrouvez à Ransmannsby.  Les doux souvenirs
laissent place à la peur, car vous savez que %l vous attend.]],
      },
      offeredit = {
         synopsis = "[Emportez %o et partez.]",
         output = "text",
         text = [["Ça alors, que le diable m'emporte.  Vous l'avez eu.  Je suis fier de vous,
vous faites honneur au rang de %r !

"Pendant votre absence, je me suis mis à réfléchir : vous et %o,
ensemble, vous pourriez me rapporter plus de trésors que chacun de votre
côté, alors pourquoi ne pas l'emporter avec vous ?  Tout ce que je demande,
c'est une part de tout le butin que vous trouverez.  C'est une meilleure
offre que celle que j'avais faite à %n.

"Mais vous avez vu ce qui est arrivé à %n quand il a refusé.
Ne m'obligez pas, cette fois, à trouver quelqu'un d'autre à envoyer à vos trousses."]],
      },
      offeredit2 = {
         synopsis = "[Prenez %o et procurez-vous l'Amulette.]",
         output = "text",
         text = [[%lC semble tenté d'échanger %o contre
le passe-partout ordinaire que vous devinez dans sa poche, mais, remarquant
votre vigilance, se dégonfle de toute évidence.

"Allez chaparder l'Amulette avant que quelqu'un d'autre ne vous coiffe au
poteau.  %Z sont par le chemin de l'aller, de l'autre côté du portail magique."]],
      },
      othertime = {
         text = [[Vous vous passez la main dans les cheveux, en espérant que les petits
poils de votre nuque restent bien à plat, et vous vous préparez à votre
rencontre avec %l.]],
      },
      posthanks = {
         synopsis = "[Et si vous échangiez %o contre autre chose ?]",
         output = "text",
         text = [["Quel talent pour le vol, n'est-ce pas, %p ?  Puis-je vous proposer un
échange contre %o ?  Regardez autour de vous, tout ce qui se trouve
dans le donjon est à vous, il suffit de demander."]],
      },
   },
   Sam = {
      assignquest = {
         synopsis = "[Vous devez pénétrer dans %i, puis reprendre %o à %n.]",
         output = "text",
         text = [["Domo, %p-san, vous êtes en effet à la hauteur.  Je peux maintenant vous
dire ce que j'attends de vous.

"Le daimyo %n nous a trahis.  Il nous a dérobé
%o et l'a emporté dans son donjon, au plus profond de
%i.

"Si je ne puis montrer %o à l'empereur lorsqu'il viendra
pour la fête, il saura que j'ai failli à mon devoir, et
exigera que je commette seppuku.

"Vous devez pénétrer dans %i et récupérer le
bien de l'empereur.  Faites vite !  L'empereur sera ici pour le
cha-no-yu dans 5 bâtons d'encens.

"Wakarimasu ka ?"]],
      },
      badalign = {
         synopsis = "[Revenez lorsque vous saurez penser selon le chemin %a et agir selon le chemin %a.]",
         output = "text",
         text = [["%p-san, vous feriez mieux de rejoindre les kyokaku.

"Vous avez des talents, mais tant que vous ne saurez pas vous en remettre
au bushido pour savoir quand et comment les employer, vous ne serez pas
samouraï.  Lorsque vous saurez penser selon le chemin %a et agir selon
le chemin %a, revenez."]],
      },
      badlevel = {
         synopsis = "[\"J'ai besoin de quelqu'un du rang de %R pour vaincre %n.  Revenez lorsque vous serez à la hauteur.\"]",
         output = "text",
         text = [["%p-san, vous avez bien appris et fait honneur à votre famille.
Mais il me faut les talents de quelqu'un du rang de %R pour vaincre %n.
Allez chercher des maîtres.  Apprenez ce qu'ils ont appris.  Lorsque vous
serez à la hauteur, revenez me voir."]],
      },
      discourage = {
         "\"Ahh, je rencontre enfin le daimyo des kyokaku !\"",
         "\"Ta mort ne m'apportera aucun honneur.\"",
         "\"Tu sais que je ne puis rengainer mes sabres avant qu'ils aient tué.\"",
         "\"Ta présence ne fait qu'aggraver le déshonneur de %l, qui n'est pas venu en personne.\"",
         "\"Je ferai du thé avec tes cheveux et je le servirai à %l.\"",
         "\"La peur se lit dans tes yeux, lâche !\"",
         "\"Je n'ai jamais entendu parler de toi, %p-san ; ta vie a-t-elle été si indigne ?\"",
         "\"Si tu ne m'obéis pas, tu mourras.\"",
         "\"Agenouille-toi et fais les deux entailles de l'honneur.  Je raconterai à tes %sp ta mort honorable.\"",
         "\"Ton maître était un piètre professeur.  Tu paieras pour les erreurs de ton enseignement.\"",
      },
      encourage = {
         "\"Pour vaincre %n, vous devez surmonter les sept émotions : la haine, l'adoration, la joie, l'anxiété, la colère, le chagrin et la peur.\"",
         "\"Souvenez-vous que votre honneur est mon honneur ; vous agissez en mon nom.\"",
         "\"J'irai au temple brûler de l'encens pour votre retour sans encombre.\"",
         "\"Sayonara.\"",
         "\"La défaite peut être honorable, mais elle ne rapporte rien.\"",
         "\"Votre kami doit être fort pour que vous réussissiez.\"",
         "\"Vous méritez assurément votre rang de %R, mais vous devez à présent mériter celui de samouraï.\"",
         "\"Si vous échouez, %n s'abattra sur le pays comme un tai-fun.\"",
         "\"Si vous suivez vraiment le chemin %a, %d vous écoutera.\"",
         "\"Aiguisez vos sabres et votre esprit pour la tâche qui vous attend.\"",
      },
      firsttime = {
         synopsis = "[La bannière de %n flotte sur la ville.  Qu'est-il arrivé à %l ?]",
         output = "text",
         text = [[Avant même que vos sens ne s'accoutument, vous reconnaissez le kami
des lieux : %H.

Vous parvenez à %x l'étendard de votre teki, %n, qui flotte
au-dessus de la ville.  Comment une telle chose a-t-elle pu se produire ?
Pourquoi des ninjas errent-ils librement ; où sont les samouraïs de votre
daimyo, %l ?

Vous adressez une rapide prière à Izanagi et Izanami, et marchez vers
la ville.]],
      },
      goal_alt = {
         text = "Vous voici de nouveau dans la demeure de %n.",
      },
      goal_first = {
         synopsis = "[Vous entendez les railleries de %n, mais après une prière à %d, vous poursuivez votre chemin.]",
         output = "text",
         text = [[Dans votre esprit, vous entendez les railleries de %n.

Vous devenez semblable au plant de riz et vous ployez jusqu'au sol, en
adressant une prière à %d.  Mais lorsque le vent est passé, vous vous
redressez fièrement.  Remettant votre kami entre les mains du destin,
vous avancez.]],
      },
      goal_next = {
         text = [[Tandis que vous arrivez de nouveau dans la demeure de %n, vos pensées
se tournent uniquement vers %o.]],
      },
      gotit = {
         synopsis = "[Vous sentez le pouvoir de %o et êtes empli d'humilité.]",
         output = "text",
         text = [[Lorsque vous ramassez %o, vous sentez la force de son karma.
Vous comprenez aussitôt pourquoi tant de bons samouraïs ont dû mourir pour le
défendre.  L'humilité vous envahit à l'idée de tenir l'un des artefacts de
la déesse du soleil.]],
      },
      guardtalk_after = {
         "\"Venez, joignez-vous à nous pour fêter cela avec un peu de saké.\"",
         "\"Ikaga desu ka ?\"",
         "\"Vous avez apporté un grand honneur à notre clan et à %l.\"",
         "\"Je vous en prie, %r, asseyez-vous un moment et racontez-nous comment vous avez vaincu les Ninjas.\"",
         "\"%lC est toujours en vie !  Vous nous avez évité de devenir des ronins.\"",
      },
      guardtalk_before = {
         "\"Pour réussir, vous devez avancer comme un papillon porté par le vent.\"",
         "\"Ikaga desu ka ?\"",
         "\"Je crains pour le Pays des Dieux.\"",
         "\"%nC a engagé les Ninjas -- soyez sur vos gardes.\"",
         "\"Si %o n'est pas rendu, nous serons tous des ronins.\"",
      },
      hasamulet = {
         synopsis = "[Portez l'Amulette sur le Plan Astral pour achever votre tâche.]",
         output = "text",
         text = [["Ah, %p-sama.  Vous avez perdu votre peine en revenant ici.
Maintenant que vous êtes en possession de l'Amulette, l'honneur vous oblige
à achever la quête que vous avez entreprise.  Nous aurons tout le temps
pour le saké et les récits lorsque vous aurez terminé.

"Partez maintenant, et puissent nos prières vous pousser comme un vent favorable."]],
      },
      killed_nemesis = {
         synopsis = "[%nC meurt sans honneur.]",
         output = "text",
         text = [[Vos talents de guérisseur vous disent que les blessures de %n sont mortelles.

Vous savez que le bushido vous commande de l'achever et de laisser son kami
mourir dans l'honneur, mais la pensée de tant de samouraïs morts à cause du
déshonneur de cet homme vous empêche de porter le coup de grâce.

Vous ordonnez que sa tête non lavée soit livrée aux corbeaux et son corps
jeté à la mer.]],
      },
      leader_first = {
         synopsis = "[%lC a besoin de quelqu'un pour mener ses samouraïs contre %n.  Êtes-vous à la hauteur ?]",
         output = "text",
         text = [["Ah, %p-san, je suis heureux de vous revoir.  J'ai besoin de quelqu'un qui
puisse mener mes samouraïs contre %n.  Si vous en êtes capable, c'est
vous qui serez cette personne."]],
      },
      leader_last = {
         synopsis = "[Partez et ne revenez pas.]",
         output = "text",
         text = [["Vous n'êtes plus mon samouraï, %p.

"Le hara-kiri vous est refusé.  Il vous est ordonné de vous raser la tête
et de vous faire moine.  Votre fief et votre famille sont confisqués.
Wakarimasu ka ?"]],
      },
      leader_next = {
         text = [["Une fois de plus, %p-san, vous vous agenouillez devant moi.  Êtes-vous enfin
capable d'être mon vassal ?"]],
      },
      leader_other = {
         synopsis = "[Êtes-vous vraiment samouraï ?]",
         output = "text",
         text = [["Vous commencez à mettre mon matsu à l'épreuve, %p-san.
Si vous ne pouvez déterminer ce que j'attends d'un samouraï, comment
pourrais-je compter sur vous pour comprendre ce dont j'ai besoin de la part
d'un samouraï ?"]],
      },
      locate_first = {
         text = [[Instinctivement, vous portez la main à vos sabres.  Vous ne reconnaissez
pas la configuration de ces terres, mais vous savez que vos teki sont partout.]],
      },
      locate_next = {
         text = [[Soulagement : vos %sp, dans %H, ne peuvent voir votre
peur.  Vous vous apprêtez de nouveau à avancer.]],
      },
      nemesis_first = {
         text = [["Ah, ce sera donc toi, %p-san.  Je t'offre le seppuku.
Je serai ton second si tu le souhaites."]],
      },
      nemesis_next = {
         text = [["Je t'ai offert une sortie honorable.  À présent, j'aurai ta
tête, que j'enverrai non lavée à %l."]],
      },
      nemesis_other = {
         text = "\"Après t'avoir expédié, je maudirai ton kami.\"",
      },
      nemesis_wantsit = {
         text = [["Tu as combattu mes samouraïs ; tu dois sûrement savoir que tu
ne pourras pas rapporter %o dans
%H."]],
      },
      nexttime = {
         text = "Une fois de plus, vous voici de retour dans %H.",
      },
      offeredit = {
         synopsis = "[L'empereur souhaite que vous emportiez %o pour reprendre l'Amulette.]",
         output = "text",
         text = [[Tandis que vous vous inclinez devant %l, il vous accueille :

    "Vous avez apporté un grand honneur à votre famille, %p-sama.

    "Pendant votre absence, les conseillers de l'empereur ont découvert
    dans les textes anciens que le karma du samouraï qui cherche à reprendre
    l'Amulette et le karma de %o sont unis
    comme les saisons s'unissent pour former une année.

    "Puisque vous avez fait preuve d'une telle fidélité, l'empereur vous
    demande de vous libérer de vos autres obligations et de poursuivre la
    route sur laquelle le destin a placé vos pas.  Je considérerais comme
    un honneur que vous me permettiez de veiller sur votre maisonnée
    jusqu'à votre retour avec l'Amulette."

Sur ces mots, %l s'incline, et pose son sabre sur
%o.]],
      },
      offeredit2 = {
         synopsis = "[Prenez %o, retournez dans %Z et reprenez l'Amulette.]",
         output = "text",
         text = [[%lC serre %o un instant, puis pose de nouveau
son regard sur vous.

"Le temps est venu de reprendre l'Amulette.  Retournez dans %Z
par le portail magique que vous avez emprunté pour venir ici, afin d'accomplir
la destinée qui vous attend."]],
      },
      othertime = {
         synopsis = "[%HC est menacé par %n.]",
         output = "text",
         text = [[Vous voici de retour dans %H.

Aussitôt, vous sentez un subtil changement dans votre karma.  Vous semblez
savoir que si vous ne réussissez pas dans votre quête, %n aura détruit
le kami des lieux avant votre prochain retour.]],
      },
      posthanks = {
         text = "%lC s'incline.  \"%p-sama, parlez-nous de votre quête de l'Amulette.\"",
      },
   },
   Tou = {
      assignquest = {
         synopsis = "[Entrez dans %i et reprenez %o à %n.]",
         output = "text",
         text = [["Oui, vous avez fait de grands progrès en tant que %c depuis notre dernière rencontre.

"Hélas, les choses ne vont pas très bien ici, à %H.  La Guilde
des Voleurs s'est donné un nouveau chef, %n, qui a offert un dîner pour
rencontrer les autres notables de la ville.  À la porte, l'un de ses
hommes de main a proposé de surveiller mon portefeuille.

"Je sais ce que vous pensez : 'Comme c'est gentil !'  Mais il se trouve que
même à la Guilde des Voleurs, il y a des gens à qui on ne peut pas faire
confiance.  Mon portefeuille contenait -- vous savez que la Monnaie Royale
fait des essais de cartes de crédit, pour qu'on n'ait plus à trimballer des
sacs d'or ?  La plupart des prototypes n'ont pas fonctionné, et ne servent
qu'à crocheter les serrures.  Celle-ci, %o, est
unique en son genre.  En tant que résident d'%H qui dépense le plus et se
fait le plus souvent détrousser, c'est à moi qu'on l'a confiée.  Mais quand
j'ai récupéré mon portefeuille, la Carte avait disparu.  J'ai fait appel au
Patricien, qui m'a répondu que l'affaire relevait de la doctrine du
_detectores custodes_.

"Nous pensons que %n détient la Carte, et qu'il s'est
retranché dans %i.  Grâce au pouvoir de %o, les
Voleurs sont plus audacieux que jamais.  Les citoyens ont peur, et le
tourisme est en baisse.  Il nous faut quelqu'un qui trouve le moyen
d'entrer dans %i et de reprendre la Carte avant qu'il ne reste plus un
seul %c !

"Alors, %p, ça vous dit, un petit Détour ?"]],
      },
      badalign = {
         synopsis = "[Vous ne suivez pas assez fidèlement le chemin %a.  Revenez quand ce sera le cas.]",
         output = "text",
         text = [["Oh, %p, vous avez quitté le chemin %a.  Vous savez que
%d n'aime pas qu'on s'égare.  Vous ne pouvez pas affronter %n
dans cet état !

"Retournez dans le Donjon.  Travaillez sur vous-même.  Revenez quand vous
suivrez vraiment le chemin %a, et on en reparlera."]],
      },
      badlevel = {
         synopsis = "[Revenez lorsque vous aurez atteint le rang de %R.]",
         output = "text",
         text = [["%pC, vous n'avez encore que le rang de %r.  Je ne crois pas que vous
soyez de taille à affronter %n pour l'instant.

"Poursuivez votre Tour.  Explorez.  Prenez des photos.  Apprenez des choses.
Revenez quand vous aurez le rang de %R, et vous aurez peut-être une chance.
%nC sera toujours là quand vous serez de taille à l'affronter."]],
      },
      discourage = {
         "\"J'ai salement battu %l, et je vais te battre encore plus salement, %p.\"",
         "\"Même %d ne peut rien pour toi ici.\"",
         "\"Implore ma pitié, et peut-être que j'envisagerai de songer à y aller doucement avec toi.\"",
         "\"T'aurais pu faire un bon Voleur si tu suivais pas tant le chemin %a.\"",
         "\"Écris tes cartes postales, %p.  Ton Tour s'arrête ici.\"",
         "\"%lC pouvait pas m'envoyer un meilleur %c à tabasser ?\"",
         "\"Avec %o, je vais faire une note qui ruinera %H.\"",
         "\"Personne ne vient à bout de %n.  PERSONNE !\"",
         "\"Pauvre %l, si naïf.  Il t'a envoyé me demander gentiment de rendre %o ?\"",
         "\"Comment ça s'écrit, %p ?  Je veux être sûr que ta tombe soit sans faute.\"",
      },
      encourage = {
         "\"Qui aurait cru que %n serait un tel criminel !\"",
         "\"Attention aux pièges sur le chemin de %i.  Certains datent des Guerres des Mages !\"",
         "\"Reprenez %o, et j'espère pouvoir annuler les transactions qu'a faites %n.\"",
         "\"Si vous êtes dans l'impasse, essayez de prier %d.  Les dieux sont capricieux, mais %dh aide parfois les %cp.\"",
         "\"Vous devez vaincre %n, sinon %nh vous suivra jusqu'ici !\"",
         "\"Si vous arrivez à reprendre %o à %n, ça pourrait vous aider à %ni battre.\"",
         "\"Faites vite, %p !  Nous comptons tous sur vous.\"",
         "\"Je sais que vous ne me laisserez pas tomber, %p !\"",
         "\"Ne quittez pas le chemin %a.  Souvenez-vous, les %cp ne prennent pas parti, ils prennent des photos.\"",
         "\"Si quelqu'un vous cause des ennuis, souvenez-vous : un sourire peut mener loin.\"",
      },
      firsttime = {
         synopsis = "[Vous voici de retour à %H, mais tout est d'un calme inquiétant.]",
         output = "text",
         text = [[En reprenant vos esprits, vous retrouvez les paysages familiers
d'%H.  Vous poussez un soupir de soulagement, en respirant l'air pollué.

Mais ce n'est pas l'%H dont vous vous souvenez.  Les rues sont... calmes ?
Des portes se ferment, des rideaux frémissent, et quelques habitants
filent nerveusement, mais vous n'entendez pas les colporteurs vanter leurs
tourtes à la viande filandreuse, ni les péniches se frayer un chemin à
coups de proue le long du fleuve.  Une bouffée de chou pourri laisse penser
que les champs hors de la ville ne sont pas correctement entretenus.

Quelque chose ne tourne pas rond.  Avec un peu de chance, ce n'est qu'une
sorte de malentendu.  %lC arrangera tout ça, quand vous l'aurez trouvé.]],
      },
      goal_alt = {
         text = "Vous voici de retour dans le repaire où se terre %n.",
      },
      goal_first = {
         text = "Vous sentez la présence de %o tout près.",
      },
      goal_next = {
         text = [[Cette fois, vous trouverez %o, c'est sûr !]],
      },
      gotit = {
         synopsis = "[Vous ressentez un grand contentement en ramassant %o.  Reste à la rapporter à %l.]",
         output = "text",
         text = [[Vous ressentez un profond contentement en ramassant %o.
Voilà qui fera très bien l'affaire !  Vous feriez mieux de la rapporter à %l
au plus vite.]],
      },
      guardtalk_after = {
         "\"La Géhenne pour 5 zorkmids par jour -- plutôt 500 par jour, si tu veux mon avis.\"",
         "\"Tu sais où je pourrais trouver de jolies cartes postales des Mines des Gnomes ?\"",
         "\"Tu as essayé les toilettes bizarres ?\"",
         "\"Si tu restes un peu, je te montrerai les photos de mon dernier voyage.\"",
         "\"Tu m'as rapporté des souvenirs ?\"",
      },
      guardtalk_before = {
         "\"La Géhenne pour 5 zorkmids par jour -- plutôt 500 par jour, si tu veux mon avis.\"",
         "\"Tu sais où je pourrais trouver de jolies cartes postales des Mines des Gnomes ?\"",
         "\"Tu as essayé les toilettes bizarres ?\"",
         "\"Ne loge pas à l'Auberge, il paraît que la cuisine est infecte et qu'il y a des rats.\"",
         "\"On m'avait dit que c'était la basse saison !\"",
      },
      hasamulet = {
         synopsis = "[Vous avez l'Amulette.  Portez-la sur le Plan Astral pour achever votre tâche.]",
         output = "text",
         text = [["%pC !  Et vous avez trouvé l'Amulette de Yendor ?  Incroyable !

"Mais vous ne pouvez pas la garder ici, à %H.  Elle ne serait pas en sécurité !

"D'ailleurs, l'Amulette n'est pas pour nous autres %cp, elle est pour %d.
J'ai entendu dire que %d a un temple sur le Plan Astral, qui est d'ailleurs
un endroit magnifique à visiter à cette époque de l'année.  Trouvez le
chemin, puis offrez-la à %d sur son grand autel.  Quelle joie ce sera pour
%d de l'avoir enfin à soi !

Bon voyage, %p."]],
      },
      killed_nemesis = {
         synopsis = "[%nC vous maudit en mourant.]",
         output = "text",
         text = [[%nC s'effondre, le corps plié en deux.  Cherchant son souffle, %nh
parvient à proférer une malédiction :

"Tu t'en tireras pas comme ça, %p !  Mes gars te poursuivront où que
tu ailles, et reprendront %o pour les Voleurs."
Et il n'est plus.

Un instant, une silhouette en manteau noir vacille au coin de votre œil, et
vous avez cru entendre des bruits de sabots.]],
      },
      leader_first = {
         synopsis = "[Quelqu'un doit vaincre %n.  Êtes-vous à la hauteur ?]",
         output = "text",
         text = [["Aloha, %p, je suis si content de vous voir !  Comment se passe votre Tour ?

Écoutez, je suis vraiment désolé d'écourter vos vacances, mais nous avons
un petit problème ici.  Quelqu'un doit faire quelque chose au sujet de
%n !  Et si ce quelqu'un, c'était vous ?  Laissez-moi vous regarder..."]],
      },
      leader_last = {
         synopsis = "[Quittez %H et n'y revenez jamais.]",
         output = "text",
         text = [["Oh non, %p.  Vous n'avez rien d'un %c.  Quittez %H, et ne
remontrez jamais votre visage ici.  Votre adhésion à la Guilde des Intrus
de la ville est révoquée.  Aloha."]],
      },
      leader_next = {
         text = "\"Re-aloha, %p.  Vous vous sentez à la hauteur, cette fois ?\"",
      },
      leader_other = {
         text = "\"J'espère que vous avez fait vos préparatifs, cette fois.\"",
      },
      locate_first = {
         synopsis = "[Vous parvenez à %x l'œuvre des larbins qu'emploie %n.]",
         output = "text",
         text = [[Ce n'est pas le quartier le plus accueillant de la ville.  Il y a des
marques de Voleurs partout, et les larbins qu'emploie %n rasent les murs
des ruelles.]],
      },
      locate_next = {
         text = "Vous savez que cette fois, vous devrez détruire %n.",
      },
      nemesis_first = {
         synopsis = "[Quelqu'un du rang de %r ne me vaincra pas.]",
         output = "text",
         text = [["Alors, %p, %l croit que tu vas me prendre
%o.  C'est pas mignon, ça ?

"Il m'envoie quelqu'un du rang de %r pour me vaincre.  Moi !  Quand ce sera
fini, je te coulerai les pieds dans le béton et je te jetterai dans le fleuve !"]],
      },
      nemesis_next = {
         text = [["Je t'ai laissé filer la dernière fois, %p.  Cette
fois, je vais te détruire."]],
      },
      nemesis_other = {
         synopsis = "[Fuis, ou tu souffriras atrocement.]",
         output = "text",
         text = [["Ces rencontres commencent à m'ennuyer.  Tu empiètes sur mon précieux
temps de magouilles.

"Si tu fiches pas le camp tout de suite, je vais te faire souffrir si fort
que %l se sentira coupable de t'avoir envoyé ici."]],
      },
      nemesis_wantsit = {
         synopsis = "[\"Rends-moi %o et nous régnerons sur %H.\"]",
         output = "text",
         text = [["Pauvre andouille.  Tu sais même pas te servir de
%o.  Rends-la-moi et je t'apprendrai,
et ensemble, nous pourrons régner sur %H.  Et on le pourra vraiment !
Je dis pas ça juste pour que tu me rendes %o,
je le jure.  Parole de Voleur !

"Mais fais-le maintenant, parce que ma patience a des limites."]],
      },
      nexttime = {
         text = "Une fois de plus, vous voici de retour à %H.",
      },
      offeredit = {
         synopsis = "[Prenez %o et, avec l'aide de %d, reprenez l'Amulette.]",
         output = "text",
         text = [[%lC prend %o et
la fait plier dans sa main, puis lève les yeux et sourit.

"%pC, pendant votre absence, %d m'est apparu et m'a suggéré de
vous transférer %o.  Emportez-la avec vous dans
votre quête de l'Amulette de Yendor, et vous voyagerez en bonne compagnie.

"Souvenez-vous -- il y a des choses que l'or peut acheter.  Pour tout le
reste, il y a %o."]],
      },
      offeredit2 = {
         synopsis = "[Gardez %o et retournez dans %Z par le portail.]",
         output = "text",
         text = [["%oC est à vous.  Ne partez jamais sans elle !
%Z sont juste de l'autre côté du portail magique que vous avez
emprunté pour venir ici."]],
      },
      othertime = {
         text = [[Vous voici de retour à %H.  Tout semble si mort que vous
avez l'impression qu'il ne restera bientôt plus un seul %c ici.]],
      },
      posthanks = {
         text = [["Mais regardez-vous, %p !  Racontez-moi tout de votre Tour.  Avez-vous
déjà trouvé l'Amulette de Yendor ?"]],
      },
   },
   Val = {
      assignquest = {
         synopsis = "[Trouvez %i ; vainquez %n ; revenez avec %o.]",
         output = "text",
         text = [["Ce n'est pas clair, %p, car ma vision est limitée sans notre relique.
Mais il est désormais probable que vous puissiez vaincre %n et reprendre
%o.

"Il y a peu, %n et ses sbires ont attaqué ce lieu.  Ils ont
ouvert les immenses cheminées volcaniques que vous pouvez %x autour de la
colline, puis ont attaqué.  Je savais que cela devait advenir, et j'avais
demandé à %d une troupe de %gp pour aider à défendre ce lieu.  Les quelques
guerriers que vous pouvez %x ici sont les plus puissants du Valhalla, et
sont tout ce qui reste des cent que %d a envoyés.

"Malgré la grande et glorieuse bataille que nous avons livrée, %n est
finalement parvenu à dérober %o.  Cela a rompu l'équilibre de l'univers,
et si on ne me le rend pas, %n pourrait déclencher le Ragnarök.

"Vous devez trouver l'entrée menant vers %i.  Descendez
à partir de là et vous trouverez le repaire de %n.  Vainquez-le et
rapportez-moi %o."]],
      },
      badalign = {
         synopsis = "[Vous avez quitté le chemin %a.  Revenez après vous être purifiée.]",
         output = "text",
         text = [["NON !  C'est terrible.  Je vous vois devenir l'alliée de %n, et
mener ses armées lors des dernières grandes batailles.  Cela ne doit pas
advenir !  Vous avez quitté le chemin %a.  Vous devez vous purifier,
et ne revenir ici que lorsque vous aurez retrouvé un état de pureté."]],
      },
      badlevel = {
         synopsis = "[Revenez lorsque vous aurez atteint le rang de %R.]",
         output = "text",
         text = [["Je vous vois combattre %n, %p.  Mais vous n'êtes pas prête, et
vous périrez de la main de %n si vous poursuivez.  Non.  Cela ne va pas.
Retournez de par le monde, et acquérez davantage d'expérience dans l'art
de la guerre.  Ce n'est qu'une fois revenue avec le rang de %R que vous
pourrez vaincre %n."]],
      },
      discourage = {
         "\"Je suis ta mort, %c.\"",
         "\"Tu ne peux l'emporter, %r.  J'ai prévu chacun de tes gestes.\"",
         "\"Une fois débarrassé de toi, je n'aurai plus qu'à prendre le Valhalla.\"",
         "\"J'ai tué des dizaines des meilleurs guerriers de %d en m'emparant de %o.  Crois-tu vraiment qu'une seule %c puisse me tenir tête ?\"",
         "\"Qui portera les âmes des %cp jusqu'au Valhalla, %r ?\"",
         "\"Non, %d ne peut rien pour toi ici.\"",
         "\"Bel instrument de %d que tu fais, %p.  Tu n'es qu'une mauviette !\"",
         "\"Jamais je n'ai vu de %c aussi maladroite au combat.\"",
         "\"Tu vas mourir, petite %s.\"",
         "\"Ton corps, je le détruis maintenant ; ton âme, quand mes hordes submergeront le Valhalla !\"",
      },
      encourage = {
         "\"Allez avec la bénédiction de %d.\"",
         "\"Invoquez %d lorsque vous serez dans le besoin.\"",
         "\"Servez-vous de %o si vous le pouvez.  Il vous protégera.\"",
         "\"Le froid magique est très efficace contre %n.\"",
         "\"Pour affronter %n, vous devrez être immunisée contre le feu.\"",
         "\"Puisse %d fortifier votre bras armé.\"",
         "\"Fiez-vous à %d.  Il ne vous abandonnera pas.\"",
         "\"La venue du Ragnarök devient plus probable à chaque instant.  Vous devez vous hâter, %p.\"",
         "\"Si %n parvient à maîtriser %o, il sera assez puissant pour affronter %d bien plus tôt que le destin ne l'a prévu.  Cela ne doit pas être !\"",
         "\"Souvenez-vous de votre entraînement, %p.  Vous pouvez réussir.\"",
      },
      firsttime = {
         synopsis = "[Vous arrivez en contrebas de %H.  Quelque chose ne va pas ; il y a de la lave.]",
         output = "text",
         text = [[Vous vous matérialisez au pied d'une colline enneigée.  Au sommet de la
colline se dresse un lieu que vous connaissez bien, %H.  Vous comprenez
aussitôt que quelque chose ne va pas du tout !

Par endroits, la neige et la glace ont fondu en mares d'eau fumante.
Des fumerolles et des mares de lave bouillonnante entourent la colline.
L'air charrie une puanteur de soufre, et vous pouvez %x des créatures
qui ne devraient pas pouvoir vivre dans un tel milieu avancer vers vous.]],
      },
      goal_first = {
         synopsis = "[C'est le repaire de %n.]",
         output = "text",
         text = [[À travers des nuages de gaz sulfureux, vous parvenez à %x une palissade
de roche entourée d'une douve de lave bouillonnante.  Vous vous souvenez de
la description que %l vous en a faite.  C'est le repaire de %n.]],
      },
      goal_next = {
         text = "Une fois de plus, vous voici en vue du repaire de %n.",
      },
      gotit = {
         synopsis = "[Vous devez rapporter %o à %l.]",
         output = "text",
         text = [[Lorsque vous ramassez %o, votre esprit s'emplit soudain d'images,
et vous percevez toutes les possibilités de chaque choix que vous pourriez
faire.  Tandis que vous commencez à maîtriser et canaliser vos pensées,
vous comprenez que vous devez rapporter immédiatement %o à %l.]],
      },
      guardtalk_after = {
         "\"Salut à toi, brave %c, heureuse rencontre.\"",
         "\"Que %d guide tes pas, %p.\"",
         "\"%lC nous a dit que tu avais réussi !\"",
         "\"Tu as repris %o juste à temps, %p.\"",
         "\"Gloire à %d, qui nous a rendu %o.\"",
      },
      guardtalk_before = {
         "\"Salut à toi, brave %c, heureuse rencontre.\"",
         "\"Que %d guide tes pas, %p.\"",
         "\"%lC s'affaiblit.  Sans %o, sa prescience s'obscurcit.\"",
         "\"Tu dois te hâter, %p, sinon le Ragnarök pourrait bien survenir.\"",
         "\"Je réglerais bien son compte à cet immonde %n moi-même, mais %d me l'interdit.\"",
      },
      hasamulet = {
         synopsis = "[Portez l'Amulette jusqu'au temple de %d sur le Plan Astral et offrez-la.]",
         output = "text",
         text = [["Excellent, %p.  Je vois que vous avez repris l'Amulette !

"Vous devez porter l'Amulette jusqu'au Grand Temple de %d, sur le Plan
Astral.  Là, vous devrez offrir l'Amulette à %d.

"Partez maintenant, %S de notre peuple.  Je ne puis vous prédire votre
destin, car le pouvoir de l'Amulette interfère avec le mien.  J'espère que
vous réussirez."]],
      },
      killed_nemesis = {
         synopsis = "[%nC meurt.]",
         output = "text",
         text = [[Une expression de surprise et d'horreur apparaît sur le visage de %n.

    "Non !!!  %oC m'a menti !  J'ai été trompé !"

Soudain, %n se prend la tête à deux mains et hurle de douleur, puis meurt.]],
      },
      leader_first = {
         synopsis = "[Nous avons besoin de votre aide.  Êtes-vous à la hauteur ?]",
         output = "text",
         text = [["Ah, %p, %S de notre peuple.  Vous voici enfin de retour dans %H.
Nous avons cruellement besoin de votre aide, mais je dois déterminer si
vous êtes déjà prête pour une telle entreprise.

"Laissez-moi lire votre destin..."]],
      },
      leader_last = {
         synopsis = "[\"Quittez ma présence et ne revenez jamais.\"]",
         output = "text",
         text = [["Non, %p.  Votre destin est scellé.  Je dois chercher une autre
championne.  Quittez ma présence, et ne revenez jamais.  Sachez-le : jamais
vous ne réussirez en cette vie, et le Valhalla vous est refusé."]],
      },
      leader_next = {
         text = [["Laissez-moi lire votre avenir à présent, %p ; peut-être êtes-vous parvenue
à le changer suffisamment..."]],
      },
      leader_other = {
         text = [["De nouveau, je vais lire votre destin, %S de notre peuple.  Espérons
toutes deux que vous avez suffisamment changé pour être prête à
accomplir cette tâche..."]],
      },
      locate_first = {
         synopsis = "[Voici l'entrée menant vers %i.]",
         output = "text",
         text = [[La glace et la neige laissent place au fond d'une vallée.  Devant vous,
vous pouvez %x une immense colline ronde entourée de mares de lave.  Voici
donc l'entrée menant vers %i.  On dirait toutefois que vous
n'y entrerez pas sans combattre.]],
      },
      locate_next = {
         text = "Une fois de plus, vous vous tenez devant l'entrée menant vers %i.",
      },
      nemesis_first = {
         synopsis = "[\"%oC m'a montré que je dois te tuer.\"]",
         output = "text",
         text = [["Ainsi !  %lC m'envoie enfin une %c pour me défier !

"Je pensais que maîtriser %o me permettrait de défier
%d, mais il m'a montré que je dois d'abord te tuer !  Alors viens, petite
%s.  Une fois que je t'aurai vaincue, je pourrai enfin engager la bataille
finale contre %d."]],
      },
      nemesis_next = {
         text = "\"Tu me défies encore, %r.  Bien.  Je vais te tuer à présent.\"",
      },
      nemesis_other = {
         text = "\"N'as-tu donc pas encore compris ?  Tu ne peux vaincre %n !\"",
      },
      nemesis_wantsit = {
         text = "\"Je vais te tuer, %c, et arracher %o de tes mains broyées.\"",
      },
      nexttime = {
         text = "Une fois de plus, vous voici près de la demeure de %l.",
      },
      offeredit = {
         synopsis = "[Prenez %o.  Cherchez l'Amulette.]",
         output = "text",
         text = [[Tandis que vous approchez, %l se lève et touche %o.

"Vous pouvez emporter %o avec vous, %p.  Je lui ai retiré
le pouvoir de prédire l'avenir, car nul mortel ne devrait posséder
un tel pouvoir.  Ses autres capacités, en revanche, sont à votre disposition.

"Vous devez à présent partir, au nom de %d, à la recherche de l'Amulette
de Yendor.  Que vos pas soient guidés par %d, %S de notre peuple."]],
      },
      offeredit2 = {
         synopsis = "[C'est à vous de veiller sur %o désormais.  Repassez le portail et trouvez l'Amulette.]",
         output = "text",
         text = [["Attention, %p !  %oC pourrait se briser, et ce serait
une perte tragique.  C'est à vous de veiller sur lui désormais, et l'heure
est venue de reprendre votre quête de l'Amulette.  %Z attendent votre
retour par le portail magique que vous avez emprunté pour venir ici."]],
      },
      othertime = {
         text = [[De nouveau, vous vous matérialisez près de la demeure de %l.  Un
sentiment tenace vous dit que c'est peut-être la dernière fois que vous
venez ici.]],
      },
      posthanks = {
         text = [["Salutations, %p.  Je n'ai pu prêter à votre quête de l'Amulette
autant d'attention que je l'aurais souhaité.  Comment vous en sortez-vous ?"]],
      },
   },
   Wiz = {
      assignquest = {
         synopsis = "[Rendez-vous dans %i ; terrassez %n ; revenez avec %o.]",
         output = "text",
         text = [["Oui, %p, vous êtes véritablement à la hauteur de cette terrible tâche.
Écoutez attentivement, car ce que je vais vous dire est d'une importance
capitale.

"Depuis que vous nous avez quittés pour parfaire vos talents de par le
monde, nous avons été attaqués à l'improviste par les forces de %n.
Comme vous le savez, nous pensions que %n avait péri à la fin de l'âge
dernier, mais hélas, il n'en était rien.

"%nC a lancé contre nous une armée d'abominations.  Parmi elles se trouvait
un serviteur, dépourvu d'esprit et ensorcelé, si bien que, dans la confusion,
il a pu franchir nos défenses.  Hélas, cette créature a dérobé
%o, et je crains qu'elle ne l'ait remis à %n.

"Au fil des ans, j'avais tissé l'essentiel de mon pouvoir dans cette amulette,
si bien que, sans elle, je ne suis plus que l'ombre de moi-même, et je
crains de périr bientôt.

"Vous devez vous rendre dans %i, et dans ses souterrains,
trouver et terrasser %n, puis me rapporter %o.

"Partez maintenant, avec %d, et accomplissez cette quête avant qu'il ne
soit trop tard."]],
      },
      badalign = {
         synopsis = "[Partez ; revenez lorsque vous serez digne de %d.]",
         output = "text",
         text = [["Vous me stupéfiez, %p !  Combien de fois vous ai-je dit que la voie
du mage est exigeante ?  Il faut user du monde avec soin, de peur de le
laisser en ruine et de faciliter la tâche de %n.

"Vous devez repartir et prouver votre valeur.  Ne revenez pas avant d'être
véritablement à la hauteur de cette quête.  Puisse %d vous guider dans
cette tâche."]],
      },
      badlevel = {
         synopsis = "[Partez ; revenez lorsque vous aurez atteint le rang de %R.]",
         output = "text",
         text = [["Hélas, %p, vous n'avez pas encore fait la preuve de vos talents
en matière de sorts.  Avec votre rang de %r, l'épreuve qui
vous attend aurait sûrement raison de vous.  Partez maintenant,
élargissez vos horizons, et revenez lorsque vous aurez acquis la renommée
que confère le rang de %R."]],
      },
      discourage = {
         "\"Tes pouvoirs dérisoires ne font pas le poids face à moi, imbécile !\"",
         "\"Après ta défaite, ton tourment durera mille ans.\"",
         "\"Après ta chute, %p, je dévorerai %l en guise de dessert !\"",
         "\"Vas-tu enfin implorer ma pitié ?  Je pourrais me montrer clément...\"",
         "\"Ton âme rejoindra la multitude asservie que je commande !\"",
         "\"Ton manque de volonté est flagrant, et tu en mourras.\"",
         "\"Ta foi en %d ne te sert à rien !  Viens, soumets-toi à moi !\"",
         "\"Le rang de %r n'est rien comparé à mon talent !\"",
         "\"Ainsi, c'est toi le meilleur espoir de %l ?  Comme c'est cocasse.\"",
         "\"Sens ma puissance, %c !  Ma victoire est imminente !\"",
      },
      encourage = {
         "\"Prenez garde, car %n est immunisé contre la plupart des attaques magiques.\"",
         "\"Pour entrer dans %i, vous devrez franchir bien des pièges.\"",
         "\"%nC est peut-être vulnérable aux attaques physiques.\"",
         "\"%d vous viendra en aide lorsque vous l'appellerez.\"",
         "\"Vous devez détruire %n entièrement.  Sinon, il vous poursuivra.\"",
         "\"%oC est un puissant artefact.  Grâce à lui, vous pourrez détruire %n.\"",
         "\"Partez avec la bénédiction de %d.\"",
         "\"Je demanderai à mes %gp de guetter votre retour.\"",
         "\"N'hésitez pas à prendre dans ce coffre tout objet qui pourrait vous aider.\"",
         "\"Vous saurez quand %o sera proche.  Avancez avec prudence !\"",
      },
      firsttime = {
         synopsis = "[Vous voici à la tour de %l, mais quelque chose ne va pas du tout.]",
         output = "text",
         text = [[Vous vous retrouvez soudain dans un décor familier.  Vous remarquez
non loin ce qui ressemble à une grande bâtisse de pierre trapue.  Attendez !
On dirait la tour de votre ancien professeur, %l.

Pourtant, les choses ne sont plus comme lors de votre dernière visite.  Des
brumes et des zones d'obscurité inexpliquée entourent la tour.  Quelque chose
bouge dans les ombres.

Votre professeur ne permettrait jamais à des formes aussi inesthétiques
d'entourer la tour...  à moins que quelque chose n'aille terriblement mal !]],
      },
      goal_alt = {
         text = "Vous voici de retour dans le repaire où se terre %n.",
      },
      goal_first = {
         text = "Vous sentez la présence de votre mentor ; peut-être %o est-il tout proche.",
      },
      goal_next = {
         text = "L'aura de %o picote aux confins de votre perception.",
      },
      gotit = {
         synopsis = "[Vous sentez le pouvoir de %o et savez que vous devez le rapporter à %l.]",
         output = "text",
         text = [[Lorsque vous touchez %o, son pouvoir réconfortant vous
emplit d'une énergie nouvelle.  Vous avez l'impression de percevoir les
pensées d'autrui qui circulent à travers lui.  Bien que vous brûliez de
porter %o et d'attaquer le Sorcier de Yendor, vous savez que vous devez
le rendre à sa légitime propriétaire, %l.]],
      },
      guardtalk_after = {
         "\"J'ai un peu d'œil de triton à échanger, tu n'aurais pas un dard d'orvet en trop ?\"",
         "\"Le portail magique semble désormais devoir rester stable pour un bon moment.\"",
         "\"As-tu remarqué combien %l est plus forte depuis qu'on a repris %o ?\"",
         "\"Grâces soient rendues à %d !  Nous n'étions pas certains que tu vaincrais %n.\"",
         "\"Moi aussi, je vais partir de par le monde, car %n n'était qu'un mal parmi tant d'autres à vaincre.\"",
      },
      guardtalk_before = {
         "\"Tu n'aurais pas un peu d'œil de triton dans ce sac bien trop rempli, %s ?\"",
         "\"Ah, le sort de création du portail magique a fonctionné.  Formidable !\"",
         "\"Vite !  %lC risque de ne pas survivre à l'incantation du sort de portail !\"",
         "\"Les sorts de %n étaient tout simplement trop puissants pour que nous puissions y résister.\"",
         "\"Moi aussi, je vais partir de par le monde, car %n n'est qu'un mal parmi tant d'autres à vaincre.\"",
      },
      hasamulet = {
         synopsis = "[Portez l'Amulette jusqu'à l'autel de %d sur le Plan Astral.]",
         output = "text",
         text = [["Félicitations, %p.  J'ai toujours su que si quelqu'un pouvait réussir
à vaincre le Sorcier de Yendor et ses sbires, ce serait vous.

"Partez maintenant, et portez l'Amulette jusqu'au Plan Astral.  Une fois
là-bas, présentez l'Amulette sur l'autel de %d.  En chemin, vous
traverserez les quatre Plans Élémentaires.  Ces plans ne ressemblent à rien
de ce que vous avez connu jusqu'ici, alors préparez-vous !

"C'est pour cela que vous avez vu le jour, %s !  Je suis très fière de vous."]],
      },
      killed_nemesis = {
         synopsis = "[%nC vous maudit en mourant.]",
         output = "text",
         text = [[%nC, dont le corps commence à se ratatiner, croasse :

    "Je hanterai ta progression jusqu'à la fin des temps.  Mille
    malédictions sur toi et sur %l."

Puis le corps éclate en un nuage de poussière suffocante, et s'envole.]],
      },
      leader_first = {
         synopsis = "[Vous avez fait bien du chemin, mais êtes-vous à la hauteur de la tâche que je vous réserve ?]",
         output = "text",
         text = [["Approchez, %p, car ma voix faiblit avec l'âge.
Oui, je vois que vous avez fait bien du chemin depuis votre départ
de par le monde, quittant l'abri sûr de cette tour.  Cependant, je dois
d'abord déterminer si vous possédez tous les talents nécessaires pour
accomplir la tâche que je vous réserve."]],
      },
      leader_last = {
         synopsis = "[\"Hors d'ici !\"]",
         output = "text",
         text = [["Quelle folie, %p !  Pourquoi ai-je gâché toutes ces années à vous enseigner
les arts ésotériques ?  Hors d'ici !  Je trouverai quelqu'un d'autre."]],
      },
      leader_next = {
         text = "\"Eh bien, %p, vous voici de retour.  Peut-être êtes-vous désormais à la hauteur...\"",
      },
      leader_other = {
         text = [["Cela devient fastidieux, %p, mais la persévérance est la marque d'un vrai mage.
J'espère bien que vous êtes vraiment à la hauteur cette fois !"]],
      },
      locate_first = {
         text = "Des volutes de brume tourbillonnent alentour.  Vous sentez que le repaire où se terre %n est proche.",
      },
      locate_next = {
         text = "Vous pensez pouvoir de nouveau envahir %i.",
      },
      nemesis_first = {
         synopsis = "[\"Ta destruction devrait être divertissante.\"]",
         output = "text",
         text = [["Ah, je te reconnais, %p.  Ainsi, %l te charge de me voler
%o, hmmm ?  Eh bien, il faut être fou pour envoyer contre moi
un esprit aussi chétif.

"Ta destruction, toutefois, devrait être divertissante.  À la fin, tu
me supplieras de te tuer !"]],
      },
      nemesis_next = {
         synopsis = "[\"Ton âme sera bientôt à mes ordres.\"]",
         output = "text",
         text = [["Comme c'est gentil à toi de revenir, %p !  Notre dernière rencontre m'a
beaucoup plu.  As-tu encore faim de souffrance ?

"Viens !  Ton âme, comme %o, sera bientôt à mes ordres."]],
      },
      nemesis_other = {
         text = [["Je ne doute pas que ta persévérance fera l'objet d'innombrables
ballades, mais tu ne seras plus là pour les entendre, je le crains !"]],
      },
      nemesis_wantsit = {
         text = [["Voleur !  %oC m'appartient, désormais.  Je donnerai
ta chair vivante en pâture à mes sbires."]],
      },
      nexttime = {
         text = "Une fois de plus, vous voici de retour dans %H.",
      },
      offeredit = {
         synopsis = "[Emportez %o dans votre quête de l'Amulette.]",
         output = "text",
         text = [[%lC remarque %o en votre possession,
vous adresse un sourire radieux et déclare :

    "Je savais que vous pourriez vaincre %n et reprendre
    %o.  Nous n'oublierons jamais ce
    vaillant service.

    "Emportez-le avec vous dans votre quête de l'Amulette de Yendor.
    Je sens qu'il s'est déjà accordé à vous.

    "Puisse %d vous guider dans votre quête, et vous garder de tout mal."]],
      },
      offeredit2 = {
         synopsis = "[Gardez %o, retournez dans %Z par le portail ; trouvez l'autre Amulette.]",
         output = "text",
         text = [["C'est à vous de veiller sur %o désormais.  Il est temps de
reprendre l'/autre/ Amulette.  %Z attendent votre retour par
le portail magique que vous avez emprunté pour venir ici."]],
      },
      othertime = {
         text = [[Vous voici de retour dans %H.
Vous avez l'étrange sentiment que vous venez peut-être ici pour la dernière fois.]],
      },
      posthanks = {
         text = [["Approchez, %S de notre ordre, et racontez-moi vos aventures.
Alors, avez-vous réussi votre quête de l'Amulette de Yendor ?"]],
      },
   },
}
