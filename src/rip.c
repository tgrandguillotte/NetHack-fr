/* NetHack 5.0	rip.c	$NHDT-Date: 1781973064 2026/06/20 16:31:04 $  $NHDT-Branch: NetHack-5.0 $:$NHDT-Revision: 1.49 $ */
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/*-Copyright (c) Robert Patrick Rankin, 2017. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"

/* Defining TEXT_TOMBSTONE causes genl_outrip() to exist, but it doesn't
   necessarily have to be used by a binary with multiple window-ports */

#if defined(TTY_GRAPHICS) || defined(X11_GRAPHICS) || defined(GEM_GRAPHICS) \
    || defined(DUMPLOG) || defined(CURSES_GRAPHICS) || defined(SHIM_GRAPHICS) \
    || defined(AMII_GRAPHICS)
#define TEXT_TOMBSTONE
#endif
#if defined(mac) || defined(__BEOS__)
#ifndef TEXT_TOMBSTONE
#define TEXT_TOMBSTONE
#endif
#endif

#ifdef TEXT_TOMBSTONE
staticfn void center(int, char *);

#ifndef NH320_DEDICATION
/* A normal tombstone for end of game display. */
static const char *const rip_txt[] = {
    "                       ----------",
    "                      /          \\",
    "                     /   REPOSE   \\",
    "                    /      EN      \\",
    "                   /      PAIX      \\",
    "                  /                  \\",
    "                  |                  |", /* Name of player */
    "                  |                  |", /* Amount of $ */
    "                  |                  |", /* Type of death */
    "                  |                  |", /* . */
    "                  |                  |", /* . */
    "                  |                  |", /* . */
    "                  |       1001       |", /* Real year of death */
    "                 *|     *  *  *      | *",
    "        _________)/\\\\_//(\\/(/\\)/\\//\\/|_)_______", 0
};
#define STONE_LINE_CENT 28 /* char[] element of center of stone face */
#else                      /* NH320_DEDICATION */
/* NetHack 3.2.x displayed a dual tombstone as a tribute to Izchak. */
static const char *const rip_txt[] = {
    "              ----------                      ----------",
    "             /          \\                    /          \\",
    "            /   REPOSE   \\                  /   Cette    \\",
    "           /      EN      \\                /  version de  \\",
    "          /      PAIX      \\              /  NetHack est   \\",
    "         /                  \\            /   dédiée à la    \\",
    "         |                  |            |   mémoire de     |",
    "         |                  |            |                  |",
    "         |                  |            |  Izchak Miller   |",
    "         |                  |            |   1935 - 1994    |",
    "         |                  |            |                  |",
    "         |                  |            |  Monté au ciel   |",
    "         |       1001       |            |                  |",
    "      *  |     *  *  *      | *        * |      *  *  *     | *",
    (" _____)/\\|\\__//(\\/(/\\)/\\//\\/|_)___"
     "_____)/|\\\\_/_/(\\/(/\\)/\\/\\/|_)____"),
    0
};
#define STONE_LINE_CENT 19 /* char[] element of center of stone face */
#endif                     /* NH320_DEDICATION */
#define STONE_LINE_LEN  16 /* # chars that fit on one line
                            * (note 1 ' ' border)           */
#define NAME_LINE  6 /* *char[] line # for player name */
#define GOLD_LINE  7 /* *char[] line # for amount of gold */
#define DEATH_LINE 8 /* *char[] line # for death description */
#define YEAR_LINE 12 /* *char[] line # for year */

/* number of display columns of a UTF-8 string */
staticfn int
rip_cols(const char *s)
{
    int n = 0;

    for (; *s; s++)
        if ((*s & 0xC0) != 0x80)
            n++;
    return n;
}

/* byte offset just past the first 'cols' display columns of s */
staticfn int
rip_bytes(const char *s, int cols)
{
    int i = 0;

    while (s[i] && cols > 0) {
        i++;
        while ((s[i] & 0xC0) == 0x80)
            i++;
        cols--;
    }
    return i;
}

/* replace the middle of tombstone line 'line' by text (which may contain
   multi-byte UTF-8 characters); the line is reallocated as needed */
staticfn void
center(int line, char *text)
{
    char *old = gr.rip[line], *nw;
    int cols = rip_cols(text), start, oldlen = (int) strlen(old);

    start = STONE_LINE_CENT - ((cols + 1) >> 1);
    if (start < 0)
        start = 0;
    if (start + cols > oldlen)
        cols = oldlen - start;
    nw = (char *) alloc((unsigned) (oldlen + strlen(text) + 1));
    (void) memcpy(nw, old, (size_t) start);
    Strcpy(nw + start, text);
    Strcat(nw, old + start + cols);
    free((genericptr_t) old);
    gr.rip[line] = nw;
}

void
genl_outrip(winid tmpwin, int how, time_t when)
{
    char **dp;
    char *dpx;
    char buf[BUFSZ];
    int x;
    int line, year;
    long cash;

    gr.rip = dp = (char **) alloc(sizeof(rip_txt));
    for (x = 0; rip_txt[x]; ++x)
        dp[x] = dupstr(rip_txt[x]);
    dp[x] = (char *) 0;

    /* Put name on stone */
    Sprintf(buf, "%.*s", (int) STONE_LINE_LEN, svp.plname);
    center(NAME_LINE, buf);

    /* Put $ on stone */
    cash = max(gd.done_money, 0L);
    /* arbitrary upper limit; practical upper limit is quite a bit less */
    if (cash > 999999999L)
        cash = 999999999L;
    Sprintf(buf, "%ld Au", cash);
    center(GOLD_LINE, buf);

    /* Put together death description */
    formatkiller(buf, sizeof buf, how, FALSE);

    /* Put death type on stone */
    for (line = DEATH_LINE, dpx = buf; line < YEAR_LINE; line++) {
        char tmpchar;
        int i, i0 = (int) strlen(dpx);
        int lim = rip_bytes(dpx, STONE_LINE_LEN);

        if (rip_cols(dpx) > STONE_LINE_LEN) {
            boolean found = FALSE;

            for (i = lim; i > 0; --i)
                if (dpx[i] == ' ') {
                    i0 = i;
                    found = TRUE;
                    break;
                }
            if (!found)
                i0 = lim;
        }
        tmpchar = dpx[i0];
        dpx[i0] = 0;
        center(line, dpx);
        if (tmpchar != ' ') {
            dpx[i0] = tmpchar;
            dpx = &dpx[i0];
        } else
            dpx = &dpx[i0 + 1];
    }

    /* Put year on stone */
    year = (int) ((yyyymmdd(when) / 10000L) % 10000L);
    Sprintf(buf, "%4d", year);
    center(YEAR_LINE, buf);

#ifdef DUMPLOG
    if (tmpwin == 0)
        dump_forward_putstr(0, 0, "Fin de la partie :", TRUE);
    else
#endif
        putstr(tmpwin, 0, "");

    for (; *dp; dp++)
        putstr(tmpwin, 0, *dp);

    putstr(tmpwin, 0, "");
#ifdef DUMPLOG
    if (tmpwin != 0)
#endif
        putstr(tmpwin, 0, "");

    for (x = 0; rip_txt[x]; x++) {
        free((genericptr_t) gr.rip[x]);
    }
    free((genericptr_t) gr.rip);
    gr.rip = 0;
}

#endif /* TEXT_TOMBSTONE */

/*rip.c*/
