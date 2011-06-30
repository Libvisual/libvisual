#ifndef _AVS_LEXER_H
#define _AVS_LEXER_H 1

#define AVS_LEXER_MAXID 32 

struct _AvsLexerContext;
typedef struct _AvsLexerContext AvsLexerContext;

enum _AvsLexerState;
typedef enum _AvsLexerState AvsLexerState;

enum _AvsLexerTokenType;
typedef enum _AvsLexerTokenType AvsLexerTokenType;

struct _AvsLexerTokenValue;
typedef struct _AvsLexerTokenValue AvsLexerTokenValue;

enum _AvsLexerError;
typedef enum _AvsLexerError AvsLexerError;

enum _AvsLexerError {
	AvsLexerErrorSuccess,
	AvsLexerErrorInvalid,
	AvsLexerErrorConstantOverflow,
	AvsLexerErrorConstantInvalid,
};

struct _AvsLexerContext {
	unsigned char *base, *pos, *end;
	unsigned int  length;
	unsigned int  line, column;
	AvsLexerError error;
};
enum _AvsLexerState {
	AvsLexerStateInvalid,
	AvsLexerStateComment,
	AvsLexerStateIdentifier,
	AvsLexerStateConstant,
	AvsLexerStateFraction,
	AvsLexerStatePunctuator,
	AvsLexerStateNewline,
	AvsLexerStateTab,
	AvsLexerStateWhitespace,
	AvsLexerStateLast,
};

enum _AvsLexerTokenType {
	AvsLexerTokenEOF,
	AvsLexerTokenInvalid,
	AvsLexerTokenIdentifier,
	AvsLexerTokenConstant,
	AvsLexerTokenPunctuator,
};

struct _AvsLexerTokenValue {
	AvsLexerTokenType	type;
	union {
		char		identifier[AVS_LEXER_MAXID];
		AvsNumber	constant;
		char		punctuator;
	} v;
};


	
/* prototypes */
AvsLexerTokenType avs_lexer_token(AvsLexerContext *ctx, AvsLexerTokenValue *value);
const char *avs_lexer_error(AvsLexerContext *ctx);
int avs_lexer_reset(AvsLexerContext *ctx, unsigned char *data, unsigned int length);
int avs_lexer_init(AvsLexerContext *ctx);

#endif /* !_AVS_LEXER_H */
