%{
#include <stdio.h>
#include <stdlib.h>

// Declare functions defined by the lexer
extern int yylex();
%}
 
/* list the supported tokens */
%token BOOL
%token PLUS MUL MINUS DIV ASSIGN EQUAL NOT_EQUAL
%token LT LE GT GE COMMA COLON SEMICOLON LPARENTHESIS RPARENTHESIS
%token LBRACKET RBRACKET LBRACE RBRACE AND OR INC DEC
%token INT FLOAT TYPE QUALIFIER KEYWORD SWIZZLE STATE IDENTIFIER

/* indicate which of the below nodes is the root of the parse tree
 * (defaults to first rule) */
%start root_node

%%

root_node:  hello_node  ;

hello_node:  SEMICOLON 
  { printf ( "parsed a hello node !\nHello, user !\n");};


%%

int main(int argc,char* argv[])
{
	int yyin;
	// we assume that the input file is given as input as first argument
  ++argv, --argc;   
  if ( argc > 0 )
    yyin = fopen( argv[0], "r" );
  else
    yyin = stdin;
    yyparse();
    return 0;
}

int yyerror(char* msg)
{
	fprintf(stderr, "error: %s\n", msg);
	return 0;
}
