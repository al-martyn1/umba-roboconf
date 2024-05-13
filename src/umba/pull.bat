git subtree pull --prefix=src/luart     http://alexander_martynov@gitblit.dep111.rtc.local/r/~alexander_martynov/luart.git      master   --squash
git subtree pull --prefix=src/mcusp     http://alexander_martynov@gitblit.dep111.rtc.local/r/~alexander_martynov/mcusp.git      master   --squash
git subtree pull --prefix=src/retarget  http://alexander_martynov@gitblit.dep111.rtc.local/r/~alexander_martynov/retarget.git   external --squash
git gc --prune=now
