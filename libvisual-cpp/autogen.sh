#!/bin/sh

libtoolize -f && \
aclocal && \
autoheader && \
automake -a && \
autoconf
