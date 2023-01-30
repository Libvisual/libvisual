#! /usr/bin/env bash
# Run this to generate all the initial makefiles, etc.
# Mimics gnome-autogen.sh of gnome-common to some extent.

set -e -u

autoreconf --install --verbose --force

if [[ -z "${NOCONFIGURE:-}" ]]; then
    if [[ "$#" == 0 ]]; then
        echo >&2
        echo "**Warning**: I am going to run './configure' with no arguments." >&2
        echo "             If you wish to pass any to it, please specify them on the" >&2
        echo "             '$0' command line." >&2
        echo >&2
    fi

    exec ./configure "$@"
else
    echo 'Skipping configure process.'
fi
