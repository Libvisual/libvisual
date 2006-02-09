#include <stdio.h>
#include <stdlib.h>

int yyerror (char *s) 	  { return 0; }
int yylex(YYSTYPE *lvalp) { return 0; }

enum commands { Default, Error, Reduce, Shift, Accept, };
char *name[] = { "D", "E", "R", "S", "A", };

struct state {
	unsigned char	command;
	unsigned char	index;
	unsigned char	token;
	unsigned char	length;
};

/* Lexer tokens to bison tokens mapping, order is important */
int types[] = { YYEOF, YYEOF, IDENTIFIER, CONSTANT, -1 };

/* Lexer punctuators to bison tokens mapping, order isn't important */
char *puncs = ",;=()&-+*%|/";

void emit_state(struct state *s)
{
	fprintf(stdout, "%s(%2d,%2d,%2d),", name[s->command], s->index, s->token, s->length);
}

void generate_parser_table(void)
{
	int v, state, symbol, max, xb, xe, xx, xor;
	static unsigned char translate[256];
	static signed char gotojumptable[YYNSTATES][YYNNTS];
	struct state sinfo, *s = &sinfo;
		    
	fprintf(stdout, "#ifndef _AVS_PARSERTABLES_H\n"
			"#define _AVS_PARSERTABLES_H 1\n\n"
			"#define AVS_PARSER_STATES %2d\n"
			"#define AVS_PARSER_TOKENS %2d\n\n",
			YYNSTATES, YYNTOKENS);

	fprintf(stdout, "static AvsParserState parserstate[AVS_PARSER_STATES][AVS_PARSER_TOKENS] = {\n");
	for (state=0; state < YYNSTATES; state++) {
		fprintf(stdout, "\t/* %2d */ {\n\t\t", state);

		/* Calculate goto jumptable */
		for (symbol=0; symbol < YYNNTS; symbol++) {
			v = state + yypgoto[symbol];
			if (v >= 0 && v <= YYLAST && yycheck[v] == state)
				gotojumptable[state][symbol] = yytable[v];
			else
				gotojumptable[state][symbol] = yydefgoto[symbol];
		}

		/* Calculate state tables */
		if (yypact[state] == YYPACT_NINF) {
			/* Default action */
			v = yydefact[state];
			if (v == 0) {
				/* Error */
				s->command = Error;
				s->index = s->token = s->length = 0;
				emit_state(s);
			} else {
				/* Default reduce rule */
				s->command = Default;
				s->index = v;
				s->token = yyr1[v] - YYNTOKENS;
				s->length = yyr2[v];
				emit_state(s);
				s->command = Reduce;
			}
			
			for (symbol=1; symbol < YYNTOKENS; symbol++) {
				emit_state(s);
				if ((symbol + 1) % (YYNTOKENS / 3) == 0)
					fprintf(stdout, "\n\t\t");
			}
		} else {
			/* Decide action */
			for (symbol=0; symbol < YYNTOKENS; symbol++) {
				v = yypact[state] + symbol;
				memset(s, 0, sizeof(struct state));
				if (v < 0 || v > YYLAST || yycheck[v] != symbol) {
					/* Default action */
					/* DEFAULT() || ERROR() */
					v = yydefact[state];
					if (v == 0) {
						s->command = Error;
					} else {
						s->command = Reduce;
						s->index = v;
						s->token = yyr1[v] - YYNTOKENS;
						s->length = yyr2[v];
					}
				} else {
					v = yytable[v];
					if (v <= 0) {
						if (v == 0 || v == YYTABLE_NINF) {
							/* Error */
							s->command = Error;
						} else {
							/* Reduce */
							v = -v;
							s->command = Reduce;
							s->index = v;
							s->token = yyr1[v] - YYNTOKENS;
							s->length = yyr2[v];
						}
					} else {
						if (v == YYFINAL) {
							/* Accept */
							s->command = Accept;
						} else {
							/* Shift */
							s->command = Shift;
							s->index = v;
						}
					}

				}
				
				emit_state(s);
				if ((symbol + 1) % (YYNTOKENS / 3) == 0)
					fprintf(stdout, "\n\t\t");
			}
		}
		fprintf(stdout, "\n\t},\n");
	}
	fprintf(stdout, "};\n\n");

	max = 0;
	for (v=0; types[v] != -1; v++) {
		translate[v] = YYTRANSLATE(types[v]);
		max = (v > max ? v : max);
	}
	
	for (v=0; v < strlen(puncs); v++) {
		translate[puncs[v]] = YYTRANSLATE(puncs[v]);
		max = (puncs[v] > max ? puncs[v] : max);
	}

	fprintf(stdout, "static unsigned char parsetranslate[256] = {\n\t");
	
	for (v=0; v <= max; v++) {
		fprintf(stdout, "%2d, ", translate[v]);
		if ((v + 1) % 16 == 0)
			fprintf(stdout, "\n\t");
	}
	
	fprintf(stdout, "\n};\n\n");

#if 0
	fprintf(stdout, "static unsigned char parseruleargs[%d] = {\n\t", YYNRULES+1);
	for (v=0; v <= YYNRULES; v++) {
		fprintf(stdout, "%2d,", yyr2[v]);
		if ((v + 1) % 16 == 0)
			fprintf(stdout, "\n\t");
	}
	fprintf(stdout, "\n};\n\n");
#endif

	fprintf(stdout, "static signed char parsergotostate[%d][%d] = {\n", YYNSTATES, YYNNTS);
	for (state=0; state < YYNSTATES; state++) {
		fprintf(stdout, "\t{ ");
		for (symbol=0; symbol < YYNNTS; symbol++) {
			fprintf(stdout, "%2d,", gotojumptable[state][symbol]);
		}
		fprintf(stdout, " },\n");
	}
	fprintf(stdout, "};\n\n");

#if YYDEBUG
	fprintf(stdout, "static AvsParserErrorState parsererror[AVS_PARSER_STATES] = {\n");
	for (state=0; state < YYNSTATES; state++) {
		v = yypact[state];
		if (v > YYPACT_NINF && v < YYLAST) {
			xb = 0;
			if (v < 0)
				xb = -v;

			xe = YYLAST - v;
			if (xe >= YYNTOKENS)
				xe = YYNTOKENS;

			if (xe - xb > 32) {
				fprintf(stderr, "WARNING: Error bitmask overflow!\n");
			}
			fprintf(stderr, "NOTICE: ||| state = %d, start = %d, end = %d\n",
					state, xb, xe);
			xor = 0;
			for (xx=xb; xx < xe; xx++) {
				fprintf(stderr, "NOTICE: xx = %d, yycheck = %d, YYTERROR = %d\n", xx, yycheck[v + xx], YYTERROR);
				if (yycheck[v + xx] != xx ||
						xx == YYTERROR)
					continue;

				fprintf(stderr, "NOTICE: --- Adding ---\n");
				xor |= 1 << (xx - xb);
			}		
		} else
			xb = xe = xor = 0;
		
		fprintf(stdout, "\t{ %2d, %2d, 0x%08x },\n", xb, xe, xor);
		
	}
	fprintf(stdout, "};\n\n");
	
	fprintf(stdout, "static char *parsertokenname[%d] = {\n\t", YYNTOKENS);
	for (state=0, v = 8; state < YYNTOKENS; state++) {
		v += strlen(yytname[state]) + 4;
		if (v >= 80) {
			v = 8;
			fprintf(stdout, "\n\t");
		}
		fprintf(stdout, "\"%s\", ", yytname[state]);
	}

	if (v > 8)
		fputc('\n', stdout);
	fprintf(stdout, "};\n\n");
#endif
	fprintf(stdout, "#endif /* !_AVS_PARSERTABLES_H */\n");
}

