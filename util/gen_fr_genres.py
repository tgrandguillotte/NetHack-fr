#!/usr/bin/env python3
# Genere src/fr_genres.h (dictionnaire trie des genres) a partir de
# src/fr_genres.txt et de listes supplementaires passees en arguments.
# Format des listes : "mot<espace ou tabulation>m|f" ; '#' = commentaire.
import sys, os
here = os.path.dirname(os.path.abspath(__file__))
src = os.path.join(here, '..', 'src')
d = {}
for fn in [os.path.join(src, 'fr_genres.txt')] + sys.argv[1:]:
    if not os.path.exists(fn):
        continue
    for line in open(fn, encoding='utf-8'):
        line = line.split('#')[0].strip()
        if not line:
            continue
        parts = line.split()
        if len(parts) < 2 or parts[-1] not in ('m', 'f'):
            continue
        mot = ' '.join(parts[:-1]).lower()
        if ' ' in mot:
            continue
        d.setdefault(mot, parts[-1])
# reecrire la liste maitresse fusionnee
with open(os.path.join(src, 'fr_genres.txt'), 'w', encoding='utf-8') as f:
    f.write('# Genre grammatical des noms (m/f). Source de src/fr_genres.h,\n'
            '# regenerer avec util/gen_fr_genres.py\n')
    for k in sorted(d, key=lambda s: s.encode('utf-8')):
        f.write('%s %s\n' % (k, d[k]))
with open(os.path.join(src, 'fr_genres.h'), 'w', encoding='utf-8') as f:
    f.write('/* fr_genres.h : genre des noms francais. GENERE par '
            'util/gen_fr_genres.py a partir de src/fr_genres.txt */\n')
    for k in sorted(d, key=lambda s: s.encode('utf-8')):
        f.write('    { "%s", \'%s\' },\n' % (k.replace('"', '\\"'), d[k]))
print(len(d), 'mots')
