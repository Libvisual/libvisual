## Process this file with automake to generate a Makefile.in

SUBDIRS = @BUILD_SUBDIRS@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libvisual.pc

install-exec-local: lvconfig.h 
	        $(mkinstalldirs) $(prefix)/include/libvisual
	        file=$(prefix)/include/libvisual/lvconfig.h; \
	        if test -r $$file && cmp -s lvconfig.h $$file; then :; \
	        else $(INSTALL_DATA) lvconfig.h $$file; fi

lvconfig.h: stamp-gc-h
	@if test -f lvconfig.h; then :; \
	else rm -f stamp-gc-h; $(MAKE) stamp-gc-h; fi
stamp-gc-h: config.status
	cd $(top_builddir) && $(SHELL) ./config.status lvconfig.h
	echo timestamp > stamp-gc-h

uninstall-local:
	rm -f $(prefix)/include/libvisual/lvconfig.h

EXTRA_DIST = Doxyfile

DISTCLEANFILES = lvconfig.h