void generate_switch_table(char *file)
{
	FILE *fp;
	char **line, buf[4096], *p;
	int linebuffer = 8192, lines = 0;
	int i;
	
	if ((fp=fopen(file, "r")) == NULL) {
		fprintf(stderr, "Unable to open file: '%s'\n", file);
		exit(1);
	}
	
	line = malloc(sizeof(char *) * linebuffer);
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		p = buf + strlen(buf) - 1;
		while (p > buf && isspace(*p))
			*p-- = 0;

		p = buf;
		while (*p && isspace(*p))
			p++;

		line[lines++] = strdup(p);
		if (lines >= linebuffer) {
			linebuffer <<= 1;
			line = realloc(line, sizeof(char *) * linebuffer);
		}
	}

	fclose(fp);

	for (i=2; i <= YYNRULES; i++) {
		while((p=strstr(line[yyrline[i]-1], "/*")))
			p[0] = p[1] = '[';
		while((p=strstr(line[yyrline[i]-1], "*/")))
			p[0] = p[1] = ']';
		fprintf(stdout, "\t\tcase %2d: /* Rule: '%s' */\n", i, line[yyrline[i]-1]);
	}
		
}

int main(int argc, char **argv)
{
	if (argc > 2 && !strcasecmp(argv[1], "switchtable"))
		generate_switch_table(argv[2]);
	else
		generate_parser_table();
	
	return 0;	
}

