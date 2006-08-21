CC = gcc
CFLAGS = -ggdb3 `pkg-config --cflags libvisual-0.4`
LIBS = `pkg-config --libs libvisual-0.4`

TESTS = $(basename $(wildcard unit_test_*.c))

all: $(TESTS) test

$(TESTS): $(addsuffix .o, $(TESTS)) 
	$(CC) $(CFLAGS) -o $@ $(addsuffix .o, $@) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $< 

test: $(addsuffix .test, $(TESTS))
	@echo -ne "\n@@@ All units tests were completed successfully! @@@\n\n"

$(addsuffix .test, $(TESTS)): $(TESTS)
	./$(basename $@)
	@echo -ne "\n@@@ Test: $(basename $@) done @@@\n\n"
	
clean::
	rm -f *.o $(TESTS) core.* core

