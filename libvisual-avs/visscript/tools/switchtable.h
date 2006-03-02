		case  2: /* Rule: ': statement_list' */
		case  3: /* Rule: '| /* empty */' */
		case  4: /* Rule: ': statement' */
		case  5: /* Rule: '| statement_list statement' */
		case  6: /* Rule: ': ';'' */
		case  7: /* Rule: '| expr ';'' */
		case  8: /* Rule: ': primary' */
		case  9: /* Rule: '| '-' expr %prec UNARY' */
		case 10: /* Rule: '| '+' expr %prec UNARY' */
		case 11: /* Rule: '| expr '+' expr' */
		case 12: /* Rule: '| expr '-' expr' */
		case 13: /* Rule: '| expr '*' expr' */
		case 14: /* Rule: '| expr '/' expr' */
		case 15: /* Rule: '| expr '%' expr' */
		case 16: /* Rule: '| expr '&' expr' */
		case 17: /* Rule: '| expr '|' expr' */
		case 18: /* Rule: '| expr '=' expr' */
		case 19: /* Rule: '| '(' expr ')'' */
		case 20: /* Rule: 'fn_expr' */
		case 21: /* Rule: '| expr' */
		case 22: /* Rule: '| fn_expr ',' expr' */
		case 23: /* Rule: ': CONSTANT' */
		case 24: /* Rule: '| IDENTIFIER' */
		case 25: /* Rule: '| IDENTIFIER '(' fn_expr ')'' */
