/* NetHack 5.0	timeout.c	$NHDT-Date: 1781973070 2026/06/20 16:31:10 $  $NHDT-Branch: NetHack-5.0 $:$NHDT-Revision: 1.212 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2018. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

#ifndef SFCTOOL
staticfn void stoned_dialogue(void);
staticfn void vomiting_dialogue(void);
staticfn void sleep_dialogue(void);
staticfn void choke_dialogue(void);
staticfn void levitation_dialogue(void);
staticfn void slime_dialogue(void);
staticfn void slimed_to_death(struct kinfo *) NO_NNARGS;
staticfn void sickness_dialogue(void);
staticfn void phaze_dialogue(void);
staticfn void region_dialogue(void);
staticfn void done_timeout(int, int);
staticfn void slip_or_trip(void);
staticfn void see_lamp_flicker(struct obj *, const char *) NONNULLPTRS;
staticfn void lantern_message(struct obj *) NONNULLARG1;
staticfn void cleanup_burn(ANY_P *, long) NONNULLARG1;

/* used by wizard mode #timeout and #wizintrinsic; order by 'interest'
   for timeout countdown, where most won't occur in normal play */
static const struct propname {
    int prop_num;
    const char *prop_name;
} propertynames[] = {
    { INVULNERABLE, "invulnérable" },
    { STONED, "en pétrification" },
    { SLIMED, "en transformation en limon" },
    { STRANGLED, "étranglement" },
    { SICK, "mortellement malade" },
    { STUNNED, "étourdi" },
    { CONFUSION, "confus" },
    { HALLUC, "hallucinations" },
    { BLINDED, "aveuglé" },
    { DEAF, "surdité" },
    { VOMITING, "vomissements" },
    { GLIB, "doigts glissants" },
    { WOUNDED_LEGS, "jambes blessées" },
    { SLEEPY, "somnolence" },
    { TELEPORT, "téléportation" },
    { POLYMORPH, "polymorphie" },
    { LEVITATION, "lévitation" },
    { FAST, "très rapide" }, /* timed 'FAST' is very fast */
    { CLAIRVOYANT, "clairvoyance" },
    { DETECT_MONSTERS, "détection des monstres" },
    { SEE_INVIS, "voir l'invisible" },
    { INVIS, "invisible" },
    /* temporary acid resistance and stone resistance can come from eating */
    { ACID_RES, "résistance à l'acide" },
    { STONE_RES, "résistance à la pétrification" },
    /* timed displacement is possible via eating a displacer beast corpse */
    { DISPLACED, "déplacement" },
    /* timed pass-walls is a potential prayer result if surrounded by stone
       with nowhere to be safely teleported to */
    { PASSES_WALLS, "passe-muraille" },
    /* likewise for magical breathing vs poison gas regions */
    { MAGICAL_BREATHING, "respiration magique" },
    /* timed fire resistance and water walking are possible in explore mode
       (as well as in wizard mode) after life-saving in lava if it fails to
       teleport the hero to safety and player declines to die */
    { WWALKING, "marche sur l'eau" },
    { FIRE_RES, "résistance au feu" },
    /*
     * Properties beyond here don't have timed values during normal play,
     * so there's not much point in trying to order them sensibly.
     * They're either on or off based on equipment, role, actions, &c,
     * but in wizard mode, #wizintrinsic can give them as timed effects.
     */
    { COLD_RES, "résistance au froid" },
    { SLEEP_RES, "résistance au sommeil" },
    { DISINT_RES, "résistance à la désintégration" },
    { SHOCK_RES, "résistance à l'électricité" },
    { POISON_RES, "résistance au poison" },
    { DRAIN_RES, "résistance au drainage" },
    { SICK_RES, "résistance à la maladie" },
    { ANTIMAGIC, "résistance à la magie" },
    { HALLUC_RES, "résistance aux hallucinations" },
    { BLND_RES, "résistance à l'aveuglement lumineux" },
    { FUMBLING, "maladresse" },
    { HUNGER, "faim vorace" },
    { TELEPAT, "télépathie" },
    { WARNING, "avertissement" },
    { WARN_OF_MON, "avert. : type ou classe de monstre" },
    { WARN_UNDEAD, "avert. : morts-vivants" },
    { SEARCHING, "fouille" },
    { INFRAVISION, "infravision" },
    { ADORNED, "paré (+/- Cha)" },
    { STEALTH, "furtivité" },
    { AGGRAVATE_MONSTER, "aggravation des monstres" },
    { CONFLICT, "conflit" },
    { JUMPING, "saut" },
    { TELEPORT_CONTROL, "contrôle de la téléportation" },
    { FLYING, "vol" },
    { SWIMMING, "nage" },
    { SLOW_DIGESTION, "digestion lente" },
    { HALF_SPDAM, "dégâts des sorts réduits de moitié" },
    { HALF_PHDAM, "dégâts physiques réduits de moitié" },
    { REGENERATION, "régénération des PV" },
    { ENERGY_REGENERATION, "régénération de l'énergie" },
    { PROTECTION, "protection supplémentaire" },
    { PROT_FROM_SHAPE_CHANGERS, "protection contre les métamorphes" },
    { POLYMORPH_CONTROL, "contrôle de la polymorphie" },
    { UNCHANGING, "immuable" },
    { REFLECTING, "réflexion" },
    { FREE_ACTION, "action libre" },
    { FIXED_ABIL, "caractéristiques fixes" },
    { LIFESAVED, "vie sauve" },
    {  0, 0 },
};

const char *
property_by_index(int idx, int *propertynum)
{
    if (!IndexOkT(idx, propertynames))
        idx = SIZE(propertynames) - 1;

    if (propertynum)
        *propertynum = propertynames[idx].prop_num;
    return propertynames[idx].prop_name;
}

/* He is being petrified - dialogue by inmet!tower */
static NEARDATA const char *const stoned_texts[] = {
    "Vous ralentissez.",                /* 5 */
    "Vos membres se raidissent.",       /* 4 */
    "Vos membres se pétrifient.",       /* 3 */
    "Votre corps est devenu pierre.",   /* 2 */
    "Vous êtes une statue."             /* 1 */
};

staticfn void
stoned_dialogue(void)
{
    long i = (Stoned & TIMEOUT);

    if (i > 0L && i <= SIZE(stoned_texts)) {
        char buf[BUFSZ];

        Strcpy(buf, stoned_texts[SIZE(stoned_texts) - i]);
        if (nolimbs(gy.youmonst.data) && strstri(buf, "membres"))
            (void) strsubst(buf, "membres", "extrémités");
        urgent_pline("%s", buf);
    }
    switch ((int) i) {
    case 5: /* slowing down */
        HFast = 0L;
        if (gm.multi > 0)
            nomul(0);
        break;
    case 4: /* limbs stiffening */
        /* just one move left to save oneself so quit fiddling around;
           don't stop attempt to eat tin--might be lizard or acidic */
        if (!Popeye(STONED))
            stop_occupation();
        if (gm.multi > 0)
            nomul(0);
        break;
    case 3: /* limbs turned to stone */
        stop_occupation();
        nomul(-3); /* can't move anymore */
        gm.multi_reason = "getting stoned";
        gn.nomovemsg = You_can_move_again; /* not unconscious */
        /* "your limbs have turned to stone" so terminate wounded legs */
        if (Wounded_legs && !u.usteed)
            heal_legs(2);
        break;
    case 2: /* turned to stone */
        if ((HDeaf & TIMEOUT) > 0L && (HDeaf & TIMEOUT) < 5L)
            set_itimeout(&HDeaf, 5L); /* avoid Hear_again at tail end */
        /* if also vomiting or turning into slime, stop those (no messages) */
        if (Vomiting)
            make_vomiting(0L, FALSE);
        if (Slimed)
            make_slimed(0L, (char *) 0);
        break;
    default:
        break;
    }
    exercise(A_DEX, FALSE);
}

/* hero is getting sicker and sicker prior to vomiting */
static NEARDATA const char *const vomiting_texts[] = {
    "avez légèrement la nausée.",                /* 14 */
    "vous sentez un peu confus.",                /* 11 */
    "n'arrivez pas à avoir les idées claires.",  /* 8 */
    "vous sentez incroyablement mal.",           /* 5 */
    "êtes sur le point de vomir."                /* 2 */
};

staticfn void
vomiting_dialogue(void)
{
    const char *txt = 0;
    char buf[BUFSZ];
    long v = (Vomiting & TIMEOUT);

    /* note: nhtimeout() hasn't decremented timed properties for the
       current turn yet, so we use Vomiting-1 here */
    switch ((int) (v - 1L)) {
    case 14:
        txt = vomiting_texts[0];
        break;
    case 11:
        txt = vomiting_texts[1];
        if (strstri(txt, " confus") && (Confusion || flags.female)) {
            Sprintf(buf, "vous sentez %s confus%s.",
                    Confusion ? "encore plus" : "un peu", UE);
            txt = buf;
        }
        break;
    case 6:
        make_stunned((HStun & TIMEOUT) + (long) d(2, 4), FALSE);
        if (!Popeye(VOMITING))
            stop_occupation();
        FALLTHROUGH;
    /*FALLTHRU*/
    case 9:
        make_confused((HConfusion & TIMEOUT) + (long) d(2, 4), FALSE);
        if (gm.multi > 0)
            nomul(0);
        break;
    case 8:
        txt = vomiting_texts[2];
        if (strstri(txt, " idées") && Stunned)
            txt = strsubst(strcpy(buf, txt), "n'arrivez pas à ",
                           "ne pouvez pas ");
        break;
    case 5:
        txt = vomiting_texts[3];
        break;
    case 2:
        txt = vomiting_texts[4];
        if (cantvomit(gy.youmonst.data))
            txt = "avez des haut-le-cœur incontrôlables.";
        else if (Hallucination)
            /* "hurl" is short for "hurl chunks" which is slang for
               relatively violent vomiting... */
            txt = "êtes sur le point de gerber !";
        break;
    case 0:
        stop_occupation();
        if (!cantvomit(gy.youmonst.data)) {
            morehungry(20);
            /* case 2 used to be "You suddenly vomit!" but it wasn't sudden
               since you've just been through the earlier messages of the
               countdown, and it was still possible to move around between
               that message and "You can move again." (from vomit()'s
               nomul(-2)) with no intervening message; give one here to
               have more specific point at which hero became unable to move
               [vomit() issues its own message for the cantvomit() case
               and for the FAINTING-or-worse case where stomach is empty] */
            if (u.uhs < FAINTING)
                You("%s !", !Hallucination ? "vomissez" : "dégobillez");
        }
        vomit();
        break;
    default:
        break;
    }
    if (txt)
        You1(txt);
    exercise(A_CON, FALSE);
}

staticfn void
sleep_dialogue(void)
{
    long i = (HSleepy & TIMEOUT);

    if (i == 4)
        You("bâillez.");
}

DISABLE_WARNING_FORMAT_NONLITERAL   /* RESTORE is after slime_dialogue */

static NEARDATA const char *const choke_texts[] = {
    "Vous avez du mal à respirer.",
    "Vous cherchez désespérément votre souffle.",
    "Vous ne pouvez plus respirer.",
    "Vous devenez %s.",
    "Vous étouffez."
};

static NEARDATA const char *const choke_texts2[] = {
    "Votre %s se comprime.",
    "Votre sang a du mal à atteindre votre cerveau.",
    "La pression sur votre %s augmente.",
    "Votre conscience s'estompe.",
    "Vous étouffez."
};

staticfn void
choke_dialogue(void)
{
    long i = (Strangled & TIMEOUT);

    if (i > 0 && i <= SIZE(choke_texts)) {
        if (Breathless || !rn2(50)) {
            urgent_pline(choke_texts2[SIZE(choke_texts2) - i],
                         body_part(NECK));
        } else {
            const char *str = choke_texts[SIZE(choke_texts) - i];

            if (strchr(str, '%'))
                urgent_pline(str, fr_adj(hcolor(NH_BLUE),
                                         flags.female ? FR_FEM : FR_MASC,
                                         FALSE));
            else
                urgent_pline("%s", str);
            stop_occupation();
        }
    }
    exercise(A_STR, FALSE);
}

