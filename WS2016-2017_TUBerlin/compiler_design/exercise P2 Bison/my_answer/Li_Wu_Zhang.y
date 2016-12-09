/*Jiapeng Li 0387565, Dongmin Wu 0387563, Yuan Zhang 0387552 */


%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define YYFPRINTF printf





#define debug_flow(x, ... ) //printf( (x), ##__VA_ARGS__);printf("<-")
#define debug_log(x, ... )  //printf("\n[Debug]");printf( (x), ##__VA_ARGS__);printf("\n\n")
#define log(x, ... )  printf( (x), ##__VA_ARGS__);


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
/*property*/		{"rt_RayOrigin","rt_RayDirection","rt_InverseRayDirection","rt_Epsilon","rt_HitDistance", "rt_BoundMin", "rt_BoundMax", "rt_GeometricNormal","rt_dPdu", "rt_dPdv", "rt_ShadingNormal", "rt_TextureUV", "rt_TextureUVW", "rt_dsdu","rt_dsdv", "rt_PDF", "rt_TimeSeed",/*TA said we can add following two properties in Material*/"rt_HitPoint"},
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
/*property*/		{"rt_RayOrigin","rt_RayDirection","rt_InverseRayDirection","rt_HitPoint", "rt_dPdu", "rt_dPdv","rt_LightDirection","rt_LightDistance","rt_LightColor","rt_EmissionColor","rt_BSDFSeed", "rt_TimeSeed", "rt_PDF", "rt_SampleColor","rt_BSDFValue", "rt_du","rt_dv",/*TA said we can add following two properties in Material*/"rt_ShadingNormal","rt_HitDistance"},
/*method*/		{"constructor","shade", "BSDF", "sampleBSDF", "evaluatePDF", "emission"}
		};

rtsl_class rt_Light = {
/*original_name*/	"rt_Light",
/*show_name*/		"light", 
/*property*/		{"rt_HitPoint","rt_GeometricNormal","rt_ShadingNormal", "rt_LightDirection", "rt_TimeSeed"},
/*method*/		{"constructor","illumination"}
		};


prtsl_class all_shader_classes[5] = {&rt_Camera,  &rt_Texture,&rt_Primitive, &rt_Material, &rt_Light};

prtsl_class current_shader = NULL;


static property_has_msg = 0;
static method_has_msg = 0;


int find_string_in_set(const char* string, const char ** string_set)
{
	int ret = 1;
	debug_log(" >>> %s", __FUNCTION__);
	char** indicator  = string_set;
	while (*indicator)
	{
		if ( 0 == strcmp(*indicator,string))
		{
			ret = 0;
			break;
		}
		indicator++;
	}
	debug_log(" <<< %s, (%d)", __FUNCTION__,ret);
	return ret;

}

int shader_detecting(const char* shader_name)
{
	debug_log(" >>> %s", __FUNCTION__);
	prtsl_class * shader_indicator = all_shader_classes;
	while(*shader_indicator)
	{

		if ( 0 == strcmp ((*shader_indicator)->original_name,shader_name))
		{
			current_shader = (*shader_indicator);
			debug_log("entering shading! %s\n", (*shader_indicator)->original_name);
			break;
		}
	   	else  
		{
			shader_indicator ++;
		}
	}
	debug_log(" <<< %s", __FUNCTION__);
	return 0;
}


void error_msg_property(char* msg, const char* current, const char* input)
{
	
 	if (property_has_msg == 0  && method_has_msg ==0)
	{
		sprintf(msg, "Error: %s cannot access to a state of %s\n",current, input);
		property_has_msg =1;
	}

}

