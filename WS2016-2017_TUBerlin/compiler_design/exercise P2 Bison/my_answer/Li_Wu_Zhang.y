/*Jiapeng Li 0387565, Dongmin Wu 0387563, Yuan Zhang 0387552 */

/*TODO!!! issue, what should I do if the built-in function used as identifier*/

%{
#include <stdio.h>
#include <stdlib.h>

#define YYFPRINTF printf





#define yylog(x, ... ) printf( (x), ##__VA_ARGS__);printf("<-")


// Declare functions defined by the lexer
extern int yylex();


typedef struct _rtsl_class
{
	char* original_name;
	char* show_name;
	char* property[32];
	char* method[32];
} rtsl_class, *prtsl_class;


rtsl_class rt_Camera = {
/*original_name*/	"rt_Camera",
/*show_name*/		"camera", 
/*property*/		{"rt_RayOrigin","rt_RayDirection","rt_InverseRayDirection","rt_Epsilon","rt_HitDistance", "rt_ScreenCoord", "rt_LensCoord", "rt_du", "rt_dv", "rt_TimeSeed"},
/*method*/		{"constructor","generateRay"}
		};

rtsl_class rt_Primitive = {
/*original_name*/	"rt_Primitive",
/*show_name*/		"primitive", 
/*property*/		{"rt_RayOrigin","rt_RayDirection","rt_InverseRayDirection","rt_Epsilon","rt_HitDistance", "rt_BoundMin", "rt_BoundMax", "rt_GeometricNormal","rt_dPdu", "rt_dPdv", "rt_ShadingNormal", "rt_TextureUV", "rt_TextureUVW", "rt_dsdu","rt_dsdv", "rt_PDF", "rt_TimeSeed"},
/*method*/		{"constructor","intersect","computeBounds", "computeNormal", "computeTextureCoordinates", "computeDerivatives", "generateSample", "samplePDF"}
		};

rtsl_class rt_Texture = {
/*original_name*/	"rt_Texture",
/*show_name*/		"texture", 
/*property*/		{"rt_TextureUV","rt_TextureUVW","rt_TextureColor","rt_FloatTxtureValue","rt_du", "rt_dv","rt_dsdu","rt_dtdu","rt_dsdv","rt_dtdv", "rt_dPdu", "rt_dPdv" "rt_TimeSeed"},
/*method*/		{"constructor","lookup"}
		};


rtsl_class rt_Material = {
/*orginal_name*/	"rt_Material",
/*show_name*/		"material", 
/*property*/		{"rt_RayOrigin","rt_RayDirection","rt_InverseRayDirection","rt_HitPoint", "rt_dPdu", "rt_dPdv","rt_LightDirection","rt_LightDistance","rt_LightColor","rt_EmissionColor","rt_BSDFSeed", "rt_TimeSeed", "rt_PDF", "rt_SampleColor","rt_BSDFValue", "rt_du","rt_dv"},
/*method*/		{"constructor","shade", "BSDF", "sampleBSDF", "evaluatePDF", "emission"}
		};

rtsl_class rt_Light = {
/*original_name*/	"rt_Light",
/*show_name*/		"light", 
/*property*/		{"rt_HitPoint","rt_GeometricNormal","rt_ShadingNormal", "rt_LightDirection", "rt_TimeSeed"},
/*method*/		{"constructor","illumination"}
		};


prtsl_class all_shader_classes[5] = {&rt_Camera, &rt_Primitive, &rt_Texture, &rt_Material, &rt_Light};

prtsl_class current_shader = NULL;


int shader_detecting(const char* shader_name)
{
	/*printf("\n[debug] >>> %s\n\n", __FUNCTION__);*/
	prtsl_class * shader_indicator = all_shader_classes;
	while(*shader_indicator)
	{

		if ( 0 == strcmp ((*shader_indicator)->original_name,shader_name))
		{
			current_shader = (*shader_indicator);
			/*printf("\n[debug]entering shading! %s\n", (*shader_indicator)->original_name);*/
			break;
		}
	   	else  
		{
			shader_indicator ++;
		}
	}
	/*printf("\n[debug] <<< %s\n\n", __FUNCTION__);*/
	return 0;
}

int check_property_matching(const char* string, const char* item_name)
{
	/*printf("\n[debug] >>> %s\n\n", __FUNCTION__);*/
	/*printf("\n[debug] %s name:  %s\n\n",item_name,  string);*/
	char** indicator  = NULL;
	if (0 == strcmp(item_name, "property"))
		indicator = current_shader->property;
	else if (0 == strcmp(item_name, "method"))
		indicator = current_shader->method;
	else return 2;
	while (*indicator)
	{
		if ( 0 == strcmp(*indicator,string))
		{
			/*printf("\n[debug]don't worry!!match!!");*/
			return 0;
		}
		indicator++;
	}
	return 1;

}


%}