static NEARDATA const char *const sickness_texts[] = {
    "Votre maladie s'aggrave.",
    "Votre maladie est grave.",
    "Vous êtes aux portes de la Mort.",
};

staticfn void
sickness_dialogue(void)
{
    long j = (Sick & TIMEOUT), i = j / 2L;

    if (i > 0L && i <= SIZE(sickness_texts) && (j % 2) != 0) {
        char buf[BUFSZ], pronounbuf[40];

        Strcpy(buf, sickness_texts[SIZE(sickness_texts) - i]);
        /* change the message slightly for food poisoning */
        if ((u.usick_type & SICK_NONVOMITABLE) == 0)
            (void) strsubst(buf, "maladie", "intoxication");
        if (Hallucination && strstri(buf, "portes de la Mort")) {
            /* youmonst: for Hallucination, mhe()'s mon argument isn't used */
            Strcpy(pronounbuf, mhe(&gy.youmonst));
            Sprintf(eos(buf), "  %s vous %s à entrer.",
                    /* upstart() modifies its argument but vtense() doesn't
                       care whether or not that has already happened */
                    upstart(pronounbuf), vtense(pronounbuf, "inviter"));
        }
        urgent_pline("%s", buf);
    }
    exercise(A_CON, FALSE);
}

static NEARDATA const char *const levi_texts[] = {
    "Vous flottez un peu plus bas.",
    "Vous vacillez de façon instable %s %s."
};

staticfn void
levitation_dialogue(void)
{
    /* -1 because the last message comes via float_down() */
    long i = (((HLevitation & TIMEOUT) - 1L) / 2L);

    if (ELevitation)
        return;

    if (!ACCESSIBLE(levl[u.ux][u.uy].typ)
        && !is_pool_or_lava(u.ux,u.uy))
        return;

    if (((HLevitation & TIMEOUT) % 2L) && i > 0L && i <= SIZE(levi_texts)) {
        const char *s = levi_texts[SIZE(levi_texts) - i];

        if (strchr(s, '%')) {
            boolean danger = (is_pool_or_lava(u.ux, u.uy)
                              && !Is_waterlevel(&u.uz));

            urgent_pline(s, danger ? "au-dessus" : "dans",
                         danger ? du(surface(u.ux, u.uy)) : "l'air");
        } else
            pline1(s);
        stop_occupation();
    }
}

static NEARDATA const char *const slime_texts[] = {
    "Vous devenez un peu %s.",            /* 5 */
    "Vos membres suintent.",              /* 4 */
    "Votre peau commence à se détacher.", /* 3 */
    "Vous vous changez en %s.",           /* 2 */
    "Vous n'êtes plus que %s."            /* 1 */
};

staticfn void
slime_dialogue(void)
{
    long t = (Slimed & TIMEOUT), i = t / 2L;

    if (t == 1L) {
        /* display as green slime during "You have become green slime."
           but don't worry about not being able to see self; if already
           mimicking something else at the time, implicitly be revealed */
        gy.youmonst.m_ap_type = M_AP_MONSTER;
        gy.youmonst.mappearance = PM_GREEN_SLIME;
        /* no message given when 't' is odd, so no automatic update of
           self; force one */
        newsym(u.ux, u.uy);
    }

    if ((t % 2L) != 0L && i >= 0L && i < SIZE(slime_texts)) {
        char buf[BUFSZ];

        Strcpy(buf, slime_texts[SIZE(slime_texts) - i - 1L]);
        if (nolimbs(gy.youmonst.data) && strstri(buf, "membres"))
            (void) strsubst(buf, "membres", "extrémités");

        if (strchr(buf, '%')) {
            if (i == 4L) {  /* "you are turning green" */
                if (!Blind) /* [what if you're already green?] */
                    urgent_pline(buf, fr_adj(hcolor(NH_GREEN),
                                           flags.female ? FR_FEM : FR_MASC,
                                           FALSE));
            } else {
                urgent_pline(buf, an(Hallucination ? rndmonnam(NULL)
                                                   : "limon vert"));
            }
        } else {
            urgent_pline("%s", buf);
        }
    }

    switch (i) {
    case 3L:  /* limbs becoming oozy */
        HFast = 0L; /* lose intrinsic speed */
        if (!Popeye(SLIMED))
            stop_occupation();
        if (gm.multi > 0)
            nomul(0);
        break;
    case 2L: /* skin begins to peel */
        if ((HDeaf & TIMEOUT) > 0L && (HDeaf & TIMEOUT) < 5L)
            set_itimeout(&HDeaf, 5L); /* avoid Hear_again at tail end */
        break;
    case 1L: /* turning into slime */
        /* if also turning to stone, stop doing that (no message) */
        if (Stoned)
            make_stoned(0L, (char *) 0, KILLED_BY_AN, (char *) 0);
        break;
    }
    exercise(A_DEX, FALSE);
}

RESTORE_WARNING_FORMAT_NONLITERAL

void
burn_away_slime(void)
{
    if (Slimed) {
        make_slimed(0L, "Le limon qui vous recouvre est brûlé !");
    }
}

/* countdown timer for turning into green slime has run out; kill our hero */
staticfn void
slimed_to_death(struct kinfo *kptr)
{
    uchar save_mvflags;

    /* redundant: polymon() cures sliming when polying into green slime */
    if (Upolyd && gy.youmonst.data == &mons[PM_GREEN_SLIME]) {
        dealloc_killer(kptr);
        return;
    }
    /* more sure killer reason is set up */
    if (kptr && kptr->name[0]) {
        svk.killer.format = kptr->format;
        Strcpy(svk.killer.name, kptr->name);
    } else {
        svk.killer.format = NO_KILLER_PREFIX;
        Strcpy(svk.killer.name, flags.female ? "changée en limon vert"
                                            : "changé en limon vert");
    }
    dealloc_killer(kptr);

    /*
     * Polymorph into a green slime, which might destroy some worn armor
     * (potentially affecting bones) and dismount from steed.
     * Can't be Unchanging; wouldn't have turned into slime if we were.
     * Despite lack of Unchanging, neither done() nor savelife() calls
     * rehumanize() if hero dies while polymorphed.
     * polymon() undoes the slime countdown's mimick-green-slime hack
     * but does not perform polyself()'s light source bookkeeping.
     * No longer need to manually increment uconduct.polyselfs to reflect
     * [formerly implicit] change of form; polymon() takes care of that.
     * Temporarily ungenocide if necessary.
     */
    if (emits_light(gy.youmonst.data))
        del_light_source(LS_MONSTER, monst_to_any(&gy.youmonst));
    save_mvflags = svm.mvitals[PM_GREEN_SLIME].mvflags;
    svm.mvitals[PM_GREEN_SLIME].mvflags = save_mvflags & ~G_GENOD;
    /* become a green slime; also resets youmonst.m_ap_type+.mappearance */
    (void) polymon(PM_GREEN_SLIME);
    svm.mvitals[PM_GREEN_SLIME].mvflags = save_mvflags;
    done_timeout(TURNED_SLIME, SLIMED);

    /* life-saved; even so, hero still has turned into green slime;
       player may have genocided green slimes after being infected */
    if ((svm.mvitals[PM_GREEN_SLIME].mvflags & G_GENOD) != 0) {
        char slimebuf[BUFSZ];

        svk.killer.format = KILLED_BY;
        Strcpy(svk.killer.name, "limonicide");
        /* vary the message depending upon whether life-save was due to
           amulet or due to declining to die in explore or wizard mode */
        Strcpy(slimebuf, "le limon vert a été génocidé...");
        if (iflags.last_msg == PLNMSG_OK_DONT_DIE)
            /* follows "OK, so you don't die." and arg is second sentence */
            urgent_pline("Si, vous mourez.  %s", upstart(slimebuf));
        else
            /* follows "The medallion crumbles to dust." */
            urgent_pline("Malheureusement, %s", slimebuf);
        /* die again; no possibility of amulet this time */
        done(GENOCIDED); /* [should it be done_timeout(GENOCIDED, SLIMED)?] */
        /* could be life-saved again (only in explore or wizard mode)
           but green slimes are gone; just stay in current form */
    }
    return;
}

/* Intrinsic Passes_walls is temporary when your god is trying to fix
   all troubles and then TROUBLE_STUCK_IN_WALL calls safe_teleds() but
   it can't find anywhere to place you.  If that happens you get a small
   value for (HPasses_walls & TIMEOUT) to move somewhere yourself.
   Message given is "you feel much slimmer" as a joke hint that you can
   move between things which are closely packed--like the substance of
   solid rock! */
static NEARDATA const char *const phaze_texts[] = {
    "Vous commencez à avoir l'impression d'enfler.",
    "Vous vous sentez plutôt flasque.",
};

staticfn void
phaze_dialogue(void)
{
    long i = ((HPasses_walls & TIMEOUT) / 2L);

    if (EPasses_walls || (HPasses_walls & ~TIMEOUT))
        return;

    if (((HPasses_walls & TIMEOUT) % 2L) && i > 0L && i <= SIZE(phaze_texts))
        pline("%s", phaze_texts[SIZE(phaze_texts) - i]);
}

/* Similar to Passes_walls, if prayer tries to save hero from a poison
   gas region but can't, (HMagical_breathing & TIMEOUT) will be set to
   a small value.  Unlike Passes_walls, there's no joke message. */
static NEARDATA const char *const region_texts[] = {
    "Vous semblez avoir du mal à respirer.",
    "L'air ici semble vicié.",
};

staticfn void
region_dialogue(void)
{
    boolean no_need_to_breathe, in_poison_gas_cloud;
    long r = (HMagical_breathing & TIMEOUT), i = r / 2L;

    /* might have poly'd into non-breather or moved out of gas cloud */
    HMagical_breathing &= ~TIMEOUT;
    no_need_to_breathe = Breathless;
    in_poison_gas_cloud = region_danger();
    HMagical_breathing |= r;
    if (no_need_to_breathe || !in_poison_gas_cloud)
        return;

    if ((r % 2L) && i > 0L && i <= SIZE(region_texts))
        pline("%s", region_texts[SIZE(region_texts) - i]);
}

/* when a status timeout is fatal, keep the status line indicator shown
   during end of game rundown (and potential dumplog);
   timeout has already counted down to 0 by the time we get here */
staticfn void
done_timeout(int how, int which)
{
    long *intrinsic_p = &u.uprops[which].intrinsic;

    *intrinsic_p |= I_SPECIAL; /* affects final disclosure */
    done(how);

    /* life-saved */
    *intrinsic_p &= ~I_SPECIAL;
    disp.botl = TRUE;
}

