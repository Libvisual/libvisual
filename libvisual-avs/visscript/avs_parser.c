#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "avs.h"
#include "avs_parser_private.h"
#include "avs_parser_table.h"

static inline void reduce_rule(AvsParserContext *px, AvsRunnable *obj, AvsParserState *cs, AvsParserValue *vp)
{
	#define PV(num)	\
		(&vp[-cs->length + num])

	switch (cs->index)
	{
		case  2: /* Rule: ': statement_list' */
		case  3: /* Rule: '| semicolon' */
		case  4: /* Rule: '| [[ empty ]]' */
		case  5: /* Rule: ': expr' */
		case  6: /* Rule: '| statement_list semicolon expr' */
			break;
			
		case  7: /* Rule: ': ';'' */
			avs_compile_marker(px->compiler, 
					obj,
					AvsCompilerMarkerSequencePoint,
					NULL);		
			avs_compile_reset_stack(px->compiler);
			break;
			
		case  8: /* Rule: ': primary' */
			break;
			
		case  9: /* Rule: '| '-' expr %prec UNARY' */
			avs_compile_arithop(px->compiler, obj, AvsCompilerInstructionNegate);
			break;
			
		case 10: /* Rule: '| '+' expr %prec UNARY' */
			break;
			
		case 11: /* Rule: '| expr '+' expr' */
		case 12: /* Rule: '| expr '-' expr' */
		case 13: /* Rule: '| expr '*' expr' */
		case 14: /* Rule: '| expr '/' expr' */
		case 15: /* Rule: '| expr '%' expr' */
		case 16: /* Rule: '| expr '&' expr' */
		case 17: /* Rule: '| expr '|' expr' */
		case 18: /* Rule: '| expr '=' expr' */ {
			static unsigned char rule2insn[] = {
				AvsCompilerInstructionAdd, AvsCompilerInstructionSub, AvsCompilerInstructionMul, AvsCompilerInstructionDiv,
				AvsCompilerInstructionMod, AvsCompilerInstructionAnd, AvsCompilerInstructionOr,  AvsCompilerInstructionAssign,
			};
			avs_compile_arithop(px->compiler, obj, rule2insn[cs->index-11]);
			break;
		}

		case 19: /* Rule: '| '(' expr ')'' */
		case 20: /* Rule: 'fn_expr' */
			break;
			
		case 21: /* Rule: '| {[[ argument marker ]]} expr' */
			avs_compile_marker(px->compiler, obj, AvsCompilerMarkerArgument, NULL);
			break;
			
		case 22: /* Rule: '| {[[ argument marker ]]} expr' */
			break;
			
		case 23: /* Rule: '| fn_expr ',' {[[ argument marker ]]} expr' */
			avs_compile_marker(px->compiler, obj, AvsCompilerMarkerArgument, NULL);
			break;
			
		case 24: /* Rule: '| fn_expr ',' {[[ argument marker ]]} expr' */
			break;
			
		case 25: /* Rule: ': CONSTANT' */
			avs_compile_push_constant(px->compiler, obj, PV(1)->token.v.constant);
			break;
			
		case 26: /* Rule: '| IDENTIFIER' */
			avs_compile_push_identifier(px->compiler, obj, PV(1)->token.v.identifier);
			break;
			
		case 27: /* Rule: '{[[ stack marker ]]}' */
			avs_compile_marker(px->compiler, 
					   obj, 
					   AvsCompilerMarkerFunction,
					   PV(-1)->token.v.identifier);
			break;
			
		case 28: /* Rule: '| IDENTIFIER '('' */		
			avs_compile_arithop(px->compiler, obj, AvsCompilerInstructionCall);
			break;
		
		case 29: /* Rule: ': expr' */
		case 30: /* Rule: '| [[ empty ]]' */
			break;
	}
}

static void report_error(AvsParserContext *px, int token, AvsParserErrorState *es)
{
	char *p;
	int xt;

	if (!es->end) {
		fprintf(stderr, "syntax error\n");
		return;
	}
	
	fprintf(stderr, "syntax error, unexpected %s, expecting", parsertokenname[token]);
	p = " ";
	for (xt=es->base; xt < es->end; xt++) {
		if (((1 << (xt - es->base)) & es->mask) == 0)
			continue;

		fprintf(stderr, "%s%s", p, parsertokenname[xt]);
		p = " or ";
	}
	fprintf(stderr, "\n");
}