/*value with the prefix of rt_ should treat as string*/
%union{
	char * strval;
	int intval;
}
%type <strval> TYPE
%type <strval> STATE
%type <strval> IDENTIFIER
%type <strval> declarator
%type <strval> direct_declarator

%error-verbose
 
/* list the supported tokens */


/*TODO!!! unused BOOL*/
%token BOOL
%token PLUS MUL MINUS DIV ASSIGN EQUAL NOT_EQUAL
%token LT LE GT GE COMMA COLON SEMICOLON LPARENTHESIS RPARENTHESIS
%token LBRACKET RBRACKET LBRACE RBRACE AND OR INC DEC
%token INT FLOAT TYPE QUALIFIER KEYWORD SWIZZLE STATE IDENTIFIER
%token IF ELSE CASE DEFAULT SWITCH
%token TYPE_INT TYPE_FLOAT
%token RETURN
%token WHILE DO FOR 
%token RIGHT_ASSIGN LEFT_ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN AND_ASSIGN XOR_ASSIGN OR_ASSIGN

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
	| jump_statement       {yylog("jump_statement");}
	|compound_statement {yylog("compound_statement");}
	|expression_statement  {yylog("expression_statement");}
	| selection_statement   {yylog("selection_statement");}
	| iteration_statement    {yylog("iteration_statement");}
	;


iteration_statement
	: WHILE LPARENTHESIS expression RPARENTHESIS statement
	| DO statement WHILE LPARENTHESIS expression RPARENTHESIS SEMICOLON
	| FOR LPARENTHESIS expression_statement expression_statement RPARENTHESIS statement
	| FOR LPARENTHESIS expression_statement expression_statement expression RPARENTHESIS statement
	| FOR LPARENTHESIS declaration expression_statement RPARENTHESIS statement
	| FOR LPARENTHESIS declaration expression_statement expression RPARENTHESIS statement
	;


jump_statement
	: RETURN SEMICOLON 
	| RETURN expression SEMICOLON
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
	: IF LPARENTHESIS expression RPARENTHESIS statement ELSE statement  {printf("IF - ELSE\n");}/*TA said bison will automatically choose the first production*/
	| IF LPARENTHESIS expression RPARENTHESIS statement   {printf("IF\n");}
	| SWITCH LPARENTHESIS expression RPARENTHESIS statement
	;
	


primary_expression 
	: IDENTIFIER
	| KEYWORD /*sqrt is a KEYWORD*/
	| TYPE   /* to match requirement*/
	| STATE  /*for shader*/ {check_property_matching($1,"property");}
	| BOOL
	| constant
	| LPARENTHESIS expression RPARENTHESIS
	;

constant
	: INT		/* includes character_constant */
	| FLOAT
	;


postfix_expression
	: primary_expression  {yylog("primary_expression");}
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
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator
	: ASSIGN
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
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
	| */direct_declarator    { $$= $1;yylog("direct_declarator");}
	;

declarator_for_func
	:direct_declarator LPARENTHESIS RPARENTHESIS
	;

/*TODO!!!!!*/
direct_declarator
	: IDENTIFIER    {/*function name starts from here*/ $$= $1;}
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
	: declaration_specifiers declarator declaration_list compound_statement {check_property_matching($2,"method")}
	| declaration_specifiers declarator compound_statement  {check_property_matching($2,"method")}
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



/*****rt_ shading start**/

shader_declaration
	:KEYWORD IDENTIFIER COLON TYPE SEMICOLON {shader_detecting($4);printf("SHADER_DEF %s\n", current_shader->show_name);}
	;



/*****rt_ shading end****/





identifier_list
	: IDENTIFIER
	| identifier_list COMMA IDENTIFIER
	;




translation_unit
	: external_declaration
	| translation_unit external_declaration
	;

external_declaration
	: function_definition     {printf("FUNCTION_DEF\n")}
	| declaration     {printf("DECLARATION\n")}
	| shader_declaration
	;


%%

