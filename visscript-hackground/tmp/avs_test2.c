#include <stdio.h>

#include "avs_lexer.h"

#include "tabs.h"

int main(int argc, char **argv)
{
	int i;
	AvsLexerContext ctx;
	AvsLexerTokenValue v;
	AvsLexerTokenType t;
	int sstack[1024], *sp = sstack, *psp;
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
	int state;
	int length;
	int lookup;
	char buf[32768];

	length = read(0, buf, sizeof(buf));
	buf[length] = 0;
	avs_lexer_init(&ctx, buf, length);

	state = 0;
	lextoken = YYEMPTY;
	sp--;
	for (;;) {
		*++sp = state;
		fprintf(stderr, "Entering state: %d\n", *sp);

		lookup = yypact[*sp];
		if (lookup == YYPACT_NINF)
			goto default_action;

		if (lextoken == YYEMPTY) {
			fprintf(stderr, "Reading a token: ");
			lextoken = avs_lexer_token(&ctx, &v);
			if (v.type == AvsLexerTokenConstant) {
				fprintf(stderr, "AVS: Constant: %f\n", v.v.constant);
				lextoken = CONSTANT;
			}
			if (v.type == AvsLexerTokenIdentifier) {
				fprintf(stderr, "AVS: Identifier: %s\n", v.v.identifier);
				lextoken = IDENTIFIER;
			}
			if (v.type == AvsLexerTokenPunctuator) {
				fprintf(stderr, "AVS: Punctuator: '%c'\n", v.v.punctuator);
				lextoken = v.v.punctuator;
			}
			fprintf(stderr, "Token type: %d %s\n", v.type, type[v.type]);
		}

		if (lextoken <= YYEOF) {
			lextoken = token = YYEOF;
			fprintf(stderr, "Now at end of input.\n");
		} else {
			token = yytranslate[lextoken]; 
			fprintf(stderr, "Next token is: %d\n", token);
		}

		lookup += token;
		if (lookup < 0 || lookup > YYLAST || yycheck[lookup] != token)
			goto default_action;

		lookup = yytable[lookup];
		if (lookup <= 0) {
			/* Reduce */
			if (lookup == 0 || lookup == YYTABLE_NINF) {
				fprintf(stderr, "ERRORNOUS STATE!\n");
				exit(1);
			}
			lookup = -lookup;
			goto reduce;
		}

		if (lookup == YYFINAL) {
			fprintf(stderr, "Accept rule!\n");
			/* return */
			exit(0);
		}

		/* Shift */
		fprintf(stderr, "Shifting token %s, ", yytname[token]);

		if (lextoken != YYEOF)
			lextoken = YYEMPTY;

		*++vp = v;
		state = lookup;
		continue;

default_action:
		lookup = yydefact[state];
		if (lookup == 0) {
			fprintf(stderr, "Error in default state!\n");
			exit(1);
		}
		goto reduce;


reduce:
		length = yyr2[lookup];
		v = vp[1 - length];

		fprintf(stderr, "Reducing stack by rule %d (length: %d)\n", lookup - 1, length);
		switch (lookup)
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
				{ fprintf(stderr, "PP: +\n");    ;}
				break;

			case 12:
				{ fprintf(stderr, "PP: -\n");    ;}
				break;

			case 13:
				{ fprintf(stderr, "PP: *\n");    ;}
				break;

			case 14:
				{ fprintf(stderr, "PP: /\n");    ;}
				break;

			case 15:
				{ fprintf(stderr, "PP: %\n");    ;}
				break;

			case 16:
				{ fprintf(stderr, "PP: &\n");    ;}
				break;

			case 17:
				{ fprintf(stderr, "PP: |\n");    ;}
				break;

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


		vp -= length;
		sp -= length;

		fprintf(stderr, "Stack now");
		for (psp = sstack; psp <= sp; psp++) {
			fprintf(stderr, " %d", *psp);
		}
		fprintf(stderr, "\n");

		*++vp = v;

		lookup = yyr1[lookup];		/* Rule -> nonterminal symbol */
		state = *sp + yypgoto[lookup - YYNTOKENS]; /* newstaterafterreduce = lookup[previous_state][nt] */

		//	if (0 <= state && state <= YYLAST && yycheck[state] == *sp)
		fprintf(stderr, "Reduced non-terminal symbol: %d (name: %s)\n", lookup - YYNTOKENS, yytname[lookup]);
		fprintf(stderr, "Reduced original state: %d\n", *sp);
		if (state >= 0 && state <= YYLAST && yycheck[state] == *sp) {
			fprintf(stderr, "Reduced state offset: %d\n", state);
			state = yytable[state];
			fprintf(stderr, "Reduced state: %d\n", state);
		} else {
			state = yydefgoto[lookup - YYNTOKENS];
			fprintf(stderr, "Reduced default state: %d\n", state);
		}

	}

	return 0;
}