void
nh_timeout(void)
{
    struct prop *upp;
    struct kinfo *kptr;
    boolean was_flying;
    int sleeptime;
    int m_idx;
    int baseluck = (flags.moonphase == FULL_MOON) ? 1 : 0;

    if (flags.friday13)
        baseluck -= 1;

    if (svq.quest_status.killed_leader)
        baseluck -= 4;

    if (Role_if(PM_ARCHEOLOGIST) && uarmh && uarmh->otyp == FEDORA)
        baseluck += 1;

    if (u.uluck != baseluck
        && svm.moves % ((u.uhave.amulet || u.ugangr) ? 300 : 600) == 0) {
        /* Cursed luckstones stop bad luck from timing out; blessed luckstones
         * stop good luck from timing out; normal luckstones stop both;
         * neither is stopped if you don't have a luckstone.
         * Luck is based at 0 usually, +1 if a full moon and -1 on Friday 13th
         */
        int time_luck = stone_luck(FALSE);
        boolean nostone = !carrying(LUCKSTONE) && !stone_luck(TRUE);

        if (u.uluck > baseluck && (nostone || time_luck < 0))
            u.uluck--;
        else if (u.uluck < baseluck && (nostone || time_luck > 0))
            u.uluck++;
    }
    if (u.uinvulnerable)
        return; /* things past this point could kill you */
    if (Stoned)
        stoned_dialogue();
    if (Slimed)
        slime_dialogue();
    if (Vomiting)
        vomiting_dialogue();
    if (Strangled)
        choke_dialogue();
    if (Sick)
        sickness_dialogue();
    if (HLevitation & TIMEOUT)
        levitation_dialogue();
    if (HPasses_walls & TIMEOUT)
        phaze_dialogue();
    if (HMagical_breathing & TIMEOUT)
        region_dialogue();
    if (HSleepy & TIMEOUT)
        sleep_dialogue();
    if (u.mtimedone && !--u.mtimedone) {
        if (Unchanging)
            u.mtimedone = rnd(100 * gy.youmonst.data->mlevel + 1);
        else if (is_were(gy.youmonst.data))
            you_unwere(FALSE); /* if polycontrl, asks whether to rehumanize */
        else
            rehumanize();
    }
    if (u.ucreamed)
        u.ucreamed--;

    /* Dissipate spell-based protection. */
    if (u.usptime) {
        if (--u.usptime == 0 && u.uspellprot) {
            u.usptime = u.uspmtime;
            u.uspellprot--;
            find_ac();
            if (!Blind)
                Norep("La brume %s qui vous entoure %s.",
                      fr_adj(hcolor(NH_GOLDEN), FR_FEM, FALSE),
                      u.uspellprot ? "devient moins dense" : "disparaît");
        }
    }

    if (u.ugallop) {
        if (--u.ugallop == 0L && u.usteed)
            pline("%s cesse de galoper.", Monnam(u.usteed));
    }

    was_flying = Flying;
    for (upp = u.uprops; upp < u.uprops + SIZE(u.uprops); upp++)
        if ((upp->intrinsic & TIMEOUT) && !(--upp->intrinsic & TIMEOUT)) {
            kptr = find_delayed_killer((int) (upp - u.uprops));
            switch (upp - u.uprops) {
            case STONED:
                if (kptr && kptr->name[0]) {
                    svk.killer.format = kptr->format;
                    Strcpy(svk.killer.name, kptr->name);
                } else {
                    svk.killer.format = NO_KILLER_PREFIX;
                    Strcpy(svk.killer.name, flags.female ? "tuée par pétrification"
                                                        : "tué par pétrification");
                }
                dealloc_killer(kptr);
                /* (unlike sliming, you aren't changing form here) */
                done_timeout(STONING, STONED);
                break;
            case SLIMED:
                slimed_to_death(kptr); /* done_timeout(TURNED_SLIME,SLIMED) */
                break;
            case VOMITING:
                make_vomiting(0L, TRUE);
                break;
            case SICK:
                /* hero might be able to bounce back from food poisoning,
                   but not other forms of illness */
                if ((u.usick_type & SICK_NONVOMITABLE) == 0
                    && rn2(100) < ACURR(A_CON)) {
                    You("êtes guéri%s de votre maladie.", UE);
                    make_sick(0, NULL, FALSE, SICK_ALL);
                    exercise(A_CON, FALSE);
                    adjattrib(A_CON, -1, 1);
                    break;
                }
                urgent_pline("Vous mourez de votre maladie.");
                if (kptr && kptr->name[0]) {
                    svk.killer.format = kptr->format;
                    Strcpy(svk.killer.name, kptr->name);
                } else {
                    svk.killer.format = KILLED_BY_AN;
                    svk.killer.name[0] = 0; /* take the default */
                }
                dealloc_killer(kptr);

                if ((m_idx = name_to_mon(svk.killer.name,
                                         (int *) 0)) >= LOW_PM) {
                    if (type_is_pname(&mons[m_idx])) {
                        svk.killer.format = KILLED_BY;
                    } else if (mons[m_idx].geno & G_UNIQ) {
                        Strcpy(svk.killer.name, the(svk.killer.name));
                        svk.killer.format = KILLED_BY;
                    }
                }
                done_timeout(POISONING, SICK);
                u.usick_type = 0;
                break;
            case FAST:
                if (!Very_fast)
                    You_feel("vous sentez ralentir%s.",
                             Fast ? " un peu" : "");
                break;
            case CONFUSION:
                /* So make_confused works properly */
                set_itimeout(&HConfusion, 1L);
                make_confused(0L, TRUE);
                if (!Confusion)
                    stop_occupation();
                break;
            case STUNNED:
                set_itimeout(&HStun, 1L);
                make_stunned(0L, TRUE);
                if (!Stunned)
                    stop_occupation();
                break;
            case BLINDED: {
                boolean was_blind = !!Blind;

                set_itimeout(&HBlinded, 1L);
                make_blinded(0L, TRUE);
                if (was_blind && !Blind)
                    stop_occupation();
                break;
            }
            case DEAF:
                set_itimeout(&HDeaf, 1L);
                make_deaf(0L, TRUE);
                disp.botl = TRUE;
                if (!Deaf)
                    stop_occupation();
                break;
            case INVIS:
                newsym(u.ux, u.uy);
                if (!Invis && !BInvis && !Blind) {
                    if (!See_invisible)
                        You("n'êtes plus invisible.");
                    else
                        You("ne pouvez plus voir à travers vous-même.");
                    stop_occupation();
                }
                break;
            case SEE_INVIS:
                set_mimic_blocking(); /* do special mimic handling */
                see_monsters();       /* make invis mons appear */
                newsym(u.ux, u.uy);   /* make self appear */
                stop_occupation();
                break;
            case WOUNDED_LEGS:
                heal_legs(0);
                stop_occupation();
                break;
            case HALLUC:
                set_itimeout(&HHallucination, 1L);
                (void) make_hallucinated(0L, TRUE, 0L);
                if (!Hallucination)
                    stop_occupation();
                break;
            case SLEEPY:
                if (unconscious() || Sleep_resistance) {
                    incr_itimeout(&HSleepy, rnd(100));
                } else if (Sleepy) {
                    You("vous endormez.");
                    sleeptime = rnd(20);
                    fall_asleep(-sleeptime, TRUE);
                    incr_itimeout(&HSleepy, sleeptime + rnd(100));
                }
                break;
            case LEVITATION:
                /* timed Levitation is ordinary, timed Flying is via
                   #wizintrinsic only; still, we want to avoid float_down()
                   reporting "you have stopped levitating and are now flying"
                   when both are timing out together; if that is about to
                   happen, end Flying early to skip feedback about it;
                   assumes Levitation is handled before Flying */
                if ((HFlying & TIMEOUT) == 1L)
                    set_itimeout(&HFlying, 0L); /* bypass 'case FLYING' */
                (void) float_down(I_SPECIAL | TIMEOUT, 0L);
                break;
            case FLYING:
                /* timed Flying is via #wizintrinsic only */
                if (was_flying && !Flying) {
                    disp.botl = TRUE;
                    You("atterrissez.");
                    spoteffects(TRUE);
                }
                break;
            case ACID_RES:
                if (!Acid_resistance) {
                    if (eating_dangerous_corpse(ACID_RES)) {
                        /* extend temporary acid resistance if in midst
                           of eating an acidic corpse; this will repeat
                           until eating is finished or interrupted */
                        set_itimeout(&u.uprops[ACID_RES].intrinsic, 1L);
                        break;
                    }
                    if (!Unaware)
                        You("ne vous sentez plus à l'abri de l'acide.");
                }
                break;
            case STONE_RES:
                if (!Stone_resistance) {
                    if (eating_dangerous_corpse(STONE_RES)) {
                        /* extend temporary stoning resistance if in midst
                           of eating a stoning corpse; this will repeat
                           until eating is finished or interrupted */
                        set_itimeout(&u.uprops[STONE_RES].intrinsic, 1L);
                        break;
                    }
                    if (!Unaware)
                        You("ne vous sentez plus à l'abri de la pétrification.");
                    /* no-op if not wielding a cockatrice corpse;
                       uswapwep case is always a no-op because two-weapon
                       combat is only possible with two one-handed weapons
                       or weapon tools, not corpses */
                    wielding_corpse(uwep, (struct obj *) 0, FALSE);
                    wielding_corpse(uswapwep, (struct obj *) 0, FALSE);
                }
                break;
            case FIRE_RES:
                /* timed fire resistance and timed water walking combine
                   as a way to survive lava after multiple life-saving
                   attempts fail to relocate hero; skip timeout message
                   if hero has acquired fire resistance in the meantime */
                if (!Fire_resistance)
                    Your("capacité temporaire à survivre aux flammes a pris fin.");
                break;
            case WWALKING:
                /* [see fire resistance] */
                if (!Wwalking)
                    Your("capacité temporaire à marcher sur les liquides a pris fin.");
                break;
            case DISPLACED:
                if (!Displaced) /* give a message */
                    toggle_displacement((struct obj *) 0, 0L, FALSE);
                break;
            case WARN_OF_MON:
                /* timed Warn_of_mon is via #wizintrinsic only */
                if (!Warn_of_mon) {
                    struct permonst *wptr = svc.context.warntype.species;

                    svc.context.warntype.species = (struct permonst *) 0;
                    svc.context.warntype.speciesidx = NON_PM;
                    if (wptr)
                        You("n'êtes plus averti%s de la présence %s.", UE,
                            du(makeplural(wptr->pmnames[NEUTRAL])));
                }
                break;
            case PASSES_WALLS:
                if (!Passes_walls) {
                    if (stuck_in_wall())
                        You_feel("vous sentez de nouveau à l'étroit.");
                    else
                        pline("Vous êtes de nouveau votre moi %s.",
                              !Upolyd ? "habituel" : "inhabituel");
                }
                break;
            case MAGICAL_BREATHING:
                if (!Breathless) {
                    if (region_danger())
                        You("toussez%s",
                            Poison_resistance ? "." : " et crachez du sang !");
                }
                break;
            case STRANGLED:
                svk.killer.format = KILLED_BY;
                Strcpy(svk.killer.name,
                       (u.uburied) ? "suffocation" : "strangulation");
                /* (identical words in French) */
                done_timeout(DIED, STRANGLED);
                /* must be declining to die in explore|wizard mode;
                   treat like being cured of strangulation by prayer */
                if (uamul && uamul->otyp == AMULET_OF_STRANGULATION) {
                    Your("amulette disparaît !");
                    useup(uamul);
                }
                break;
            case FUMBLING:
                /* call this only when a move took place.  */
                /* otherwise handle fumbling msgs locally. */
                if (u.umoved && !(Levitation || Flying)) {
                    slip_or_trip();
                    nomul(-2);
                    gm.multi_reason = "fumbling";
                    gn.nomovemsg = "";
                    /* The more you are carrying the more likely you
                     * are to make noise when you fumble.  Adjustments
                     * to this number must be thoroughly play tested.
                     */
                    if ((inv_weight() > (WT_NOISY_INV * -1))) {
                        if (!Deaf)
                            You("faites beaucoup de bruit !");
                        wake_nearby(FALSE);
                    }
                }
                /* from outside means slippery ice; don't reset
                   counter if that's the only fumble reason */
                HFumbling &= ~FROMOUTSIDE;
                if (Fumbling)
                    incr_itimeout(&HFumbling, rnd(20));

                if (iflags.defer_decor) {
                    /* 'mention_decor' was deferred for message sequencing
                       reasons; catch up now */
                    deferred_decor(FALSE);
                }
                break;
            case DETECT_MONSTERS:
                see_monsters();
                break;
            case GLIB:
                make_glib(0); /* might update persistent inventory */
                break;
            case PROT_FROM_SHAPE_CHANGERS:
                /* timed Protection_from_shape_changers is via
                   #wizintrinsic only */
                if (!Protection_from_shape_changers)
                    restartcham();
                break;
            }
        }

    run_timers();
}

