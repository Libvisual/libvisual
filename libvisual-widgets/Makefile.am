## Process this file with automake to generate a Makefile.in

SUBDIRS = gtk2

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libvisual-widgets-gtk1.pc  libvisual-widgets-gtk2.pc  libvisual-widgets-qt.pc

install-exec-local: lvw/lvwconfig.h 
	        $(mkinstalldirs) $(DESTDIR)$(includedir)/lvw
	        file=$(DESTDIR)$(includedir)/lvw/lvwconfig.h; \
	        if test -r $$file && cmp -s lvw/lvwconfig.h $$file; then :; \
	        else $(INSTALL_DATA) lvw/lvwconfig.h $$file; fi

lvwconfig.h: stamp-gc-h
	@if test -f lvw/lvwconfig.h; then :; \
	else rm -f stamp-gc-h; $(MAKE) stamp-gc-h; fi
stamp-gc-h: config.status
	cd $(top_builddir) && $(SHELL) ./config.status lvw/lvwconfig.h
	echo timestamp > stamp-gc-h

uninstall-local:
		rmdir $(includedir)/libvisual


DISTCLEANFILES = lvw/lvwconfig.h

