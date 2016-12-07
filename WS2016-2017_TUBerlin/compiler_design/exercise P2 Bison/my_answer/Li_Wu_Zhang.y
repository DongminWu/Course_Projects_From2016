/*Jiapeng Li 0387565, Dongmin Wu 0387563, Yuan Zhang 0387552 */
%{
#include <stdio.h>
#include <stdlib.h>

#define YYFPRINTF printf





#define yylog(x, ... ) printf( (x), ##__VA_ARGS__);printf("<-")


// Declare functions defined by the lexer
extern int yylex();


%}


%error-verbose
 
/* list the supported tokens */
%token BOOL
%token PLUS MUL MINUS DIV ASSIGN EQUAL NOT_EQUAL
%token LT LE GT GE COMMA COLON SEMICOLON LPARENTHESIS RPARENTHESIS
%token LBRACKET RBRACKET LBRACE RBRACE AND OR INC DEC
%token INT FLOAT TYPE QUALIFIER KEYWORD SWIZZLE STATE IDENTIFIER
%token IF ELSE

/* indicate which of the below nodes is the root of the parse tree
 * (defaults to first rule) */
%start block_item_list

%%




block_item_list
	: block_item
	| block_item_list block_item
	;

block_item
	: statement 	{printf("statement\n");}
	;

statement
	:
	labeled_statement	{yylog("labeled_statement");}
	|compound_statement {yylog("compound_statement");}
	|expression_statement  {yylog("expression_statement");}
	| selection_statement
	/*| iteration_statement
	| jump_statement*/
	;

compound_statement
	: LBRACE RBRACE
	| LBRACE  block_item_list RBRACE
	;

labeled_statement
	: IDENTIFIER COLON statement /* for goto, switch..case*/
	| KEYWORD constant_expression COLON statement
	| KEYWORD COLON statement
	;


expression_statement
	: SEMICOLON		
	| expression SEMICOLON	{yylog("expression;");}	
	;


selection_statement
	: IF LPARENTHESIS expression RPARENTHESIS statement ELSE statement /*TA said bison will automatically choose the first production*/
	| IF LPARENTHESIS expression RPARENTHESIS statement
	| SWITCH LPARENTHESIS expression RPARENTHESIS statement
	;


primary_expression
	: IDENTIFIER
	| constant
	| LPARENTHESIS expression RPARENTHESIS
	;

constant
	: INT		/* includes character_constant */
	| FLOAT
	;


postfix_expression
	: primary_expression
	| postfix_expression LBRACKET expression RBRACKET
	| postfix_expression LPARENTHESIS RPARENTHESIS
	| postfix_expression LPARENTHESIS argument_expression_list RPARENTHESIS
	| postfix_expression SWIZZLE IDENTIFIER
	| postfix_expression INC
	| postfix_expression DEC
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list COMMA assignment_expression
	;

unary_expression
	: postfix_expression
	| INC unary_expression
	| DEC unary_expression
	;



cast_expression
	: unary_expression
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression MUL cast_expression
	| multiplicative_expression DIV cast_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression PLUS multiplicative_expression
	| additive_expression MINUS multiplicative_expression
	;



relational_expression
	: additive_expression
	| relational_expression LT additive_expression
	| relational_expression GT additive_expression
	| relational_expression LE additive_expression
	| relational_expression GE additive_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQUAL relational_expression
	| equality_expression NOT_EQUAL relational_expression
	;


logical_and_expression
	: equality_expression
	| logical_and_expression AND equality_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR logical_and_expression
	;

conditional_expression
	: logical_or_expression
	;

assignment_expression
	: conditional_expression
	| unary_expression ASSIGN assignment_expression
	;



expression
	: assignment_expression
	| expression COMMA assignment_expression
	;


constant_expression
	: conditional_expression	/* with constraints */

%%

