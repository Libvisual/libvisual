## Process this file with automake to generate a Makefile.in

SUBDIRS = libvisual examples

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libvisual.pc

install-exec-local: libvisual/lvconfig.h 
	        $(mkinstalldirs) $(DESTDIR)$(includedir)/libvisual
	        file=$(DESTDIR)$(includedir)/libvisual/lvconfig.h; \
	        if test -r $$file && cmp -s libvisual/lvconfig.h $$file; then :; \
	        else $(INSTALL_DATA) libvisual/lvconfig.h $$file; fi

lvconfig.h: stamp-gc-h
	@if test -f libvisual/lvconfig.h; then :; \
	else rm -f stamp-gc-h; $(MAKE) stamp-gc-h; fi
stamp-gc-h: config.status
	cd $(top_builddir) && $(SHELL) ./config.status libvisual/lvconfig.h
	echo timestamp > stamp-gc-h

uninstall-local:
		rmdir $(includedir)/libvisual

EXTRA_DIST = Doxyfile

DISTCLEANFILES = libvisual/lvconfig.h

