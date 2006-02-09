#include <stdio.h>

#include "avs_lexer.h"

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

typedef struct _AvsParserState AvsParserState;

#define AvsParserDefault 0xff

/* Command layout: |15 14 13 12 11 10 09 08|07 06 05 04 03 02|01 00|
 * 		   |   Command argument    |  Command Index  | CMD |
 */

//#define CREATECMD(arg,index,code) \
//		((code) | ((index) << 2) | ((arg) << 8))

#define CREATECMD(cmd, index, arg, length) { (cmd), (index), (arg), (length) }

#define E(index,arg,length) CREATECMD(AvsParserError,index,arg,length)
#define R(index,arg,length) CREATECMD(AvsParserReduce,index,arg,length)
#define S(index,arg,length) CREATECMD(AvsParserShift,index,arg,length)
#define A(index,arg,length) CREATECMD(AvsParserAccept,index,arg,length)
#define D(index,arg,length) CREATECMD(AvsParserDefault,index,arg,length)

#include "table.h"

#define AVS_PARSER_STATE(ptr) (((ptr) - (parserstate[0])) / AVS_PARSER_TOKENS)
#define AVS_PARSER_TOKEN(ptr) (((ptr) - (parserstate[0])) % AVS_PARSER_TOKENS)

#define TOKEN_EMPTY (-2)
#define TOKEN_EOF (0)

int main(int argc, char **argv)
{
	int i;
	AvsLexerContext ctx;
	AvsLexerTokenValue v;
	AvsLexerTokenType t;
	AvsParserState *state;
	AvsParserState *sstack[1024], **sp = sstack, **psp;
	AvsLexerTokenValue vstack[1024], *vp = vstack;
	char *type[] = {
		"AvsLexerTokenEOF",
		"AvsLexerTokenInvalid",
		"AvsLexerTokenIdentifier",
		"AvsLexerTokenConstant",
		"AvsLExerTokenPunctuator",
	};
	AvsLexerTokenValue value;
	int lextoken, token;
	int command;
	int length;
	int lookup;
	char buf[32768];
	enum AvsParserCommand acmd;
	int		 aidx, aarg;
	
	length = read(0, buf, sizeof(buf));
	buf[length] = 0;
	avs_lexer_init(&ctx, buf, length);

	state = parserstate[0];
	lextoken = TOKEN_EMPTY;
	sp--;
	for (;;) {
		*++sp = state;
		fprintf(stderr, "Entering state: %d\n", AVS_PARSER_STATE(*sp));

		if (state->command != AvsParserDefault) {
			if (lextoken == TOKEN_EMPTY) {
				fprintf(stderr, "Reading a token: ");
				lextoken = avs_lexer_token(&ctx, &v);
				fprintf(stderr, "Line: %d Column: %d\n", ctx.line, ctx.column);
				if (v.type == AvsLexerTokenConstant) {
					fprintf(stderr, "AVS: Constant: %f\n", v.v.constant);
				}
				if (v.type == AvsLexerTokenIdentifier) {
					fprintf(stderr, "AVS: Identifier: %s\n", v.v.identifier);
				}
				if (v.type == AvsLexerTokenPunctuator) {
					fprintf(stderr, "AVS: Punctuator: '%c'\n", v.v.punctuator);
				}
				fprintf(stderr, "Token type: %d %s\n", v.type, type[v.type]);
			}
			
			if (lextoken <= TOKEN_EOF) {
				lextoken = token = TOKEN_EOF;
				fprintf(stderr, "Now at end of input.\n");
			} else {
				token = parsetranslate[lextoken];
				fprintf(stderr, "Next token is: %d\n", token);
			}

			state = &state[token];
		}

		switch (state->command) {
			case AvsParserError:
				fprintf(stderr, "ERRORNOUS STATE!\n");
				exit(1);
				break;

			case AvsParserDefault: /* Default reduce rule */
			case AvsParserReduce:
//				length = yyr2[aidx];
//				v = vp[1 - length];
				v = vp[1 - state->length];
				
				fprintf(stderr, "Reducing stack by rule %d (argument length: %d)\n", state->index - 1, state->length);
				switch (state->index)
				{
					case 2:
						{ fprintf(stderr, "STATEMENT: %p\n", v); ;}
						break;

					case 3:
						{ fprintf(stderr, "EMPTY!\n"); ;}
						break;

					case 6:
						{ fprintf(stderr, "PP: ;\n"); ;}
						break;

					case 7:
						{ fprintf(stderr, "PP: expr ;\n"); ;}
						break;

					case 9:
						{ fprintf(stderr, "PP: uminus\n");	;}
						break;

					case 10:
						{ fprintf(stderr, "PP: uplus\n");    	;}
						break;

					case 11:
					case 12:
					case 13:
					case 14:
					case 15:
					case 16:
					case 17:
					case 18:
						fprintf(stderr, "PP: '%c'\n", vp[-1].v.punctuator); 
						break;
					case 19:
						{ fprintf(stderr, "PP: (expr)\n");    ;}
						break;

					case 23:
						{ fprintf(stderr, "PP: constant\n"); ;}
						break;

					case 24:
						{ fprintf(stderr, "PP: identifier\n"); ;}
						break;

					case 25:
						{ fprintf(stderr, "PP: function '(' ')'\n"); ;}
						break;


				}


				vp -= state->length;
				sp -= state->length;

				fprintf(stderr, "Stack now");
				for (psp = sstack; psp <= sp; psp++) {
					fprintf(stderr, " %d", AVS_PARSER_STATE(*psp));
				} 
				fprintf(stderr, "\n");

				*++vp = v;

				fprintf(stderr, "Reduced non-terminal symbol: %d\n", state->token);
				state = parserstate[parsergotostate[AVS_PARSER_STATE(*sp)][state->token]];
				fprintf(stderr, "Reduced original state: %d\n", AVS_PARSER_STATE(*sp));
				fprintf(stderr, "Reduced new state: %d\n", AVS_PARSER_STATE(state));
				break;

			case AvsParserShift:
				/* Shift */
//				fprintf(stderr, "Shifting token %s, ", yytname[token]);
				fprintf(stderr, "Shifting token %d\n", token);

				if (lextoken != TOKEN_EOF)
					lextoken = TOKEN_EMPTY;

				*++vp = v;
				fprintf(stderr, "To state: %d\n", state->index);
				state = parserstate[state->index];
				fprintf(stderr, "Current state: %d (len: %d sizeof: %d)\n", AVS_PARSER_STATE(state),
						parserstate[1] - parserstate[0],
						sizeof(parserstate[0]));
				break;

			case AvsParserAccept:
				fprintf(stderr, "Accept rule!\n");
				/* return */
				exit(0);
				break;
		}
	}

	return 0;
}
