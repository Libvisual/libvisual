%token IDENTIFIER
%token CONSTANT
%token FUNCTION

%right '='
%left '|'
%left '&'
%left '+' '-'
%left '*' '/' '%'
%right UNARY 

%%

translation_unit
	: statement_list
	| semicolon
	| /* empty */	
	;

statement_list
	: expr
	| statement_list semicolon expr_or_empty
	;

semicolon
	: ';'
	;
	
expr
	: primary		
	| '-' expr %prec UNARY	
	| '+' expr %prec UNARY	
	| expr '+' expr
	| expr '-' expr		
	| expr '*' expr		
	| expr '/' expr		
	| expr '%' expr		
	| expr '&' expr		
	| expr '|' expr		
	| expr '=' expr		
	| '(' expr ')'		
	;

fn_expr
	: /* empty */
	| {/* argument marker */} expr
	| fn_expr ',' {/* argument marker */} expr
	;
	
primary
	: CONSTANT		
	| IDENTIFIER		
	| IDENTIFIER '(' 
		{/* stack marker */} 
		fn_expr ')'  
	;

expr_or_empty
	: expr
	| /* empty */
	;
	
%%

#include "genparsertable.c"