void
fall_asleep(int how_long, boolean wakeup_msg)
{
    stop_occupation();
    nomul(how_long);
    gm.multi_reason = "sleeping";
#if 0   /* this was broken; the fix for 'how_long' will result in changed
         * behavior for sounds that don't go through You_hear() so needs
         * testing */
    /* You_hear() produces "You dream that you hear ..." when sleeping;
       other sound messages will either honor or ignore Deaf */
    if (wakeup_msg && gm.multi == how_long) {
        /* caller can follow with a direct call to Hear_again() if
           there's a need to override this when wakeup_msg is true */
        /* 5.0: how_long is negative so wasn't actually incrementing the
           deafness timeout when it used to be passed as-is */
        incr_itimeout(&HDeaf, abs(how_long));
        disp.botl = TRUE;
        ga.afternmv = Hear_again; /* this won't give any messages */
    }
#endif
    /* early wakeup from combat won't be possible until next monster turn */
    u.usleep = svm.moves;
    gn.nomovemsg = wakeup_msg ? "Vous vous réveillez." : You_can_move_again;
}

/* Attach an egg hatch timeout to the given egg.
 *      when = Time to hatch, usually only passed if re-creating an
 *             existing hatch timer. Pass 0L for random hatch time.
 */
void
attach_egg_hatch_timeout(struct obj *egg, long when)
{
    int i;

    /* stop previous timer, if any */
    (void) stop_timer(HATCH_EGG, obj_to_any(egg));

    /*
     * Decide if and when to hatch the egg.  The old hatch_it() code tried
     * once a turn from age 151 to 200 (inclusive), hatching if it rolled
     * a number x, 1<=x<=age, where x>150.  This yields a chance of
     * hatching > 99.9993%.  Mimic that here.
     */
    if (!when) {
        for (i = (MAX_EGG_HATCH_TIME - 50) + 1; i <= MAX_EGG_HATCH_TIME; i++)
            if (rnd(i) > 150) {
                /* egg will hatch */
                when = (long) i;
                break;
            }
    }
    if (when) {
        (void) start_timer(when, TIMER_OBJECT, HATCH_EGG, obj_to_any(egg));
    }
}

/* prevent an egg from ever hatching */
void
kill_egg(struct obj *egg)
{
    /* stop previous timer, if any */
    (void) stop_timer(HATCH_EGG, obj_to_any(egg));
}

/* timer callback routine: hatch the given egg */
void
hatch_egg(anything *arg, long timeout)
{
    struct obj *egg;
    struct monst *mon, *mon2;
    coord cc;
    coordxy x, y;
    boolean yours, silent, knows_egg = FALSE;
    boolean cansee_hatchspot = FALSE;
    int i, mnum, hatchcount = 0;

    egg = arg->a_obj;
    /* sterilized while waiting */
    if (egg->corpsenm == NON_PM)
        return;

    mon = mon2 = (struct monst *) 0;
    mnum = big_to_little(egg->corpsenm);
    /* The identity of one's father is learned, not innate */
    yours = (egg->spe || (!flags.female && carried(egg) && !rn2(2)));
    silent = (timeout != svm.moves); /* hatched while away */

    /* only can hatch when in INVENT, FLOOR, MINVENT;
       get_obj_location() will fail for MIGRATING, also for CONTAINED
       and BURIED when the flags for those aren't included in the call */
    if (get_obj_location(egg, &x, &y, 0)) {
        hatchcount = rnd((int) egg->quan);
        cansee_hatchspot = cansee(x, y) && !silent;
        if (!(mons[mnum].geno & G_UNIQ)
            && !(svm.mvitals[mnum].mvflags & (G_GENOD | G_EXTINCT))) {
            for (i = hatchcount; i > 0; i--) {
                if (!enexto(&cc, x, y, &mons[mnum])
                    || !(mon = makemon(&mons[mnum], cc.x, cc.y,
                                       NO_MINVENT | MM_NOMSG)))
                    break;
                /* tame if your own egg hatches while you're on the
                   same dungeon level, or any dragon egg which hatches
                   while it's in your inventory */
                if ((yours && !silent)
                    || (carried(egg) && mon->data->mlet == S_DRAGON)) {
                    if (tamedog(mon, (struct obj *) 0, FALSE)) {
                        if (carried(egg) && mon->data->mlet != S_DRAGON)
                            mon->mtame = 20;
                    }
                }
                if (svm.mvitals[mnum].mvflags & G_EXTINCT)
                    break;  /* just made last one */
                mon2 = mon; /* in case makemon() fails on 2nd egg */
            }
            if (!mon)
                mon = mon2;
            hatchcount -= i;
            egg->quan -= (long) hatchcount;
        }
#if 0
    /*
     * We could possibly hatch while migrating, but the code isn't
     * set up for it...
     */
    } else if (obj->where == OBJ_MIGRATING) {
        /*
         * We can do several things.  The first ones that come to
         * mind are:
         * + Create the hatched monster then place it on the migrating
         *   mons list.  This is tough because all makemon() is made
         *   to place the monster as well. Makemon() also doesn't lend
         *   itself well to splitting off a "not yet placed" subroutine.
         * + Mark the egg as hatched, then place the monster when we
         *   place the migrating objects.
         * + Or just kill any egg which gets sent to another level.
         *   Falling is the usual reason such transportation occurs.
         */
        cansee_hatchspot = FALSE;
        mon = ???;
#endif
    }

    if (mon) {
        char monnambuf[BUFSZ], carriedby[BUFSZ];
        boolean siblings = (hatchcount > 1), redraw = FALSE;

        if (cansee_hatchspot) {
            /* [bug?  m_monnam() yields accurate monster type
               regardless of hallucination] */
            Sprintf(monnambuf, "%s%s", siblings ? "des " : "",
                    siblings ? makeplural(m_monnam(mon)) : an(m_monnam(mon)));
            /* we don't learn the egg type here because learning
               an egg type requires either seeing the egg hatch
               or being familiar with the egg already,
               as well as being able to see the resulting
               monster, checked below
            */
        }
        switch (egg->where) {
        case OBJ_INVENT:
            knows_egg = TRUE; /* true even if you are blind */
            if (!cansee_hatchspot)
                You_feel("sentez quelque chose tomber de votre sac !");
            else
                You_see("%s tomber de votre sac !", monnambuf);
            if (yours) {
                boolean quiet = (is_silent(mon->data) || Deaf);

                pline("%s %s \"%s%s\"",
                      siblings ? "Leurs cris" : "Son cri",
                      siblings ? (quiet ? "semblent dire" : "ressemblent à")
                               : (quiet ? "semble dire" : "ressemble à"),
                      flags.female ? "maman" : "papa",
                      egg->spe ? "." : " ?");
            } else if (mon->data->mlet == S_DRAGON && !Deaf) {
                SetVoice(mon, 0, 80, 0);
                verbalize("Gleep!"); /* Mything eggs :-) */
            }
            break;

        case OBJ_FLOOR:
            if (cansee_hatchspot) {
                knows_egg = TRUE;
                You_see("%s éclore.", monnambuf);
                redraw = TRUE; /* update egg's map location */
            }
            break;

        case OBJ_MINVENT:
            if (cansee_hatchspot) {
                /* egg carrying monster might be invisible */
                mon2 = egg->ocarry;
                if (canseemon(mon2)
                    && (!mon2->wormno || cansee(mon2->mx, mon2->my))) {
                    Sprintf(carriedby, "du sac %s", du(a_monnam(mon2)));
                    knows_egg = TRUE;
                } else if (is_pool(mon->mx, mon->my)) {
                    Strcpy(carriedby, "de nulle part dans l'eau");
                } else {
                    Strcpy(carriedby, "de nulle part");
                }
                You_see("%s tomber %s !", monnambuf, carriedby);
            }
            break;
#if 0
        case OBJ_MIGRATING:
            break;
#endif
        default:
            impossible("egg hatched where? (%d)", (int) egg->where);
            break;
        }

        if (cansee_hatchspot && knows_egg)
            learn_egg_type(mnum);

        if (egg->quan > 0) {
            /* still some eggs left; we didn't split the stack, just
               subtracted from quantity so weight needs to be updated;
               for remainder of stack, add a new, short hatch timer */
            attach_egg_hatch_timeout(egg, (long) rnd(12));
            /* container_weight(arg) updates arg->owt, and if contained,
               its enclosing container arg->ocontainer (recursively)
               [egg won't be contained due to conditions imposed above] */
            container_weight(egg);
        } else if (carried(egg)) {
            useup(egg);
        } else {
            /* free egg here because we use it above */
            obj_extract_self(egg);
            obfree(egg, (struct obj *) 0);
            if ((mon = m_at(x,y)) && !hideunder(mon) && cansee(x, y))
                redraw = TRUE;
        }
        if (redraw)
            newsym(x, y);
    }
}

/* Learn to recognize eggs of the given type. */
void
learn_egg_type(int mnum)
{
    /* baby monsters hatch from grown-up eggs */
    mnum = little_to_big(mnum);
    svm.mvitals[mnum].mvflags |= MV_KNOWS_EGG;
    /* we might have just learned about other eggs being carried */
    update_inventory();
}

/* Attach a fig_transform timeout to the given figurine. */
void
attach_fig_transform_timeout(struct obj *figurine)
{
    int i;

    /* stop previous timer, if any */
    (void) stop_timer(FIG_TRANSFORM, obj_to_any(figurine));

    /*
     * Decide when to transform the figurine.
     */
    i = rnd(9000) + 200;
    /* figurine will transform */
    (void) start_timer((long) i, TIMER_OBJECT, FIG_TRANSFORM,
                       obj_to_any(figurine));
}

