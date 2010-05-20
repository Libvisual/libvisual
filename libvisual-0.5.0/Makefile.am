## Process this file with automake to generate a Makefile.in

SUBDIRS = m4 libvisual po

PKG_CONFIG_FILE = libvisual-$(LIBVISUAL_VERSION_SUFFIX).pc

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = $(PKG_CONFIG_FILE)

$(PKG_CONFIG_FILE): libvisual.pc
	cp -f $< $@

install-exec-local: libvisual/lvconfig.h 
	        file=$(DESTDIR)$(LIBVISUAL_INCLUDE_DIR)/libvisual/lvconfig.h; \
	        if test -r $$file && cmp -s libvisual/lvconfig.h $$file; then :; \
	        else $(INSTALL_DATA) libvisual/lvconfig.h $$file; fi

lvconfig.h: stamp-gc-h
	@if test -f libvisual/lvconfig.h; then :; \
	else rm -f stamp-gc-h; $(MAKE) stamp-gc-h; fi

stamp-gc-h: config.status
	cd $(top_builddir) && $(SHELL) ./config.status libvisual/lvconfig.h
	echo timestamp > stamp-gc-h

EXTRA_DIST = Doxyfile config.rpath libvisual.pc.in

DISTCLEANFILES = libvisual/lvconfig.h $(PKG_CONFIG_FILE)

ACLOCAL_AMFLAGS = -I m4
