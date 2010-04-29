#!/bin/sh

libtoolize -f && \
aclocal -I m4 && \
autoheader && \
automake -a && \
autoconf
