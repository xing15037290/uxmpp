#!/bin/sh

if [ -f Makefile ]; then
    make distclean
fi

rm -rf Makefile.in aclocal.m4 autom4te.cache config.guess config.sub configure depcomp install-sh ltmain.sh m4 missing src/Makefile.in src/config.h.in src/config.h.in~ test/Makefile.in doc/Makefile.in ar-lib
