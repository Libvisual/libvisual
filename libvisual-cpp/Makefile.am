SUBDIRS = libvisual-cpp tests

libvisual_cpp_includedir = $(includedir)/libvisual-cpp
libvisual_cpp_include_HEADERS = lv_build_config.hpp

# HTML documentation target

.PHONY: html-docs

if WITH_HTML_DOCS
html-docs: Doxyfile
	$(DOXYGEN)
else
html-docs: Doxyfile
	echo HTML documentation generation has been disabled.
endif
