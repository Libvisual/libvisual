#ifndef _AVS_PARSER_H
#define _AVS_PARSER_H 1

#define AVS_PARSER_INITIAL_STACKSIZE	512
#define AVS_PARSER_MAXIMUM_STACKSIZE	1024*1024
#include "avs_lexer.h"
#include "avs_compiler.h"

enum AvsParserCommand {
	AvsParserError,
	AvsParserReduce,
	AvsParserShift,
	AvsParserAccept,
	AvsParserDefault,
};

struct _AvsParserState {
	unsigned char	command;
	unsigned char	index;
	unsigned char	token;
	unsigned char	length;
};

struct _AvsParserErrorState {
	unsigned char	base;
	unsigned char	end;
	unsigned int	mask;
};

typedef struct _AvsParserState AvsParserState;
typedef struct _AvsParserErrorState AvsParserErrorState;

union _AvsParserValue {
	AvsLexerTokenValue token;
};

typedef union _AvsParserValue AvsParserValue;

struct _AvsParserContext {
	AvsLexerContext		*lexer;			/*<< Lexer context associated with parser */
	AvsCompilerContext	*compiler;		/*<< Compiler context associated with parser */
	
	/* State and value stack data */
	AvsStack		*sstack;
	AvsStack		*vstack;
};

typedef struct _AvsParserContext AvsParserContext;

/* prototypes */
int avs_parser_run(AvsParserContext *px, AvsRunnable *obj);
void avs_parser_cleanup(AvsParserContext *px);
int avs_parser_init(AvsParserContext *px, AvsLexerContext *lx, AvsCompilerContext *cx);

#endif /* !_AVS_PARSER_H */
