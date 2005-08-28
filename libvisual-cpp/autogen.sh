#!/bin/sh

aclocal && \
autoheader && \
libtoolize && \
autoconf && \
automake -a
