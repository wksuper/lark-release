#!/bin/bash

PREFIX=/usr/local
BINDIR=$PREFIX/bin
LIBDIR=$PREFIX/lib
INCDIR=$PREFIX/include
LKINCDIR=$INCDIR/lark

if [ "$(uname -s)" = "Darwin" ]; then
    if [ "$(uname -m)" = "x86_64" ]; then
        ARCH=x86_64-apple-darwin
    fi
elif [ "$(uname -s)" = "Linux" ]; then
	if [ "$(uname -m)" = "aarch64" ]; then
		ARCH=aarch64-linux-gnu
	elif [ "$(uname -m)" = "x86_64" ]; then
		ARCH=x86_64-linux-gnu
	elif [ "$(uname -m)" = "armv7l" ]; then
		ARCH=arm-linux-gnueabihf
	fi
fi
