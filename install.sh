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
install $ARCH/bin/lkdb $BINDIR/

# *.so / *.dylib / *.dll
for solib in $ARCH/lib/*
do
    install $solib $LIBDIR/
done

# *.h
install include/klogging.h $INCDIR/
install -d $LKINCDIR
for header in include/lark/*
do
    install $header $LKINCDIR/
done
