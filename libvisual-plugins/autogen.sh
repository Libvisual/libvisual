#! /usr/bin/env bash
# Run this to generate all the initial makefiles, etc.
# Mimics gnome-autogen.sh of gnome-common to some extent.

set -e -u

# For alsa.m4 (with "L" in ALSA meaning "Linux") we want three things:
# - That a build on macOS is fully okay without it.
# - That "make dist" results from Linux contain a healthy m4/alsa.m4 file.
# - That running ./autogen.sh (or autoreconf) on a host
#   without ALSA development files installed still succeeds.
alsa_m4="$(aclocal --print-ac-dir)"/alsa.m4
if [[ -f "${alsa_m4}" ]]; then
    mkdir -p m4/
    cp -v "${alsa_m4}" m4/
fi
if [[ "$(uname -s)" == Darwin ]]; then
    # This provides a dummy substitute to alsa.m4 on macOS
    # and that is a reason why "make dist" should be run on Linux, not macOS.
    echo 'AC_DEFUN([AM_PATH_ALSA], [HAVE_ALSA=no])' > acinclude.m4
elif [[ ! -f m4/alsa.m4 ]]; then
    echo "ERROR: Please install ALSA development files before running" >&2
    echo "       '$0' on Linux." >&2
    exit 1
fi

# NOTE: We're running things twice to workaround bug "too many loops" in aclocal
autoreconf --install --verbose --force || \
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
