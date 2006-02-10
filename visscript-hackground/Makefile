CC = gcc
CFLAGS = -O6 -ggdb3 -Wall
LIBS = `pkg-config --cflags --libs libvisual`

OBJS = avs_lexer.o avs_parser.o avs_test.o avs_compiler.o avs_il_tree.o avs_il_assembler.o avs_runnable.o avs_functions.o avs_stack.o avs_blob.o avs_blob_pool.o avs_x86_opcode.o avs_il_register.o avs_il_instruction.o avs_ix_compiler.o avs_ix_machine.o avs_il_core.o avs_x86_compiler.o

#avs_x86_opcode.o avs_x86_assembler.o

avs: $(OBJS) avs_functions.perf.h
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

avs_functions.perf.h: avs_functions.perf
	gperf avs_functions.perf > avs_functions.perf.h

clean:: 
	rm -f *.o core avs