void error_msg_method(char* msg, const char* current, const char* input)
{
	
 	if (method_has_msg == 0)
	{
		sprintf(msg, "Error: %s cannot have an interface method of %s\n",current, input);
		method_has_msg =1;
	}

}
int check_property_matching(const char* input_string, const char* item_name)
{
	debug_log(" >>> %s, (%s,%s)", __FUNCTION__,input_string,item_name);
	char errormsg[256];
	int ret = 0;
	char* string = strdup(input_string);
	char** current_string_set  = NULL;
	if (0 == strcmp(item_name, "property"))
		current_string_set = current_shader->property;
	else if (0 == strcmp(item_name, "method"))
		current_string_set = current_shader->method;
	else {ret = 2;goto END;}
	debug_log("finding in current shader class..");
	if ( 0 == find_string_in_set(string, current_string_set))
	{
		/*if the indentifier belongs to current shader class
		  no error*/
		ret = 0;
		goto END;
	}
	debug_log("finished, not in current shader class");
	
	debug_log("finding in other shader class..");
	for (int i = 0; i < sizeof(all_shader_classes)/sizeof(prtsl_class); i++)
	{
		debug_log("i=%d, sizeof(all_shader_classes)=%d",i,sizeof(all_shader_classes)/sizeof(prtsl_class));
		/*identifier cannot belongs to other shader class*/
		if (all_shader_classes[i] == current_shader)
			continue;/*exclude current shader, who was checked in last step*/
		else
		{
			if (0 == strcmp(item_name, "property"))
				current_string_set = all_shader_classes[i]->property;
			else if (0 == strcmp(item_name, "method"))
				current_string_set = all_shader_classes[i]->method;
			else {ret = 2;goto END;}
			if ( 0 == find_string_in_set (string, current_string_set))
			{
				if (0 == strcmp(item_name, "property"))
					error_msg_property(errormsg, current_shader->show_name, all_shader_classes[i]->show_name);
				else if (0 == strcmp(item_name, "method"))
					error_msg_method(errormsg, current_shader->show_name, all_shader_classes[i]->show_name);
				fprintf(stderr, errormsg);
				ret=1;
				goto END;
			}
		}
	}
	debug_log("finished, not in other shader class");

END:
   	
	debug_log(" <<< %s, (%s,%s)->(%d)", __FUNCTION__,string,item_name,ret);
	return ret;

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
	: block_item   	{debug_flow("block_item")}
	| block_item_list block_item  {debug_flow(" block_item list")}
	;

block_item
	:declaration {log("DECLARATION\n");} 
	|statement 	
	;



statement
	:labeled_statement	{log("STATEMENT\n");}
	| jump_statement       {log("STATEMENT\n");}
	|compound_statement {log("STATEMENT\n");}
	|expression_statement  {log("STATEMENT\n");}
	| selection_statement   {log("STATEMENT\n");}
	| iteration_statement    {log("STATEMENT\n");}
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
	: RETURN SEMICOLON  {debug_flow("RETURN SEMICOLON");}
	| RETURN expression SEMICOLON  {debug_flow("RETURN expression SEMICOLON");}

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
	| expression SEMICOLON	{debug_flow("expression;");}	
	;



selection_statement
	: IF LPARENTHESIS expression RPARENTHESIS statement ELSE statement  {log("IF - ELSE\n");}/*TA said bison will automatically choose the first production*/
	| IF LPARENTHESIS expression RPARENTHESIS statement   {log("IF\n");}
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
	: primary_expression  {debug_flow("primary_expression");}
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
	: declaration   {log("DECLARATION\n");} 
	| declaration_list declaration   {log("DECLARATION list\n");}
	;

declaration
	: declaration_specifiers SEMICOLON 
	| declaration_specifiers init_declarator_list SEMICOLON 
	;


declaration_specifiers
	: type_qualifier declaration_specifiers  {debug_flow("type_qualifier declaration_specifiers");}
	| type_qualifier
	| type_specifier declaration_specifiers {debug_flow("type_specifier declaration_specifiers");}
	| type_specifier
	;

type_specifier
	: TYPE {debug_flow("other type_specifier");}
	| TYPE_INT   {debug_flow("INT");}
	| TYPE_FLOAT 	{debug_flow("FLOAT");}
	;

type_qualifier
	: QUALIFIER {debug_flow("QUALIFIER");}
	;


init_declarator_list
	: init_declarator    {debug_flow("declarator");}
	| init_declarator_list COMMA init_declarator {debug_flow("declarator with COMMA")};
	;

init_declarator
	: declarator ASSIGN initializer
	| declarator
	;


declarator
	: /*pointer direct_declarator
	| */direct_declarator    { $$= $1;debug_flow("direct_declarator");}
	;

declarator_for_func
	:direct_declarator LPARENTHESIS RPARENTHESIS
	;

/*TODO!!!!!*/
direct_declarator
	: IDENTIFIER    {/*function name starts from here*/ $$= $1;}
	| KEYWORD  /*the case of "inside" as a parameter*/ 
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
	: declaration_specifiers declarator declaration_list {check_property_matching($2,"method");} compound_statement /*TA said this case is a very old style of C language, we can ignore it*/
	| declaration_specifiers declarator  {check_property_matching($2,"method");} compound_statement 
	;

/*****function end*******/


/*****paramater start****/

parameter_type_list
	: parameter_list 	{debug_flow("parameter_list");}
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
	:KEYWORD IDENTIFIER COLON TYPE SEMICOLON {shader_detecting($4);log("SHADER_DEF %s\n", current_shader->show_name);}
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
	: function_definition     {log("FUNCTION_DEF\n")}
	| declaration     {log("DECLARATION\n")}
	| shader_declaration
	;


%%

