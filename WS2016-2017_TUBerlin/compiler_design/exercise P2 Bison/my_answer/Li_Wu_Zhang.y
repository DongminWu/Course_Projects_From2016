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
%token IF ELSE CASE DEFAULT SWITCH
%token TYPE_INT TYPE_FLOAT

/* indicate which of the below nodes is the root of the parse tree
 * (defaults to first rule) */
%start translation_unit

%%




/*****statement start****/

block_item_list
	: block_item   	{yylog("block_item")}
	| block_item_list block_item  {yylog(" block_item list")}
	;

block_item
	:declaration {printf("DECLARATION\n");} 
	|statement 	{printf("STATEMENT\n");}
	;



/*TODO!! finish loop and jump*/
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
	| CASE constant_expression COLON statement
	| DEFAULT COLON statement
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
	| KEYWORD /*sqrt is a KEYWORD*/
	| TYPE   /* to match requirement*/
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
	| unary_operator cast_expression
	;


unary_operator
	: PLUS
	| MINUS
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

/*****statement end******/

/*****declaration start**/

declaration_list
	: declaration   {printf("DECLARATION\n");} 
	| declaration_list declaration   {printf("DECLARATION list\n");}
	;

declaration
	: declaration_specifiers SEMICOLON 
	| declaration_specifiers init_declarator_list SEMICOLON 
	;


declaration_specifiers
	: type_qualifier declaration_specifiers  {yylog("type_qualifier declaration_specifiers");}
	| type_qualifier
	| type_specifier declaration_specifiers {yylog("type_specifier declaration_specifiers");}
	| type_specifier
	;

type_specifier
	: TYPE {yylog("other type_specifier");}
	| TYPE_INT   {yylog("INT");}
	| TYPE_FLOAT 	{yylog("FLOAT");}
	;

type_qualifier
	: QUALIFIER {yylog("QUALIFIER");}
	;


init_declarator_list
	: init_declarator    {yylog("declarator");}
	| init_declarator_list COMMA init_declarator {yylog("declarator with COMMA")};
	;

init_declarator
	: declarator ASSIGN initializer
	| declarator
	;


declarator
	: /*pointer direct_declarator
	| */direct_declarator    {yylog("direct_declarator");}
	;

declarator_for_func
	:direct_declarator LPARENTHESIS RPARENTHESIS
	;

/*TODO!!!!!*/
direct_declarator
	: IDENTIFIER
	| LPARENTHESIS declarator RPARENTHESIS
	|direct_declarator LPARENTHESIS RPARENTHESIS
	| direct_declarator LPARENTHESIS identifier_list RPARENTHESIS
	| direct_declarator LPARENTHESIS parameter_type_list RPARENTHESIS
	;
	/*
	| direct_declarator LBRACKET RBRACKET
	| direct_declarator LBRACKET '*' RBRACKET
	| direct_declarator LBRACKET STATIC type_qualifier_list assignment_expression RBRACKET
	| direct_declarator LBRACKET STATIC assignment_expression RBRACKET
	| direct_declarator LBRACKET type_qualifier_list '*' RBRACKET
	| direct_declarator LBRACKET type_qualifier_list STATIC assignment_expression RBRACKET
	| direct_declarator LBRACKET type_qualifier_list assignment_expression RBRACKET
	| direct_declarator LBRACKET type_qualifier_list RBRACKET
	| direct_declarator LBRACKET assignment_expression RBRACKET
	*/

initializer
	: LBRACE initializer_list RBRACE
	| LBRACE initializer_list COMMA RBRACE
	| assignment_expression
	;

initializer_list
	: designation initializer
	| initializer
	| initializer_list COMMA designation initializer
	| initializer_list COMMA initializer
	;

designation
	: designator_list ASSIGN
	;

designator_list
	: designator
	| designator_list designator
	;

designator
	: LBRACKET constant_expression RBRACKET
	| SWIZZLE IDENTIFIER
	;

/*****declaration end****/

/*****function start*****/
function_definition
	: declaration_specifiers declarator declaration_list compound_statement
	| declaration_specifiers declarator compound_statement
	;

/*****function end*******/


/*****paramater start****/

parameter_type_list
	: parameter_list 	{yylog("parameter_list");}
	;

parameter_list
	: parameter_declaration
	| parameter_list COMMA parameter_declaration
	;


/*TODO!!!!!*/
parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers
	/*| declaration_specifiers abstract_declarator*/
	;


/*****paramater end******/

identifier_list
	: IDENTIFIER
	| identifier_list COMMA IDENTIFIER
	;




translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition     {printf("FUNCTION\n")}
	| declaration     {printf("DECLARATION\n")}
	;


%%