/* give a fumble message */
staticfn void
slip_or_trip(void)
{
    struct obj *otmp = vobj_at(u.ux, u.uy), *otmp2, *saddle;
    const char *what;
    char buf[BUFSZ];
    boolean on_foot = !u.usteed;

    if (otmp && on_foot && !u.uinwater && is_pool(u.ux, u.uy))
        otmp = 0;

    if (otmp && on_foot) { /* trip over something in particular */
        /*
          If there is only one item, it will have just been named
          during the move, so refer to it by pronoun; otherwise,
          if the top item has been or can be seen, refer to it by
          name; if not, look for rocks to trip over; trip over
          anonymous "something" if there aren't any rocks.
        */
        boolean pronoun = (iflags.last_msg == PLNMSG_ONE_ITEM_HERE);

        what = pronoun
                ? ((otmp->quan == 1L) ? "ça" : "ces trucs")
                : (otmp->dknown || !Blind)
                      ? doname(otmp)
                      : ((otmp2 = sobj_at(ROCK, u.ux, u.uy)) == 0
                             ? "quelque chose"
                             : (otmp2->quan == 1L ? "un caillou"
                                                  : "des cailloux"));
        if (Hallucination) {
            what = strcpy(buf, what);
            (void) fr_upstart(buf);
            pline("Sapristi !  %s vous %s les %s !", what,
                  (!otmp || otmp->quan == 1L) ? "mord" : "mordent",
                  makeplural(body_part(FOOT)));
        } else if (pronoun) {
            You("trébuchez dessus.");
        } else {
            You("trébuchez sur %s.", what);
        }
        if (!uarmf && otmp->otyp == CORPSE
            && touch_petrifies(&mons[otmp->corpsenm]) && !Stone_resistance) {
            Sprintf(svk.killer.name, "en trébuchant sur un cadavre %s",
                    de(mons[otmp->corpsenm].pmnames[NEUTRAL]));
            instapetrify(svk.killer.name);
        }
    } else if ((HFumbling & FROMOUTSIDE) || (is_ice(u.ux, u.uy) && !rn2(3))) {
        /* is fumbling from ice alone? */
        boolean ice_only = !(EFumbling || (HFumbling & ~FROMOUTSIDE));

        pline("%s %s %s.",
              u.usteed ? upstart(x_monnam(u.usteed, ARTICLE_THE, (char *) 0,
                                          SUPPRESS_SADDLE, FALSE))
                       : "Vous",
              u.usteed ? (rn2(2) ? "glisse" : "dérape")
                       : (rn2(2) ? "glissez" : "dérapez"),
              /* sometimes slipping due to ice occurs during turn that hero
                 has just moved off the ice; phrase things differently then */
              is_ice(u.ux, u.uy) ? "sur la glace" : "en quittant la glace");
        /* fumbling outside of ice while mounted always causes the hero to
           fall from the saddle (unless it is cursed), so to avoid a
           counterintuitive effect where ice makes riding _less_ hazardous,
           unconditionally dismount if fumbling is from a non-ice source */
        if (!on_foot
            && ((saddle = which_armor(u.usteed, W_SADDLE)) == 0
                || !saddle->cursed)
            && (!ice_only || !rn2(3))) {
            You("perdez l'équilibre.");
            dismount_steed(DISMOUNT_FELL);
        } else if (!rn2(10 + ACURR(A_DEX))) {
            /* Maybe slip in a random direction.  This takes place after
               the hero has already changed location.  If the hero is
               in grid bug form, only allow forward hurtle, otherwise a
               90 degree orthogonal one after the step would make the
               combined move appear to be a single diagonal step. */
            if (!NODIAG(u.umonnum))
                confdir(TRUE); /* sets u.dx and u.dy */
            /* Only hurtle if the random direction won't move hero back
               to same spot where this move started. */
            if (u.ux + u.dx != u.ux0 || u.uy + u.dy != u.uy0)
                hurtle(u.dx, u.dy, 1, FALSE);
        }
    } else {
        if (on_foot) {
            switch (rn2(4)) {
            case 1:
                You("trébuchez sur vos propres %s.",
                    Hallucination ? "coudes" : makeplural(body_part(FOOT)));
                break;
            case 2:
                You("glissez %s.",
                    Hallucination ? "sur une peau de banane"
                                  : "et manquez de tomber");
                break;
            case 3:
                You("pataugez.");
                break;
            default:
                You("titubez.");
                break;
            }

        /* mounted; saddle should never end up being Null here;
           don't fall off when it happens to be cursed */
        } else if ((saddle = which_armor(u.usteed, W_SADDLE)) == 0
                   || !saddle->cursed) {
            switch (rn2(4)) {
            case 1:
                pline("Vos %s glissent hors des étriers.",
                      makeplural(body_part(FOOT)));
                break;
            case 2:
                You("lâchez les rênes.");
                break;
            case 3:
                You("vous cognez contre le pommeau de la selle.");
                break;
            default:
                You("glissez sur le côté de la selle.");
                break;
            }
            dismount_steed(DISMOUNT_FELL);
        }
    }
}

/* Print a lamp flicker message with tailer.  Only called if seen. */
staticfn void
see_lamp_flicker(struct obj *obj, const char *tailer)
{
    switch (obj->where) {
    case OBJ_INVENT:
    case OBJ_MINVENT:
        pline("%s %s%s.", Yname2(obj), otense(obj, "vaciller"), tailer);
        break;
    case OBJ_FLOOR:
        You_see("%s vaciller%s.", an(xname(obj)), tailer);
        break;
    }
}

/* Print a dimming message for brass lanterns.  Only called if seen. */
staticfn void
lantern_message(struct obj *obj)
{
    /* from adventure */
    switch (obj->where) {
    case OBJ_INVENT:
        Your("lanterne faiblit.");
        if (Hallucination)
            pline("Les piles n'ont pas encore été inventées.");
        break;
    case OBJ_FLOOR:
        You_see("une lanterne faiblir.");
        break;
    case OBJ_MINVENT:
        pline("La lanterne %s faiblit.", du(mon_nam(obj->ocarry)));
        break;
    }
}

/*
 * Timeout callback for objects that are burning. E.g. lamps, candles.
 * See begin_burn() for meanings of obj->age and obj->spe.
 */
void
burn_object(anything *arg, long timeout)
{
    struct obj *obj = arg->a_obj;
    boolean canseeit, many, menorah, need_newsym, need_invupdate, bytouch;
    coordxy x, y;
    char whose[BUFSZ];

    menorah = obj->otyp == CANDELABRUM_OF_INVOCATION;
    many = menorah ? obj->spe > 1 : obj->quan > 1L;

    /* timeout while away */
    if (timeout != svm.moves) {
        long how_long = svm.moves - timeout;

        if (how_long >= obj->age) {
            obj->age = 0;
            end_burn(obj, FALSE);

            if (menorah) {
                obj->spe = 0; /* no more candles */
                obj->owt = weight(obj);
            } else if (Is_candle(obj) || obj->otyp == POT_OIL) {
                struct monst *mtmp = NULL;

                if (obj->where == OBJ_FLOOR)
                    mtmp = m_at(obj->ox, obj->oy);
                /* get rid of candles and burning oil potions;
                   we know this object isn't carried by hero,
                   nor is it migrating */
                obj_extract_self(obj);
                obfree(obj, (struct obj *) 0);
                obj = (struct obj *) 0;
                if (mtmp)
                    maybe_unhide_at(mtmp->mx, mtmp->my);
            }

        } else {
            obj->age -= how_long;
            begin_burn(obj, TRUE);
        }
        return;
    }

    /* only interested in INVENT, FLOOR, and MINVENT */
    if (get_obj_location(obj, &x, &y, 0)) {
        canseeit = !Blind && cansee(x, y);
        /* set `whose[]' to be "Your " or "Fred's " or "The goblin's " */
        (void) Shk_Your(whose, obj);
    } else {
        canseeit = FALSE;
    }
    /* when carrying the light source, you can feel the heat from lit lamp
       or candle so you'll be notified when it burns out even if blind at
       the time; brass lantern doesn't radiate sufficient heat for that
       (however, inventory formatting drops "(lit)" so player can tell) */
    bytouch = (obj->where == OBJ_INVENT && obj->otyp != BRASS_LANTERN);
    need_newsym = need_invupdate = FALSE;

    /* obj->age is the age remaining at this point.  */
    switch (obj->otyp) {
    case POT_OIL:
        /* this should only be called when we run out */
        if (canseeit) {
            switch (obj->where) {
            case OBJ_INVENT:
                need_invupdate = TRUE;
                FALLTHROUGH;
                /*FALLTHRU*/
            case OBJ_MINVENT:
                pline("%s %s de brûler.", Yname2(obj), otense(obj, "finir"));
                break;
            case OBJ_FLOOR:
                You_see("une potion d'huile enflammée s'éteindre.");
                need_newsym = TRUE;
                break;
            }
        }
        end_burn(obj, FALSE); /* turn off light source */
        if (carried(obj)) {
            useupall(obj);
        } else {
            /* clear migrating obj's destination code before obfree
               to avoid false complaint of deleting worn item */
            if (obj->where == OBJ_MIGRATING)
                obj->owornmask = 0L;
            obj_extract_self(obj);
            obfree(obj, (struct obj *) 0);
        }
        obj = (struct obj *) 0;
        break;

    case BRASS_LANTERN:
    case OIL_LAMP:
        switch ((int) obj->age) {
        case 150:
        case 100:
        case 50:
            if (canseeit) {
                if (obj->otyp == BRASS_LANTERN)
                    lantern_message(obj);
                else
                    see_lamp_flicker(obj,
                                     obj->age == 50L ? " fortement" : "");
            }
            break;

        case 25:
            if (canseeit) {
                if (obj->otyp == BRASS_LANTERN) {
                    lantern_message(obj);
                } else {
                    switch (obj->where) {
                    case OBJ_INVENT:
                    case OBJ_MINVENT:
                        pline("%s %s sur le point de s'éteindre.", Yname2(obj),
                              otense(obj, "sembler"));
                        break;
                    case OBJ_FLOOR:
                        You_see("%s sur le point de s'éteindre.", an(xname(obj)));
                        break;
                    }
                }
            }
            break;

        case 0:
            /* even if blind you'll know if holding it */
            if (canseeit || bytouch) {
                switch (obj->where) {
                case OBJ_INVENT:
                    need_invupdate = TRUE;
                    FALLTHROUGH;
                    /*FALLTHRU*/
                case OBJ_MINVENT:
                    if (obj->otyp == BRASS_LANTERN)
                        pline("%s n'a plus d'énergie.", Yname2(obj));
                    else
                        pline("%s.", Tobjnam(obj, "s'éteindre"));
                    break;
                case OBJ_FLOOR:
                    if (obj->otyp == BRASS_LANTERN)
                        You_see("une lanterne tomber en panne.");
                    else
                        You_see("%s s'éteindre.", an(xname(obj)));
                    break;
                }
            }
            end_burn(obj, FALSE);
            break;

        default:
            /*
             * Someone added fuel to the lamp while it was
             * lit. Just fall through and let begin_burn()
             * handle the new age.
             */
            break;
        }

        if (obj->age)
            begin_burn(obj, TRUE);

        break;

    case CANDELABRUM_OF_INVOCATION:
    case TALLOW_CANDLE:
    case WAX_CANDLE:
        switch (obj->age) {
        case 75:
            if (canseeit)
                switch (obj->where) {
                case OBJ_INVENT:
                case OBJ_MINVENT:
                    if (menorah)
                        pline("%s %s %s.",
                              many ? "Les bougies" : "La bougie",
                              du(yname(obj)),
                              many ? "raccourcissent" : "raccourcit");
                    else
                        pline("%s %s.", Yname2(obj),
                              otense(obj, "raccourcir"));
                    break;
                case OBJ_FLOOR:
                    if (menorah)
                        You_see("%s d'un candélabre raccourcir.",
                                many ? "les bougies" : "la bougie");
                    else
                        You_see("%s raccourcir.", an(xname(obj)));
                    break;
                }
            break;

        case 15:
            if (canseeit)
                switch (obj->where) {
                case OBJ_INVENT:
                case OBJ_MINVENT:
                    pline("%s%s %s %s !",
                          many ? "Les flammes" : "La flamme",
                          menorah ? (many ? " des bougies" : " de la bougie")
                                  : "",
                          du(yname(obj)),
                          many ? "vacillent" : "vacille");
                    break;
                case OBJ_FLOOR:
                    if (menorah)
                        You_see("%s %s d'un candélabre vaciller !",
                                many ? "les flammes" : "la flamme",
                                many ? "des bougies" : "de la bougie");
                    else
                        You_see("%s %s vaciller !",
                                many ? "les flammes" : "la flamme",
                                du(an(xname(obj))));
                    break;
                }
            break;

        case 0:
            /* we know even if blind and in our inventory */
            if (canseeit || bytouch) {
                if (menorah) {
                    switch (obj->where) {
                    case OBJ_INVENT:
                        need_invupdate = TRUE;
                        FALLTHROUGH;
                        /*FALLTHRU*/
                    case OBJ_MINVENT:
                        pline("%s %s %s.",
                              many ? "Les flammes" : "La flamme",
                              du(yname(obj)),
                              many ? "s'éteignent" : "s'éteint");
                        break;
                    case OBJ_FLOOR:
                        You_see("%s d'un candélabre s'éteindre.",
                                many ? "les flammes" : "la flamme");
                        break;
                    }
                } else {
                    switch (obj->where) {
                    case OBJ_INVENT:
                        /* no need_invupdate for update_inventory() necessary;
                           useupall() -> freeinv() handles it */
                           FALLTHROUGH;
                        /*FALLTHRU*/
                    case OBJ_MINVENT:
                        pline("%s %s consumé%s !", Yname2(obj),
                              otense(obj, "être"), accord(xname(obj)));
                        break;
                    case OBJ_FLOOR:
                        /*
                          You see some wax candles consumed!
                          You see a wax candle consumed!
                         */
                        You_see("%s se consumer !", an(xname(obj)));
                        need_newsym = TRUE;
                        break;
                    }

                    /* post message */
                    pline(Hallucination
                              ? (many ? "Elles hurlent !" : "Elle hurle !")
                              : Blind ? "" : (many ? "Leurs flammes s'éteignent."
                                                   : "Sa flamme s'éteint."));
                }
            }
            end_burn(obj, FALSE);

            if (menorah) {
                obj->spe = 0; /* no candles */
                obj->owt = weight(obj);
                if (carried(obj))
                    need_invupdate = TRUE;
            } else {
                if (carried(obj)) {
                    useupall(obj);
                } else {
                    boolean onfloor = (obj->where == OBJ_FLOOR);

                    /* clear migrating obj's destination code
                       so obfree won't think this item is worn */
                    if (obj->where == OBJ_MIGRATING)
                        obj->owornmask = 0L;
                    obj_extract_self(obj);
                    if (onfloor)
                        maybe_unhide_at(x, y);
                    obfree(obj, (struct obj *) 0);
                }
                obj = (struct obj *) 0;
            }
            break; /* case [age ==] 0 */

        default:
            /*
             * Someone added fuel (candles) to the menorah while
             * it was lit. Just fall through and let begin_burn()
             * handle the new age.
             */
            break;
        }

        if (obj && obj->age)
            begin_burn(obj, TRUE);
        break; /* case [otyp ==] candelabrum|tallow_candle|wax_candle */

    default:
        impossible("burn_object: unexpected obj %s", xname(obj));
        break;
    }
    if (need_newsym)
        newsym(x, y);
    if (need_invupdate)
        update_inventory();
}

