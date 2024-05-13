git subtree add --prefix=src/luart     http://alexander_martynov@gitblit.dep111.rtc.local/r/~alexander_martynov/luart.git      master   --squash
git subtree add --prefix=src/cmsis     http://alexander_martynov@gitblit.dep111.rtc.local/r/~alexander_martynov/cmsis.git      master   --squash
git subtree add --prefix=src/mcusp     http://alexander_martynov@gitblit.dep111.rtc.local/r/~alexander_martynov/mcusp.git      master   --squash
git subtree add --prefix=src/retarget  http://alexander_martynov@gitblit.dep111.rtc.local/r/~alexander_martynov/retarget.git   external --squash
git subtree add --prefix=src/spl       http://alexander_martynov@gitblit.dep111.rtc.local/r/~alexander_martynov/spl.git        master   --squash
git subtree add --prefix=src/global_macros  http://alexander_martynov@gitblit.dep111.rtc.local/r/~alexander_martynov/global_macros.git    external
git gc --prune=now
