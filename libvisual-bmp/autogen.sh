#!/bin/sh
# Run this to generate all the initial makefiles, etc.

# Mostly stolen from jamesh's gnome-autogen.sh script
# within gnome-common package.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

PKG_NAME="Libvisual"

if [ ! -f $srcdir/configure.ac ]; then
    #echo -n "**Error**: Directory "\`$srcdir\'" does "
    #echo "not look like the top-level $PKG_NAME directory"
    echo "**Error**: Directory <$srcdir> does not look"
    echo "like the top-level $PKG_NAME directory"
    exit 1
fi

REQUIRED_AUTOCONF_VERSION=2.57
REQUIRED_AUTOMAKE_VERSION=1.7
# I'm not shure if 1.4.3 works, I have only tested with libtool 1.5.6
REQUIRED_LIBTOOL_VERSION=1.4.3
REQUIRED_PKG_CONFIG_VERSION=0.14.0
# Probably it works with older version of glib-gettextize
REQUIRED_GLIB_GETTEXT_VERSION=2.4.0

# A list of required m4 macros. Add your own here.
REQUIRED_M4MACROS="sdl.m4"

printerr() {
    echo "$@" >&2
}

# Usage:
#     compare_versions MIN_VERSION ACTUAL_VERSION
# returns true if ACTUAL_VERSION >= MIN_VERSION
compare_versions() {
    ch_min_version=$1
    ch_actual_version=$2
    ch_status=0
    IFS="${IFS=         }"; ch_save_IFS="$IFS"; IFS="."
    set $ch_actual_version
    for ch_min in $ch_min_version; do
        ch_cur=`echo $1 | sed 's/[^0-9].*$//'`; shift # remove letter suffixes
        if [ -z "$ch_min" ]; then break; fi
        if [ -z "$ch_cur" ]; then ch_status=1; break; fi
        if [ $ch_cur -gt $ch_min ]; then break; fi
        if [ $ch_cur -lt $ch_min ]; then ch_status=1; break; fi
    done
    IFS="$ch_save_IFS"
    return $ch_status
}

# Usage:
#     version_check PACKAGE VARIABLE CHECKPROGS MIN_VERSION SOURCE
# checks to see if the package is available
version_check() {
    vc_package=$1
    vc_variable=$2
    vc_checkprogs=$3
    vc_min_version=$4
    vc_source=$5
    vc_status=1

    vc_checkprog=`eval echo "\\$$vc_variable"`
    if [ -n "$vc_checkprog" ]; then
	echo "using $vc_checkprog for $vc_package"
	return 0
    fi

    if test "x$vc_package" = "xautomake" -a "x$vc_min_version" = "x1.4"; then
	vc_comparator="="
    else
	vc_comparator=">="
    fi
    echo "checking for $vc_package $vc_comparator $vc_min_version..."
    for vc_checkprog in $vc_checkprogs; do
	echo $ECHO_N "  testing $vc_checkprog... "
	if $vc_checkprog --version < /dev/null > /dev/null 2>&1; then
	    vc_actual_version=`$vc_checkprog --version | head -n 1 | \
                               sed 's/^.*[ 	]\([0-9.]*[a-z]*\).*$/\1/'`
	    if compare_versions $vc_min_version $vc_actual_version; then
		echo "found $vc_actual_version"
		# set variable
		eval "$vc_variable=$vc_checkprog"
		vc_status=0
		break
	    else
		echo "too old (found version $vc_actual_version)"
	    fi
	else
	    echo "not found."
	fi
    done
    if [ "$vc_status" != 0 ]; then
	printerr "***Error***: You must have $vc_package $vc_comparator $vc_min_version installed"
	printerr "  to build $PKG_NAME.  Download the appropriate package for"
	printerr "  from your distribution or get the source tarball at"
        printerr "    $vc_source"
	printerr
    fi
    return $vc_status
}

# Usage:
#     require_m4macro filename.m4
# adds filename.m4 to the list of required macros
require_m4macro() {
    case "$REQUIRED_M4MACROS" in
	$1\ * | *\ $1\ * | *\ $1) ;;
	*) REQUIRED_M4MACROS="$REQUIRED_M4MACROS $1" ;;
    esac
}