/*
 * Start a burn timeout on the given object. If not "already lit" then
 * create a light source for the vision system.  There had better not
 * be a burn already running on the object.
 *
 * Magic lamps stay lit as long as there's a genie inside, so don't start
 * a timer.
 *
 * Burn rules:
 *      potions of oil, lamps & candles:
 *              age = # of turns of fuel left
 *              spe = <unused>
 *      magic lamps:
 *              age = <unused>
 *              spe = 0 not lightable, 1 lightable forever
 *      candelabrum:
 *              age = # of turns of fuel left
 *              spe = # of candles
 *
 * Once the burn begins, the age will be set to the amount of fuel
 * remaining _once_the_burn_finishes_.  If the burn is terminated
 * early then fuel is added back.
 *
 * This use of age differs from the use of age for corpses and eggs.
 * For the latter items, age is when the object was created, so we
 * know when it becomes "bad".
 *
 * This is a "silent" routine - it should not print anything out.
 */
void
begin_burn(struct obj *obj, boolean already_lit)
{
    int radius = 3;
    long turns = 0;
    boolean do_timer = TRUE;

    if (obj->age == 0 && obj->otyp != MAGIC_LAMP && !artifact_light(obj))
        return;

    switch (obj->otyp) {
    case MAGIC_LAMP:
        obj->lamplit = 1;
        do_timer = FALSE;
        break;

    case POT_OIL:
        turns = obj->age;
        if (obj->odiluted)
            turns = (3L * turns + 2L) / 4L;
        radius = 1; /* very dim light */
        break;

    case BRASS_LANTERN:
    case OIL_LAMP:
        /* magic times are 150, 100, 50, 25, and 0 */
        if (obj->age > 150L)
            turns = obj->age - 150L;
        else if (obj->age > 100L)
            turns = obj->age - 100L;
        else if (obj->age > 50L)
            turns = obj->age - 50L;
        else if (obj->age > 25L)
            turns = obj->age - 25L;
        else
            turns = obj->age;
        break;

    case CANDELABRUM_OF_INVOCATION:
    case TALLOW_CANDLE:
    case WAX_CANDLE:
        /* magic times are 75, 15, and 0 */
        if (obj->age > 75L)
            turns = obj->age - 75L;
        else if (obj->age > 15L)
            turns = obj->age - 15L;
        else
            turns = obj->age;
        radius = candle_light_range(obj);
        break;

    default:
        /* [ALI] Support artifact light sources */
        if (artifact_light(obj)) {
            obj->lamplit = 1;
            do_timer = FALSE;
            radius = arti_light_radius(obj);
        } else {
            impossible("begin burn: unexpected %s", xname(obj));
            turns = obj->age;
        }
        break;
    }

    if (do_timer) {
        if (start_timer(turns, TIMER_OBJECT, BURN_OBJECT, obj_to_any(obj))) {
            obj->lamplit = 1;
            obj->age -= turns;
            if (carried(obj) && !already_lit)
                update_inventory();
        } else {
            obj->lamplit = 0;
        }
    } else {
        if (carried(obj) && !already_lit)
            update_inventory();
    }

    if (obj->lamplit && !already_lit) {
        coordxy x, y;

        if (get_obj_location(obj, &x, &y, CONTAINED_TOO | BURIED_TOO))
            new_light_source(x, y, radius, LS_OBJECT, obj_to_any(obj));
        else
            impossible("begin_burn: can't get obj position");
    }
}

/*
 * Stop a burn timeout on the given object if timer attached.  Darken
 * light source.
 */
void
end_burn(struct obj *obj, boolean timer_attached)
{
    if (!obj->lamplit) {
        impossible("end_burn: obj %s not lit", xname(obj));
        return;
    }

    if (obj->otyp == MAGIC_LAMP || artifact_light(obj))
        timer_attached = FALSE;

    if (!timer_attached) {
        /* [DS] Cleanup explicitly, since timer cleanup won't happen */
        del_light_source(LS_OBJECT, obj_to_any(obj));
        obj->lamplit = 0;
        if (obj->where == OBJ_INVENT)
            update_inventory();
    } else if (!stop_timer(BURN_OBJECT, obj_to_any(obj)))
        impossible("end_burn: obj %s not timed!", xname(obj));
}

/*
 * Cleanup a burning object if timer stopped.
 */
staticfn void
cleanup_burn(anything *arg, long expire_time)
{
    struct obj *obj = arg->a_obj;

    if (!obj->lamplit) {
        impossible("cleanup_burn: obj %s not lit", xname(obj));
        return;
    }

    del_light_source(LS_OBJECT, obj_to_any(obj));
    /* restore unused time */
    obj->age += expire_time - svm.moves;
    obj->lamplit = 0;

    if (obj->where == OBJ_INVENT)
        update_inventory();
}

void
do_storms(void)
{
    int nstrike;
    int x, y;
    int dirx, diry;
    int count;

    /* no lightning if not stormy level or too often, even then */
    if (!svl.level.flags.stormy || rn2(8))
        return;

    /* the number of strikes is 8-log2(nstrike) */
    for (nstrike = rnd(64); nstrike <= 64; nstrike *= 2) {
        count = 0;
        do {
            x = rnd(COLNO - 1);
            y = rn2(ROWNO);
        } while (++count < 100 && levl[x][y].typ != CLOUD);

        if (count < 100) {
            dirx = rn2(3) - 1;
            diry = rn2(3) - 1;
            if (dirx != 0 || diry != 0) {
                /* BZ_M_SPELL(BZ_OFS_AD(AD_ELEC)): monster LIGHTNING spell */
                gb.buzzer = 0; /* unspecified attacker */
                buzz(BZ_M_SPELL(BZ_OFS_AD(AD_ELEC)), 8, x, y, dirx, diry);
            }
        }
    }

    if (levl[u.ux][u.uy].typ == CLOUD) {
        /* Inside a cloud during a thunderstorm is deafening. */
        /* Even if already deaf, we sense the thunder's vibrations. */
        Soundeffect(se_kaboom_boom_boom, 80);
        pline("Kaboom!!!  Boom!!  Boom!!");
        incr_itimeout(&HDeaf, rn1(20, 30));
        disp.botl = TRUE;
        if (!u.uinvulnerable) {
            stop_occupation();
            nomul(-3);
            gm.multi_reason = "hiding from thunderstorm";
            gn.nomovemsg = 0;
        }
    } else
        You_hear("un grondement.");
}

/* -------------------------------------------------------------------------
 */
/*
 * Generic Timeout Functions.
 *
 * Interface:
 *
 * General:
 *  boolean start_timer(long timeout,short kind,short func_index,
 *                      anything *arg)
 *      Start a timer of kind 'kind' that will expire at time
 *      svm.moves+'timeout'.  Call the function at 'func_index'
 *      in the timeout table using argument 'arg'.  Return TRUE if
 *      a timer was started.  This places the timer on a list ordered
 *      "sooner" to "later".  If an object, increment the object's
 *      timer count.
 *
 *  long stop_timer(short func_index, anything *arg)
 *      Stop a timer specified by the (func_index, arg) pair.  This
 *      assumes that such a pair is unique.  Return the time the
 *      timer would have gone off.  If no timer is found, return 0.
 *      If an object, decrement the object's timer count.
 *
 *  long peek_timer(short func_index, anything *arg)
 *      Return time specified timer will go off (0 if no such timer).
 *
 *  void run_timers(void)
 *      Call timers that have timed out.
 *
 * Save/Restore:
 *  void save_timers(NHFILE *, int range)
 *      Save all timers of range 'range'.  Range is either global
 *      or local.  Global timers follow game play, local timers
 *      are saved with a level.  Object and monster timers are
 *      saved using their respective ids instead of pointers.
 *
 *  void restore_timers(NHFILE *, int range, long adjust)
 *      Restore timers of range 'range'.  If from a ghost pile,
 *      adjust the timeout by 'adjust'.  The object and monster
 *      ids are not restored until later.
 *
 *  void relink_timers(boolean ghostly)
 *      Relink all object and monster timers that had been saved
 *      using their object's or monster's id number.
 *
 * Object Specific:
 *  void obj_move_timers(struct obj *src, struct obj *dest)
 *      Reassign all timers from src to dest.
 *
 *  void obj_split_timers(struct obj *src, struct obj *dest)
 *      Duplicate all timers assigned to src and attach them to dest.
 *
 *  void obj_stop_timers(struct obj *obj)
 *      Stop all timers attached to obj.
 *
 *  boolean obj_has_timer(struct obj *object, short timer_type)
 *      Check whether object has a timer of type timer_type.
 */

staticfn const char *kind_name(short);
staticfn void print_queue(winid, timer_element *);
staticfn void insert_timer(timer_element *);
staticfn timer_element *remove_timer(timer_element **, short, ANY_P *);
staticfn void write_timer(NHFILE *, timer_element *);
staticfn boolean mon_is_local(struct monst *);
staticfn boolean timer_is_local(timer_element *);
staticfn int maybe_write_timer(NHFILE *, int, boolean);

/* If defined, then include names when printing out the timer queue */
#define VERBOSE_TIMER

typedef struct {
    timeout_proc f, cleanup;
#ifdef VERBOSE_TIMER
    const char *name;
#define TTAB(a, b, c) { a, b, c }
#else
#define TTAB(a, b, c) { a, b } /* ignore c for !VERBOSE_TIMER */
#endif
} ttable;

/*
 * Table of timeout functions, listed in order of enum timeout_types:
 */
static const ttable timeout_funcs[NUM_TIME_FUNCS] = {
    /* object timers */
    TTAB(rot_organic, (timeout_proc) 0, "rot_organic"),
    TTAB(rot_corpse, (timeout_proc) 0, "rot_corpse"),
    TTAB(revive_mon, (timeout_proc) 0, "revive_mon"),
    TTAB(zombify_mon, (timeout_proc) 0, "zombify_mon"),
    TTAB(burn_object, cleanup_burn, "burn_object"),
    TTAB(hatch_egg, (timeout_proc) 0, "hatch_egg"),
    TTAB(fig_transform, (timeout_proc) 0, "fig_transform"),
    TTAB(shrink_glob, (timeout_proc) 0, "shrink_glob"),
    /* level timers */
    TTAB(melt_ice_away, (timeout_proc) 0, "melt_ice_away"),
    /* currently no monster or global timers */
};
#undef TTAB

