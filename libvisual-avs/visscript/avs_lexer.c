#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"

#define AvsLexerReturn(x) \
	return (value->type = (x))

#define AvsLexerReturnError(x) \
	do { \
		value->type = AvsLexerTokenInvalid; \
		ctx->error = (x); \
		return value->type; \
	} while (0)

#define AvsLexerReturnPunctuator(x) \
	value->type = AvsLexerTokenPunctuator; \
	value->v.punctuator = (x); \
	return (x);

#ifndef MIN
	#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

static unsigned char lexerstate[256] = {
	0,0,0,0,0,0,0,0,0,7,6,8,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	8,0,0,0,2,5,5,0,5,5,5,5,5,5,4,1,3,3,3,3,3,3,3,3,3,3,0,5,0,5,0,0,
	0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,0,0,0,2,
	0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,0,5,
};

static unsigned char idtable[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
};

static unsigned char constable[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,1,1,1,1,1,1,1,1,1,
};

static inline void decrease_gc(AvsLexerContext *ctx)
{
	ctx->pos--;
	ctx->column--;
}

static inline void advance_gc(AvsLexerContext *ctx)
{
	ctx->pos++;
	ctx->column++;
}

static inline unsigned char look_gc(AvsLexerContext *ctx)
{
	if (ctx->pos >= ctx->end)
		return 0;

	return *ctx->pos;
}

static inline unsigned char gc(AvsLexerContext *ctx)
{
	if (ctx->pos++ >= ctx->end)
		return 0;
	
	ctx->column++;
	return ctx->pos[-1];
}

static inline AvsLexerTokenType convert_number(AvsLexerContext *ctx,
					       AvsLexerTokenValue *value, 
					       unsigned char *buf, unsigned int length)
{
	unsigned char *endptr;
	
	value->v.constant = strtod((char *)buf, (char**)&endptr);

	if (errno == ERANGE)
		AvsLexerReturnError(AvsLexerErrorConstantOverflow);

	if ((endptr - buf) < length)
		AvsLexerReturnError(AvsLexerErrorConstantInvalid);

	AvsLexerReturn(AvsLexerTokenConstant);
}

AvsLexerTokenType avs_lexer_token(AvsLexerContext *ctx, AvsLexerTokenValue *value)
{
	AvsLexerState state;
	unsigned char *pos;
	int c;

	ctx->error = AvsLexerErrorSuccess;
	
next:
	state = lexerstate[gc(ctx)];
	switch (state) {
		case AvsLexerStateInvalid:
			if (ctx->pos >= ctx->end)
				AvsLexerReturn(AvsLexerTokenEOF);
			else
				AvsLexerReturnError(AvsLexerErrorInvalid);

		case AvsLexerStateComment:
			/*
			 * Ambiguous state, can contain one of the following 
			 * tokens: comment, multi line comment, division punctuator 
			 */
			switch (gc(ctx)) {
				case '/': /* Single line comment */
					for (;;) {
						if ((c = gc(ctx)) == 0)
							AvsLexerReturn(AvsLexerTokenEOF);

						if (c == '\n')
							goto next;
					}
					break;
					
				case '*': /* Multi line comment */
					for (;;) {
						if ((c = gc(ctx)) == 0)
							AvsLexerReturn(AvsLexerTokenEOF);
					
						if (c != '*')
							continue;
						
						if (look_gc(ctx) == '/') {
							advance_gc(ctx);
							goto next;
						}
					}
					break;
					
				default: /* Division punctuator */
					decrease_gc(ctx);
					AvsLexerReturnPunctuator('/');
			}
			break;

		case AvsLexerStateIdentifier:
			pos = ctx->pos - 1;
			for (;;) {
				if (idtable[gc(ctx)])
					continue;
			
				decrease_gc(ctx);
				c = MIN(ctx->pos - pos, AVS_LEXER_MAXID); 
				memcpy(value->v.identifier, pos, c);
				value->v.identifier[c] = 0;
				AvsLexerReturn(AvsLexerTokenIdentifier);
			}

		case AvsLexerStateConstant:
		case AvsLexerStateFraction:
			pos = ctx->pos - 1;
			for (;;) {
				if (constable[gc(ctx)])
					continue;

				decrease_gc(ctx);
				return convert_number(ctx, value, pos, ctx->pos - pos);
			}

		case AvsLexerStatePunctuator:
			AvsLexerReturnPunctuator(ctx->pos[-1]);

		case AvsLexerStateNewline:
			ctx->line++;
			ctx->column = 0;
			goto next;

		case AvsLexerStateTab:
			ctx->column += 7 - (ctx->column % 8);
			goto next;
			
		case AvsLexerStateWhitespace:
			goto next;

		case AvsLexerStateLast:
			AvsLexerReturnError(AvsLexerErrorInvalid);
	}

	return -1;
}

const char *avs_lexer_error(AvsLexerContext *ctx)
{
	static const char *errmsg[] = {
		"Success",
		"Reached an invalid state: lexer error",
		"Floating point constant is too big",
		"Constant is invalid or contains invalid characters",
	};
	
	return errmsg[ctx->error % 4];
}

int avs_lexer_reset(AvsLexerContext *ctx, unsigned char *data, unsigned int length)
{
	memset(ctx, 0, sizeof(AvsLexerContext));
	ctx->base = data;
	ctx->pos = data;
	ctx->end = data + length;
	ctx->length = length;
	return 0;
}

int avs_lexer_init(AvsLexerContext *ctx)
{	
	memset(ctx, 0, sizeof(AvsLexerContext));
	return 0;
}
