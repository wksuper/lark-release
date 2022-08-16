#!/bin/bash

source ./env.sh

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