/**
 * Run the parser, until EOF or an error is encountered.
 *
 * @param px Parser context, previously initialized by avs_parser_initialize()
 *
 * @see avs_parser_initialize
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */ 
int avs_parser_run(AvsParserContext *px, AvsRunnable *obj)
{
	AvsParserState *cs;
	AvsParserState **sp;
	AvsParserValue *vp;
	AvsParserValue vt, cv;
	int lt;

	/* Initialize parser states */
	cs = parserstate[0];
	lt = AVS_PARSER_EMPTY;

	sp = (AvsParserState **) px->sstack->base - 1;
	vp = (AvsParserValue *) px->vstack->base;
	
	for (;;) {
		/* Allocate new stack value */
		sp++;
		if ((unsigned char *)sp >= px->sstack->end) {
			/* Resize stack */
			px->sstack->pointer = (unsigned char *) sp;
			px->vstack->pointer = (unsigned char *) vp;
			
			if (avs_stack_resize(px->sstack) != VISUAL_OK || 
			    avs_stack_resize(px->vstack) != VISUAL_OK)
				/* Stack overflow */
				return VISUAL_ERROR_GENERAL;

			sp = (AvsParserState **) px->sstack->pointer;
			vp = (AvsParserValue *) px->vstack->pointer;
		}

		/* Save current state */
		*sp = cs;

		if (cs->command != AvsParserDefault) {
			if (lt == AVS_PARSER_EMPTY) 
				lt = parsetranslate[avs_lexer_token(px->lexer, &cv.token)];
			
			if (lt <= AVS_PARSER_EOF) {
				lt = AVS_PARSER_EOF;
				avs_debug(print("parser: Now at end of input."));
			} else 
				avs_debug(print("parser: Next token is: %d", lt));

			cs = cs + lt;
		}

		switch (cs->command) {
			case AvsParserError:
				avs_debug(print("parser: erronous state!"));
				report_error(px, lt, parsererror + AVS_PARSER_STATE(*sp));

				/* Rollback assembler & reset compiler stack */
				avs_compile_marker(px->compiler, 
						   obj,
						   AvsCompilerMarkerRollback,
						   NULL);
				avs_compile_reset_stack(px->compiler);

				/* Reset parser stacks */
				sp = (AvsParserState **) px->sstack->base - 1;
				vp = (AvsParserValue *) px->vstack->base;
				cs = parserstate[0];

				/* Clear input queue until next sequence point has been found */
				if (lt == AVS_PARSER_EOF)
					return VISUAL_OK;
				
				for (;;) {
					if (cv.token.type == AvsLexerTokenPunctuator &&
					    cv.token.v.punctuator == ';')
						break;
					
					if (!avs_lexer_token(px->lexer, &cv.token))
						return VISUAL_OK; /* EOF */

					avs_debug(print("parser: shifting error token..."));
				}

				lt = AVS_PARSER_EMPTY; 
				break;

			case AvsParserDefault: /* Default reduce rule */
			case AvsParserReduce:
				vt = vp[1 - cs->length];
				
				avs_debug(print("parser: Reducing stack by rule %d (argument length: %d)", 
					  cs->index - 1, cs->length));

				/* Reduce rule */
				reduce_rule(px, obj, cs, vp);
				
				/* Cleanup stack */
				vp -= cs->length;
				sp -= cs->length;

				/* Store return value */
				*++vp = vt;
				
				avs_debug(print("parser: Reduced non-terminal symbol: %d", cs->token));
				cs = parserstate[parsergotostate[AVS_PARSER_STATE(*sp)][cs->token]];
				avs_debug(print("parser: Reduced original state: %d", AVS_PARSER_STATE(*sp)));
				avs_debug(print("parser: Reduced new state: %d", AVS_PARSER_STATE(cs)));
				break;

			case AvsParserShift:
				/* Shift */
				avs_debug(print("parser: Shifting token %d, storing: '%c'", 
					       lt, (isprint(cv.token.v.punctuator) ?
					                    cv.token.v.punctuator : '?')));

				if (lt != AVS_PARSER_EOF)
					lt = AVS_PARSER_EMPTY;

				/* Store look-ahead value */
				*++vp = cv;
				cs = parserstate[cs->index];
				break;

			case AvsParserAccept:
				/* return */
				avs_debug(print("parser: Accept rule!"));
				return VISUAL_OK;
		}
	
	}

	return VISUAL_OK;
}

/**
 * Cleanup a parser context.
 *
 * @param px Parser context to destuct.
 *
 * @return Nothing
 */
void avs_parser_cleanup(AvsParserContext *px)
{
	visual_object_unref(VISUAL_OBJECT(px->sstack));
	visual_object_unref(VISUAL_OBJECT(px->vstack));
}


/**
 * Initialize a parser context. 
 * Associates a lexer and compiler context with the parser.
 * Allocates initial stack.
 *
 * @param px Parser context to initialize.
 * @param lx Lexer context to associate with parser.
 * @param cx Compiler context to associate with parser.
 *
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error (out of mem).
 */
int avs_parser_init(AvsParserContext *px, AvsLexerContext *lx, AvsCompilerContext *cx)
{
	memset(px, 0, sizeof(AvsParserContext));

	/* Associate lexer and compiler with parser context */
	px->lexer = lx;
	px->compiler = cx;

	/* Allocate stacks */
	px->sstack = avs_stack_new0(AvsParserState *, 
				    AVS_PARSER_INITIAL_STACKSIZE,
				    AVS_PARSER_MAXIMUM_STACKSIZE);
	px->vstack = avs_stack_new0(AvsParserValue, 
				    AVS_PARSER_INITIAL_STACKSIZE,
				    AVS_PARSER_MAXIMUM_STACKSIZE);
	return VISUAL_OK;
}