# Usage:
#     check_m4macros
# Checks that all the requested macro files are in the aclocal macro path
# Uses REQUIRED_M4MACROS and ACLOCAL variables.
check_m4macros() {
    # construct list of macro directories
    cm_macrodirs="`$ACLOCAL --print-ac-dir`"
    set - $ACLOCAL_FLAGS
    while [ $# -gt 0 ]; do
	if [ "$1" = "-I" ]; then
	    cm_macrodirs="$cm_macrodirs $2"
	    shift
	fi
	shift
    done

    cm_status=0
    if [ -n "$REQUIRED_M4MACROS" ]; then
	echo "Checking for required M4 macros..."
	# check that each macro file is in one of the macro dirs
	for cm_macro in $REQUIRED_M4MACROS; do
	    cm_macrofound=false
	    for cm_dir in $cm_macrodirs; do
		if [ -f "$cm_dir/$cm_macro" ]; then
		    cm_macrofound=true
		    break
		fi
		# The macro dir in Cygwin environments may contain a file
		# called dirlist containing other directories to look in.
		if [ -f "$cm_dir/dirlist" ]; then
		    for cm_otherdir in `cat $cm_dir/dirlist`; do
			if [ -f "$cm_otherdir/$cm_macro" ]; then
			    cm_macrofound=true
		            break
			fi
		    done
		fi
	    done
	    if $cm_macrofound; then
		:
	    else
		printerr "  $cm_macro not found"
		cm_status=1
	    fi
	done
    fi
    if [ "$cm_status" != 0 ]; then
	printerr "***Error***: some autoconf macros required to build $PKG_NAME"
	printerr "  were not found in your aclocal path. Perhaps you need to"
	printerr "  adjust your ACLOCAL_FLAGS?"
	printerr
    fi
    return $cm_status
}

want_libtool=false
want_gettext=false
want_glib_gettext=false
want_intltool=false
want_pkg_config=false

configure_files="`find $srcdir -name '{arch}' -prune -o -name configure.ac -print -o -name configure.in -print`"
for configure_ac in $configure_files; do
    if grep "^A[CM]_PROG_LIBTOOL" $configure_ac >/dev/null || \
       grep "^LT_INIT" $configure_ac >/dev/null; then
	want_libtool=true
    fi
    if grep "^AM_GNU_GETTEXT" $configure_ac >/dev/null; then
	want_gettext=true
    fi
    if grep "^AM_GLIB_GNU_GETTEXT" $configure_ac >/dev/null; then
	want_glib_gettext=true
    fi
    if grep "^\(AC\|IT\)_PROG_INTLTOOL" $configure_ac >/dev/null; then
	want_intltool=true
    fi
    if grep "^PKG_CHECK_MODULES" $configure_ac >/dev/null; then
	want_pkg_config=true
    fi
done

DIE=0

#tell Mandrake autoconf wrapper we want autoconf 2.5x, not 2.13
WANT_AUTOCONF_2_5=1
export WANT_AUTOCONF_2_5
version_check autoconf AUTOCONF 'autoconf2.50 autoconf autoconf-2.53' $REQUIRED_AUTOCONF_VERSION \
    "http://ftp.gnu.org/pub/gnu/autoconf/autoconf-$REQUIRED_AUTOCONF_VERSION.tar.gz" || DIE=1
AUTOHEADER=`echo $AUTOCONF | sed s/autoconf/autoheader/`

case $REQUIRED_AUTOMAKE_VERSION in
    1.4*) automake_progs="automake-1.4" ;;
    1.5*) automake_progs="automake-1.5 automake-1.6 automake-1.7 automake-1.8 automake-1.9" ;;
    1.6*) automake_progs="automake-1.6 automake-1.7 automake-1.8 automake-1.9" ;;
    1.7*) automake_progs="automake-1.7 automake-1.8 automake-1.9" ;;
    1.8*) automake_progs="automake-1.8 automake-1.9" ;;
    1.9*) automake_progs="automake-1.9" ;;
esac
version_check automake AUTOMAKE "$automake_progs" $REQUIRED_AUTOMAKE_VERSION \
    "http://ftp.gnu.org/pub/gnu/automake/automake-$REQUIRED_AUTOMAKE_VERSION.tar.gz" || DIE=1
ACLOCAL=`echo $AUTOMAKE | sed s/automake/aclocal/`

if $want_libtool; then
    version_check libtool LIBTOOLIZE libtoolize $REQUIRED_LIBTOOL_VERSION \
        "http://ftp.gnu.org/pub/gnu/libtool/libtool-$REQUIRED_LIBTOOL_VERSION.tar.gz" || DIE=1
    require_m4macro libtool.m4
fi

if $want_gettext; then
    version_check gettext GETTEXTIZE gettextize $REQUIRED_GETTEXT_VERSION \
        "http://ftp.gnu.org/pub/gnu/gettext/gettext-$REQUIRED_GETTEXT_VERSION.tar.gz" || DIE=1
    require_m4macro gettext.m4
fi

if $want_glib_gettext; then
    version_check glib-gettext GLIB_GETTEXTIZE glib-gettextize $REQUIRED_GLIB_GETTEXT_VERSION \
        "ftp://ftp.gtk.org/pub/gtk/v2.2/glib-$REQUIRED_GLIB_GETTEXT_VERSION.tar.gz" || DIE=1
    require_m4macro glib-gettext.m4
fi

if $want_intltool; then
    version_check intltool INTLTOOLIZE intltoolize $REQUIRED_INTLTOOL_VERSION \
        "http://ftp.gnome.org/pub/GNOME/sources/intltool/" || DIE=1
    require_m4macro intltool.m4
