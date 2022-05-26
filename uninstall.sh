#!/bin/bash

PREFIX=/usr/local
BINDIR=$PREFIX/bin
LIBDIR=$PREFIX/lib
INCDIR=$PREFIX/include
LKINCDIR=$INCDIR/lark

if [ "$(uname -s)" = "Darwin" ]; then
    ARCH=x86_64-apple-darwin
elif [ "$(uname -s)" = "Linux" ]; then
    ARCH=x86_64-linux-gnu
fi

# exe lkdb
rm -f $BINDIR/lkdb

# *.so / *.dylib / *.dll
SOLIBS=`ls $ARCH/lib`
for solib in $SOLIBS
do
    rm -f $LIBDIR/$solib
done

# *.h
rm -rf $LKINCDIR
rm -rf $INCDIR/klogging.h
