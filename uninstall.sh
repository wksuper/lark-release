#!/bin/bash

source ./env.sh

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
rm -f $INCDIR/klogging.h