staticfn const char *
kind_name(short kind)
{
    switch (kind) {
    case TIMER_NONE:
        impossible("no timer type");
        return "none";
    case TIMER_LEVEL:
        return "level";
    case TIMER_GLOBAL:
        return "global";
    case TIMER_OBJECT:
        return "object";
    case TIMER_MONSTER:
        return "monster";
    }
    return "unknown";
}

staticfn void
print_queue(winid win, timer_element *base)
{
    timer_element *curr;
    char buf[BUFSZ];

    if (!base) {
        putstr(win, 0, " <vide>");
    } else {
        putstr(win, 0, "délai    id   type   appel");
        for (curr = base; curr; curr = curr->next) {
#ifdef VERBOSE_TIMER
            Sprintf(buf, " %4ld   %4ld  %-6s %s(%s)", curr->timeout,
                    (long) curr->tid, kind_name(curr->kind),
                    timeout_funcs[curr->func_index].name,
                    fmt_ptr((genericptr_t) curr->arg.a_void));
#else
            Sprintf(buf, " %4ld   %4ld  %-6s #%d(%s)", curr->timeout,
                    curr->tid, kind_name(curr->kind), curr->func_index,
                    fmt_ptr((genericptr_t) curr->arg.a_void));
#endif
            putstr(win, 0, buf);
        }
    }
}

/* the #timeout command */
int
wiz_timeout_queue(void)
{
    winid win;
    char buf[BUFSZ];
    const char *propname;
    long intrinsic;
    int i, p, count, longestlen, ln, specindx = 0;

    win = create_nhwindow(NHW_MENU); /* corner text window */
    if (win == WIN_ERR)
        return ECMD_OK;

    Sprintf(buf, "Temps actuel = %ld.", svm.moves);
    putstr(win, 0, buf);
    putstr(win, 0, "");
    putstr(win, 0, "File des délais actifs :");
    putstr(win, 0, "");
    print_queue(win, gt.timer_base);

    /* Timed properties:
     * check every one; the majority can't obtain temporary timeouts in
     * normal play but those can be forced via the #wizintrinsic command.
     */
    count = longestlen = 0;
    for (i = 0; (propname = propertynames[i].prop_name) != 0; ++i) {
        p = propertynames[i].prop_num;
        intrinsic = u.uprops[p].intrinsic;
        if (intrinsic & TIMEOUT) {
            ++count;
            if ((ln = (int) strlen(propname)) > longestlen)
                longestlen = ln;
        }
        if (specindx == 0 && p == COLD_RES) /* was FIRE_RES but has changed */
            specindx = i;
    }
    putstr(win, 0, "");
    if (!count) {
        putstr(win, 0, "Aucune propriété temporaire.");
    } else {
        putstr(win, 0, "Propriétés temporaires :");
        putstr(win, 0, "");
        for (i = 0; (propname = propertynames[i].prop_name) != 0; ++i) {
            p = propertynames[i].prop_num;
            intrinsic = u.uprops[p].intrinsic;
            if (intrinsic & TIMEOUT) {
                if (specindx > 0 && i >= specindx) {
                    putstr(win, 0, " -- modifiables seulement via #wizintrinsic --");
                    specindx = 0;
                }
                /* timeout value can be up to 16777215 (0x00ffffff) but
                   width of 4 digits should result in values lining up
                   almost all the time (if/when they don't, it won't
                   look nice but the information will still be accurate) */
                Sprintf(buf, " %*s %4ld", -longestlen, propname,
                        (intrinsic & TIMEOUT));
                putstr(win, 0, buf);
            }
        }
    }
    if (u.uswldtim) {
        putstr(win, 0, "");
        /* decremented when engulfer makes a move, so can last longer than
           the number of turns reported if engulfer is slow */
        Sprintf(buf, "Compte à rebours d'engloutissement : %u.", u.uswldtim);
        putstr(win, 0, buf);
    }
    if (u.uinvault) {
        putstr(win, 0, "");
        Sprintf(buf, "Compteur de chambre forte : %d.", u.uinvault);
        putstr(win, 0, buf);
    }
    if (any_visible_region()) {
        visible_region_summary(win);
    }
    if (svl.level.flags.stasis_until >= svm.moves) {
        putstr(win, 0, "");
        Sprintf(buf, "Niveau interdit de téléportation pour %ld %s.",
                svl.level.flags.stasis_until - svm.moves + 1L,
                (svl.level.flags.stasis_until - svm.moves > 0L)
                  ? "tours" : "tour de plus");
        putstr(win, 0, buf);
    }
    display_nhwindow(win, FALSE);
    destroy_nhwindow(win);

    return ECMD_OK;
}

void
timer_sanity_check(void)
{
    timer_element *curr;
    unsigned long t_id;
    coordxy x, y;

    for (curr = gt.timer_base; curr; curr = curr->next) {
        t_id = curr->tid;
        switch (curr->kind) {
        case TIMER_OBJECT: {
            /* TODO? verify that the timer type is attached to applicable
               object (egg for hatch, glob for shrink, and so forth) */
            struct obj *obj = curr->arg.a_obj, *top;
            char *obj_adr = fmt_ptr((genericptr_t) obj);
            int owhere = obj->where;

            if (obj->timed == 0) {
                impossible("timer sanity: untimed obj %s, timer %lu",
                           obj_adr, t_id);
            }
            x = y = 0;
            /* if obj is in a container, possibly a nested one, figure out
               where the outermost container is */
            for (top = obj; top; top = top->ocontainer)
                if ((owhere = top->where) != OBJ_CONTAINED)
                    break;
            assert(top != NULL);
            if (owhere == OBJ_MIGRATING
                || (owhere == OBJ_MINVENT && !mon_is_local(top->ocarry))) {
                /* migrating directly or carried by migrating monster */
                ; /* not able to validate location so skip checks */
            } else if (!get_obj_location(obj, &x, &y,
                                         CONTAINED_TOO | BURIED_TOO)) {
                /* free? or on a shop's used-up bill? */
                impossible(
                    "timer sanity: can't locate obj %s [where=%d], timer %lu",
                           obj_adr, obj->where, t_id);
            } else if (!isok(x, y)) {
                impossible(
              "timer sanity: obj %s [where=%d] located at <%d,%d>, timer %lu",
                           obj_adr, obj->where, x, y, t_id);
            }
            break;
        }
        case TIMER_MONSTER:
            impossible("timer sanity: unexpected monster timer %lu", t_id);
            break;
        case TIMER_LEVEL: {
            long lwhere = curr->arg.a_long;

            x = (coordxy) ((lwhere >> 16) & 0xFFFF);
            y = (coordxy) (lwhere & 0xFFFF);
            if (isok(x, y)) {
                /* replicate isok() in order to convince static analysis
                   that the decoding via '& 0xFFFF' hasn't produced a value
                   too big for levl[][] and that the cast to a narrower type
                   hasn't intruded on the sign bit to yield a negative value;
                   the analyzer isn't aware that isok() filters such things */
                assert(x > 0 && x < COLNO && y >= 0 && y < ROWNO);

                if (curr->func_index == MELT_ICE_AWAY && !is_ice(x, y)
                    /* the terrain under the span of an open drawbridge might
                       be frozen moat; is_ice() only checks for that when
                       the drawbridge is closed (and terrain here would be
                       DRAWBRIGE_UP) */
                    && !(levl[x][y].typ == DRAWBRIDGE_DOWN
                         && (levl[x][y].drawbridgemask & DB_UNDER) == DB_ICE))
                    impossible(
                         "timer sanity: melt timer %lu on non-ice %d <%d,%d>",
                               t_id, levl[x][y].typ, x, y);
            } else {
                impossible("timer sanity: spot timer %lu at <%d,%d>",
                           t_id, x, y);
            }
            break;
        }
        case TIMER_GLOBAL:
            impossible("timer sanity: unexpected global timer %lu", t_id);
            break;
        default:
            impossible("timer sanity: unknown timer %lu, type: %d",
                       t_id, curr->kind);
            break;
        }
    }
}

/*
 * Pick off timeout elements from the global queue and call their functions.
 * Do this until their time is less than or equal to the move count.
 */
void
run_timers(void)
{
    timer_element *curr;

    /*
     * Always use the first element.  Elements may be added or deleted at
     * any time.  The list is ordered; we are done when the first element
     * is in the future.
     */
    while (gt.timer_base && gt.timer_base->timeout <= svm.moves) {
        curr = gt.timer_base;
        gt.timer_base = curr->next;

        if (curr->kind == TIMER_OBJECT)
            (curr->arg.a_obj)->timed--;
        (*timeout_funcs[curr->func_index].f)(&curr->arg, curr->timeout);
        (void) memset((genericptr_t) curr, 0, sizeof(timer_element));
        free((genericptr_t) curr);
    }
}

/*
 * Start a timer.  Return TRUE if successful.
 */
boolean
start_timer(
    long when,
    short kind,
    short func_index,
    anything *arg)
{
    timer_element *gnu, *dup;

    if (kind <= TIMER_NONE || kind >= NUM_TIMER_KINDS
        || func_index < 0 || func_index >= NUM_TIME_FUNCS)
        panic("start_timer (%s: %d)", kind_name(kind), (int) func_index);

    /* fail if <arg> already has a <func_index> timer running */
    for (dup = gt.timer_base; dup; dup = dup->next)
        if (dup->kind == kind
            && dup->func_index == func_index
            && dup->arg.a_void == arg->a_void)
            break;
    if (dup) {
        char idbuf[QBUFSZ];

#ifdef VERBOSE_TIMER
        Sprintf(idbuf, "%s timer", timeout_funcs[func_index].name);
#else
        Sprintf(idbuf, "%s timer (%d)", kind_name(kind), (int) func_index);
#endif
        impossible("Attempted to start duplicate %s, aborted.", idbuf);
        return FALSE;
    }

    gnu = (timer_element *) alloc(sizeof *gnu);
    (void) memset((genericptr_t) gnu, 0, sizeof *gnu);
    gnu->next = 0;
    gnu->tid = svt.timer_id++;
    gnu->timeout = svm.moves + when;
    gnu->kind = kind;
    gnu->needs_fixup = 0;
    gnu->func_index = func_index;
    gnu->arg = *arg;
    insert_timer(gnu);

    if (kind == TIMER_OBJECT) /* increment object's timed count */
        (arg->a_obj)->timed++;

    return TRUE;
}

/*
 * Remove the timer from the current list and free it up.  Return the time
 * remaining until it would have gone off, 0 if not found.
 */
long
stop_timer(short func_index, anything *arg)
{
    timeout_proc cleanup_func;
    timer_element *doomed;
    long timeout;

    doomed = remove_timer(&gt.timer_base, func_index, arg);

    if (doomed) {
        timeout = doomed->timeout;
        if (doomed->kind == TIMER_OBJECT)
            (arg->a_obj)->timed--;
        if ((cleanup_func = timeout_funcs[doomed->func_index].cleanup) != 0)
            (*cleanup_func)(arg, timeout);
        (void) memset((genericptr_t) doomed, 0, sizeof(timer_element));
        free((genericptr_t) doomed);
        return (timeout - svm.moves);
    }
    return 0L;
}

/*
 * Find the timeout of specified timer; return 0 if none.
 */
long
peek_timer(short type, anything *arg)
{
    timer_element *curr;

    for (curr = gt.timer_base; curr; curr = curr->next) {
        if (curr->func_index == type && curr->arg.a_void == arg->a_void)
            return curr->timeout;
    }
    return 0L;
}

/*
 * Move all object timers from src to dest, leaving src untimed.
 */
void
obj_move_timers(struct obj *src, struct obj *dest)
{
    int count;
    timer_element *curr;

    for (count = 0, curr = gt.timer_base; curr; curr = curr->next)
        if (curr->kind == TIMER_OBJECT && curr->arg.a_obj == src) {
            curr->arg.a_obj = dest;
            dest->timed++;
            count++;
        }
    if (count != src->timed)
        panic("obj_move_timers");
    src->timed = 0;
}