fi

if $want_pkg_config; then
    version_check pkg-config PKG_CONFIG pkg-config $REQUIRED_PKG_CONFIG_VERSION \
        "'http://www.freedesktop.org/software/pkgconfig/releases/pkgconfig-$REQUIRED_PKG_CONFIG_VERSION.tar.gz" || DIE=1
    require_m4macro pkg.m4
fi

check_m4macros || DIE=1

if [ "$DIE" -eq 1 ]; then
  exit 1
fi

if [ "$#" = 0 ]; then
  printerr "**Warning**: I am going to run \`configure' with no arguments."
  printerr "If you wish to pass any to it, please specify them on the"
  printerr \`$0\'" command line."
  printerr
fi

topdir=`pwd`
for configure_ac in $configure_files; do 
    dirname=`dirname $configure_ac`
    basename=`basename $configure_ac`
    if [ -f $dirname/NO-AUTO-GEN ]; then
	echo skipping $dirname -- flagged as no auto-gen
    elif [ ! -w $dirname ]; then
        echo skipping $dirname -- directory is read only
    else
	echo "Processing $configure_ac"
	cd $dirname

        # Note that the order these tools are called should match what
        # autoconf's "autoupdate" package does.  See bug 138584 for
        # details.

        # programs that might install new macros get run before aclocal
	if grep "^A[CM]_PROG_LIBTOOL" $basename >/dev/null || \
	   grep "^LT_INIT" $basename >/dev/null; then
	    echo "Running $LIBTOOLIZE..."
	    $LIBTOOLIZE --force --copy || exit 1
	fi

	if grep "^AM_GLIB_GNU_GETTEXT" $basename >/dev/null; then
	    echo "Running $GLIB_GETTEXTIZE... Ignore non-fatal messages."
	    echo "no" | $GLIB_GETTEXTIZE --force --copy || exit 1
	elif grep "^AM_GNU_GETTEXT" $basename >/dev/null; then
	   if grep "^AM_GNU_GETTEXT_VERSION" $basename > /dev/null; then
	   	echo "Running autopoint..."
		autopoint --force || exit 1
	   else
	    	echo "Running $GETTEXTIZE... Ignore non-fatal messages."
	    	echo "no" | $GETTEXTIZE --force --copy || exit 1
	   fi
	fi

	if grep "^\(AC\|IT\)_PROG_INTLTOOL" $basename >/dev/null; then
	    echo "Running $INTLTOOLIZE..."
	    $INTLTOOLIZE --force --copy --automake || exit 1
	fi

        # Now run aclocal to pull in any additional macros needed

	# if the AC_CONFIG_MACRO_DIR() macro is used, pass that
	# directory to aclocal.
	m4dir=`cat "$basename" | grep '^AC_CONFIG_MACRO_DIR' | sed -n -e 's/AC_CONFIG_MACRO_DIR(\([^()]*\))/\1/p' | sed -e 's/^\[\(.*\)\]$/\1/' | sed -e 1q`
	if [ -n "$m4dir" ]; then
	    m4dir="-I $m4dir"
	fi
	echo "Running $ACLOCAL..."
	$ACLOCAL $m4dir $ACLOCAL_FLAGS || exit 1

	# Now that all the macros are sorted, run autoconf and autoheader ...
	echo "Running $AUTOCONF..."
	$AUTOCONF || exit 1
	if grep "^A[CM]_CONFIG_HEADER" $basename >/dev/null; then
	    echo "Running $AUTOHEADER..."
	    $AUTOHEADER || exit 1
	    # this prevents automake from thinking config.h.in is out of
	    # date, since autoheader doesn't touch the file if it doesn't
	    # change.
	    test -f config.h.in && touch config.h.in
	fi

	# Finally, run automake to create the makefiles ...
	echo "Running $AUTOMAKE..."
        cp -pf COPYING COPYING.autogen_bak
        cp -pf INSTALL INSTALL.autogen_bak
	if [ $REQUIRED_AUTOMAKE_VERSION != 1.4 ]; then
	    $AUTOMAKE --gnu --add-missing --force --copy || exit 1
	else
	    $AUTOMAKE --gnu --add-missing --copy || exit 1
	fi
        cmp COPYING COPYING.autogen_bak || cp -pf COPYING.autogen_bak COPYING
        cmp INSTALL INSTALL.autogen_bak || cp -pf INSTALL.autogen_bak INSTALL
        rm -f COPYING.autogen_bak INSTALL.autogen_bak

	cd "$topdir"
    fi
done

# Specific default flags for configure when used with autogen.sh
conf_flags=""

# NOCONFIGURE must be defined on the environment to avoid running configure
if test x$NOCONFIGURE = x; then
    echo Running $srcdir/configure $conf_flags "$@" ...
    $srcdir/configure $conf_flags "$@" \
	&& echo Now type \`make\' to compile $PKG_NAME || exit 1
else
    echo Skipping configure process.
fi
