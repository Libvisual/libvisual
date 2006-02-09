#include <stdio.h>

#include "avs_lexer.h"

int main(int argc, char **argv)
{
	AvsLexerContext ctx;
	AvsLexerTokenValue v;
	AvsLexerTokenType t;
	char *type[] = {
		"AvsLexerTokenEOF",
		"AvsLexerTokenInvalid",
		"AvsLexerTokenIdentifier",
		"AvsLexerTokenConstant",
		"AvsLexerTokenPunctuator",
	};
	
	avs_lexer_init(&ctx, argv[1], strlen(argv[1]));

	while ((t=avs_lexer_token(&ctx, &v))) {
		fprintf(stderr, "Token type: %d %s\n", v.type, type[v.type]);
		switch (v.type) {
			case AvsLexerTokenEOF:
			case AvsLexerTokenInvalid:
				fprintf(stderr, "\tError: %s\n", avs_lexer_error(&ctx));
				break;

			case AvsLexerTokenIdentifier:
				fprintf(stderr, "\tIdentifier: %s\n", v.v.identifier);
				break;
				
			case AvsLexerTokenConstant:
				fprintf(stderr, "\tConstant: %f\n", v.v.constant);
				break;

			case AvsLexerTokenPunctuator:
				fprintf(stderr ,"\tPunctuator: %c\n", v.v.punctuator);
				break;
		}
	}
	return 0;
}