/*
 * Find all object timers and duplicate them for the new object "dest".
 */
void
obj_split_timers(struct obj *src, struct obj *dest)
{
    timer_element *curr, *next_timer = 0;

    for (curr = gt.timer_base; curr; curr = next_timer) {
        next_timer = curr->next; /* things may be inserted */
        if (curr->kind == TIMER_OBJECT && curr->arg.a_obj == src) {
            (void) start_timer(curr->timeout - svm.moves, TIMER_OBJECT,
                               curr->func_index, obj_to_any(dest));
        }
    }
}

/*
 * Stop all timers attached to this object.  We can get away with this because
 * all object pointers are unique.
 */
void
obj_stop_timers(struct obj *obj)
{
    timeout_proc cleanup_func;
    timer_element *curr, *prev, *next_timer = 0;

    for (prev = 0, curr = gt.timer_base; curr; curr = next_timer) {
        next_timer = curr->next;
        if (curr->kind == TIMER_OBJECT && curr->arg.a_obj == obj) {
            if (prev)
                prev->next = curr->next;
            else
                gt.timer_base = curr->next;
            if ((cleanup_func = timeout_funcs[curr->func_index].cleanup) != 0)
                (*cleanup_func)(&curr->arg, curr->timeout);
            (void) memset((genericptr_t) curr, 0, sizeof(timer_element));
            free((genericptr_t) curr);
        } else {
            prev = curr;
        }
    }
    obj->timed = 0;
}

/*
 * Check whether object has a timer of type timer_type.
 */
boolean
obj_has_timer(struct obj *object, short timer_type)
{
    long timeout = peek_timer(timer_type, obj_to_any(object));

    return (boolean) (timeout != 0L);
}

/*
 * Stop all timers of index func_index at this spot.
 *
 */
void
spot_stop_timers(coordxy x, coordxy y, short func_index)
{
    timeout_proc cleanup_func;
    timer_element *curr, *prev, *next_timer = 0;
    long where = (((long) x << 16) | ((long) y));

    for (prev = 0, curr = gt.timer_base; curr; curr = next_timer) {
        next_timer = curr->next;
        if (curr->kind == TIMER_LEVEL && curr->func_index == func_index
            && curr->arg.a_long == where) {
            if (prev)
                prev->next = curr->next;
            else
                gt.timer_base = curr->next;
            if ((cleanup_func = timeout_funcs[curr->func_index].cleanup) != 0)
                (*cleanup_func)(&curr->arg, curr->timeout);
            (void) memset((genericptr_t) curr, 0, sizeof(timer_element));
            free((genericptr_t) curr);
        } else {
            prev = curr;
        }
    }
}

/*
 * When is the spot timer of type func_index going to expire?
 * Returns 0L if no such timer.
 */
long
spot_time_expires(coordxy x, coordxy y, short func_index)
{
    timer_element *curr;
    long where = (((long) x << 16) | ((long) y));

    for (curr = gt.timer_base; curr; curr = curr->next) {
        if (curr->kind == TIMER_LEVEL && curr->func_index == func_index
            && curr->arg.a_long == where)
            return curr->timeout;
    }
    return 0L;
}

long
spot_time_left(coordxy x, coordxy y, short func_index)
{
    long expires = spot_time_expires(x, y, func_index);
    return (expires > 0L) ? expires - svm.moves : 0L;
}

/* Insert timer into the global queue */
staticfn void
insert_timer(timer_element *gnu)
{
    timer_element *curr, *prev;

    for (prev = 0, curr = gt.timer_base; curr; prev = curr, curr = curr->next)
        if (curr->timeout >= gnu->timeout)
            break;

    gnu->next = curr;
    if (prev)
        prev->next = gnu;
    else
        gt.timer_base = gnu;
}

staticfn timer_element *
remove_timer(
    timer_element **base,
    short func_index,
    anything *arg)
{
    timer_element *prev, *curr;

    for (prev = 0, curr = *base; curr; prev = curr, curr = curr->next)
        if (curr->func_index == func_index && curr->arg.a_void == arg->a_void)
            break;

    if (curr) {
        if (prev)
            prev->next = curr->next;
        else
            *base = curr->next;
    }

    return curr;
}

staticfn void
write_timer(NHFILE *nhfp, timer_element *timer)
{
    anything arg_save;

    arg_save = cg.zeroany;
    switch (timer->kind) {
    case TIMER_GLOBAL:
    case TIMER_LEVEL:
        /* assume no pointers in arg */
        Sfo_fe(nhfp, timer, "timer");
        break;

    case TIMER_OBJECT:
        if (timer->needs_fixup) {
            Sfo_fe(nhfp, timer, "timer");
        } else {
            /* replace object pointer with id */
            arg_save.a_obj = timer->arg.a_obj;
            timer->arg = cg.zeroany;
            timer->arg.a_uint = (arg_save.a_obj)->o_id;
            timer->needs_fixup = 1;
            Sfo_fe(nhfp, timer, "timer");
            timer->arg.a_obj = arg_save.a_obj;
            timer->needs_fixup = 0;
        }
        break;

    case TIMER_MONSTER:
        if (timer->needs_fixup) {
            Sfo_fe(nhfp, timer, "timer");
        } else {
            /* replace monster pointer with id */
            arg_save.a_monst = timer->arg.a_monst;
            timer->arg = cg.zeroany;
            timer->arg.a_uint = (arg_save.a_monst)->m_id;
            timer->needs_fixup = 1;
            Sfo_fe(nhfp, timer, "timer");
            timer->arg.a_monst = arg_save.a_monst;
            timer->needs_fixup = 0;
        }
        break;

    default:
        panic("write_timer");
        break;
    }
}

DISABLE_WARNING_UNREACHABLE_CODE

/*
 * Return TRUE if the object will stay on the level when the level is
 * saved.
 */
boolean
obj_is_local(struct obj *obj)
{
    switch (obj->where) {
    case OBJ_INVENT:
    case OBJ_MIGRATING:
        return FALSE;
    case OBJ_FLOOR:
    case OBJ_BURIED:
        return TRUE;
    case OBJ_CONTAINED:
        return obj_is_local(obj->ocontainer);
    case OBJ_MINVENT:
        return mon_is_local(obj->ocarry);
    }
    panic("obj_is_local");
    /*NOTREACHED*/
    return FALSE;
}

/*
 * Return TRUE if the given monster will stay on the level when the
 * level is saved.
 */
staticfn boolean
mon_is_local(struct monst *mon)
{
    struct monst *curr;

    for (curr = gm.migrating_mons; curr; curr = curr->nmon)
        if (curr == mon)
            return FALSE;
    /* `gm.mydogs' is used during level changes, never saved and restored */
    for (curr = gm.mydogs; curr; curr = curr->nmon)
        if (curr == mon)
            return FALSE;
    return TRUE;
}

/*
 * Return TRUE if the timer is attached to something that will stay on the
 * level when the level is saved.
 */
staticfn boolean
timer_is_local(timer_element *timer)
{
    switch (timer->kind) {
    case TIMER_LEVEL:
        return TRUE;
    case TIMER_GLOBAL:
        return FALSE;
    case TIMER_OBJECT:
        return obj_is_local(timer->arg.a_obj);
    case TIMER_MONSTER:
        return mon_is_local(timer->arg.a_monst);
    }
    panic("timer_is_local");
    /*NOTREACHED*/
    return FALSE;
}

RESTORE_WARNING_UNREACHABLE_CODE

/*
 * Part of the save routine.  Count up the number of timers that would
 * be written.  If write_it is true, actually write the timer.
 */
staticfn int
maybe_write_timer(NHFILE *nhfp, int range, boolean write_it)
{
    int count = 0;
    timer_element *curr;

    for (curr = gt.timer_base; curr; curr = curr->next) {
        if (range == RANGE_GLOBAL) {
            /* global timers */

            if (!timer_is_local(curr)) {
                count++;
                if (write_it) write_timer(nhfp, curr);
            }
        } else {
            /* local timers */

            if (timer_is_local(curr)) {
                count++;
                if (write_it) write_timer(nhfp, curr);
            }
        }
    }

    return count;
}


/*
 * Save part of the timer list.  The parameter 'range' specifies either
 * global or level timers to save.  The timer ID is saved with the global
 * timers.
 *
 * Global range:
 *      + timeouts that follow the hero (global)
 *      + timeouts that follow obj & monst that are migrating
 *
 * Level range:
 *      + timeouts that are level-specific (e.g. storms)
 *      + timeouts that stay with the level (obj & monst)
 */
void
save_timers(NHFILE *nhfp, int range)
{
    timer_element *curr, *prev, *next_timer = 0;
    int count;

    if (update_file(nhfp)) {
        if (range == RANGE_GLOBAL) {
            Sfo_ulong(nhfp, &svt.timer_id, "timer-timer_id");
        }
        count = maybe_write_timer(nhfp, range, FALSE);
        Sfo_int(nhfp, &count, "timer-timer_count");
        (void) maybe_write_timer(nhfp, range, TRUE);
    }

    if (release_data(nhfp)) {
        for (prev = 0, curr = gt.timer_base; curr; curr = next_timer) {
            next_timer = curr->next; /* in case curr is removed */

            if (!(!!(range == RANGE_LEVEL) ^ !!timer_is_local(curr))) {
                if (prev)
                    prev->next = curr->next;
                else
                    gt.timer_base = curr->next;
                (void) memset((genericptr_t) curr, 0, sizeof(timer_element));
                free((genericptr_t) curr);
                /* prev stays the same */
            } else {
                prev = curr;
            }
        }
    }
}
#endif /* !SFCTOOL */

/*
 * Pull in the structures from disk, but don't recalculate the object and
 * monster pointers.
 */
void
restore_timers(NHFILE *nhfp, int range, long adjust)
{
    int count = 0;
    timer_element *curr;
    boolean ghostly = (nhfp->ftype == NHF_BONESFILE); /* from a ghost level */

    if (range == RANGE_GLOBAL) {
        Sfi_ulong(nhfp, &svt.timer_id, "timer-timer_id");
    }

    /* restore elements */
    Sfi_int(nhfp, &count, "timer-timer_count");
    while (count-- > 0) {
        curr = (timer_element *) alloc(sizeof(timer_element));
        Sfi_fe(nhfp, curr, "timer");
        if (ghostly)
            curr->timeout += adjust;
#ifndef SFCTOOL
        insert_timer(curr);
#endif
    }
}

#ifndef SFCTOOL
DISABLE_WARNING_FORMAT_NONLITERAL

/* to support '#stats' wizard-mode command */
void
timer_stats(const char *hdrfmt, char *hdrbuf, long *count, long *size)
{
    timer_element *te;

    Sprintf(hdrbuf, hdrfmt, (long) sizeof (timer_element));
    *count = *size = 0L;
    for (te = gt.timer_base; te; te = te->next) {
        ++*count;
        *size += (long) sizeof *te;
    }
}

RESTORE_WARNING_FORMAT_NONLITERAL

/* reset all timers that are marked for resetting */
void
relink_timers(boolean ghostly)
{
    timer_element *curr;
    unsigned nid;

    for (curr = gt.timer_base; curr; curr = curr->next) {
        if (curr->needs_fixup) {
            if (curr->kind == TIMER_OBJECT) {
                if (ghostly) {
                    if (!lookup_id_mapping(curr->arg.a_uint, &nid))
                        panic("relink_timers 1");
                } else
                    nid = curr->arg.a_uint;
                curr->arg.a_obj = find_oid(nid);
                if (!curr->arg.a_obj)
                    panic("can't find o_id %d", nid);
                curr->needs_fixup = 0;
            } else if (curr->kind == TIMER_MONSTER) {
                panic("relink_timers: no monster timer implemented");
            } else
                panic("relink_timers 2");
        }
    }
}
#endif /* !SFCTOOL */

/*timeout.c*/
