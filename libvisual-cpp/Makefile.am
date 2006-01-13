SUBDIRS = libvisual-cpp

# HTML documentation target

.PHONY: html-docs

if WITH_HTML_DOCS
html-docs: Doxyfile
	$(DOXYGEN)
else
html-docs: Doxyfile
	echo HTML documentation generation has been disabled.
endif
