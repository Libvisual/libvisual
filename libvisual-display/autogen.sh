#!/bin/sh
# Run this to generate all the initial makefiles, etc.

DIE=0

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`autoconf' installed to."
  echo "Download the appropriate package for your distribution,"
  echo "or get the source tarball at ftp://ftp.gnu.org/gnu/autoconf/"
  DIE=1
}

(automake --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`automake' installed."
  echo "Get ftp://ftp.gnu.org/gnu/automake/automake-1.7.0.tar.gz"
  echo "(or a newer version if it is available)"
  DIE=1
  NO_AUTOMAKE=yes
}

(libtool --version) < /dev/null > /dev/null 2>&1 || {
  echo
  echo "**Error**: You must have \`libtool' installed."
  echo "Get ftp://ftp.gnu.org/gnu/libtool/libtool-1.5.6.tar.gz"
  echo "(or a newer version if it is available)"
  DIE=1
  NO_AUTOMAKE=yes
}

if test "$DIE" -eq 1; then
  exit 1
fi

libtoolize --force
autoreconf --verbose --make --install --symlink --force
