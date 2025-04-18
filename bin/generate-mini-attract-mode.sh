#!/bin/bash

# run from project root directory

shrs=$(mktemp)
cat res/SS/SHR*.CONF |
    tr -d "\r" |
    sort |
    uniq >> "$shrs"

dhgrs=$(mktemp)
cat res/SS/ACTDHGR*.CONF |
    tr -d "\r" |
    sort |
    uniq >> "$dhgrs"

hgrs=$(mktemp)
hgrtmp=$(comm -23 <(ls res/SS/ACT*) <(ls res/SS/ACTDHGR*))
hgrsrcs=$(comm -23 <(echo "$hgrtmp") <(ls res/SS/ACTDGR*))
cat $hgrsrcs |
    tr -d "\r" |
    sort |
    uniq >> "$hgrs"

grs=$(mktemp)
cat res/SS/ACTGR*.CONF |
    tr -d "\r" |
    sort |
    uniq >> "$grs"

dgrs=$(mktemp)
cat res/SS/ACTDGR*.CONF |
    tr -d "\r" |
    sort |
    uniq >> "$dgrs"

demos=$(mktemp)
cat res/ATTRACT.CONF |
    tr -d "\r" |
    grep "=0$" >> "$demos"

cat res/GAMES.CONF |
    tr -d "\r" |
    grep "," |
    grep -v "^#" |
    cut -d"," -f2 |
    cut -d"=" -f1 | \
    while read game; do
        # initialize attract mode configuration file for this game
        echo -e "#\n# Attract mode for $game\n# This file is automatically generated\n#\n" > /tmp/g

        # add box art, if any
        cat "$shrs" |
            egrep "(^|=)""$game""$" |
            cut -d"=" -f1 |
            sed -e "s/$/=C/g" >> /tmp/g

        # add DHGR action screenshots, if any
        cat "$dhgrs" |
            egrep "(^|=)""$game""$" |
            cut -d"=" -f1 |
            sed -e "s/$/=B/g" >> /tmp/g

        # add HGR action screenshots, if any
        cat "$hgrs" |
            egrep "(^|=)""$game""$" |
            cut -d"=" -f1 |
            sed -e "s/$/=A/g" >> /tmp/g

        # add GR action screenshots, if any
        cat "$grs" |
            egrep "(^|=)""$game""$" |
            cut -d"=" -f1 |
            sed -e "s/$/=D/g" >> /tmp/g

        # add DGR action screenshots, if any
        cat "$dgrs" |
            egrep "(^|=)""$game""$" |
            cut -d"=" -f1 |
            sed -e "s/$/=E/g" >> /tmp/g

        # add self-running demo, if any
        cat "$demos" |
            grep "^$game=0" >> /tmp/g

        cat /tmp/g > res/ATTRACT/"$game"

        # clean up
        rm /tmp/g
    done

rm "$demos"
rm "$dgrs"
rm "$grs"
rm "$hgrs"
rm "$dhgrs"
rm "$shrs"
