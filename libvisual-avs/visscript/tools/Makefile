BISON = bison
CC = gcc
CFLAGS = -ggdb3

all: gentables

gentables: genparsertable genlextable.pl
	perl genlextable.pl > lextab
	./genparsertable > ../avs_parser_table.h 2>parser.log
	./genparsertable switchtable avs_grammar.y >switchtable.log
	
genparsertable: avs_grammar.tab.c
	$(CC) $(CFLAGS) -o $@ $<
	
avs_grammar.tab.c: avs_grammar.y
	$(BISON) -t -o avs_grammar.tab.c avs_grammar.y

clean::
	rm -f lextab *.o core genparsertable parser.log avs_grammar.tab.c switchtable.log
