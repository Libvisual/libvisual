OCAMLFIND = ocamlfind
OCAMLFIND_FLAGS = 

OUTPUT = demo
ML_SOURCES = point.ml line.ml triangle.ml main.ml
PACKAGES = -package sdl -package sdl.sdlimage

CMI_FILES = $(ML_SOURCES:.ml=.cmi)
CMX_FILES = $(ML_SOURCES:.ml=.cmx)
CMO_FILES = $(ML_SOURCES:.ml=.cmo)
OBJ_FILES = $(ML_SOURCES:.ml=.o)

$(OUTPUT): $(CMX_FILES)
	$(OCAMLFIND) ocamlopt $(OCAMLFIND_FLAGS) -linkpkg $(PACKAGES) -o $(OUTPUT) $(CMX_FILES)

%.cmi: %.mli
	$(OCAMLFIND) ocamlc $(OCAMLFIND_FLAGS) -o $@ $<

%.cmo: %.ml
	$(OCAMLFIND) ocamlc -c $(OCAMLFIND_FLAGS) $(PACKAGES) -o $@ $<

%.cmx: %.ml
	$(OCAMLFIND) ocamlopt -c $(OCAMLFIND_FLAGS) $(PACKAGES) -o $@ $<

.PHONY: depend clean

clean:
	rm -f $(OUTPUT) $(CMX_FILES) $(CMI_FILES) $(CMO_FILES) $(OBJ_FILES) *~

depend:
	$(OCAMLFIND) ocamldep -native $(PACKAGES) $(ML_SOURCES) > dependencies

-include dependencies
