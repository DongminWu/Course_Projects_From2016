/*Jiapeng Li 0387565, Dongmin Wu 0387563, Yuan Zhang 0387552 */
/*Count and prints the number of lines. */
%option noyywrap
%option yylineno
%{
#include <stdio.h>
#include <string.h>
#include "Li_Wu_Zhang.yy.h"

#define YYFPRINTF printf

/*dongmin: I redefined a printf funciton here, to add the line number at beginning of each line*/
#define yylog(x, ... ) /*printf("[:%d]",num_lines);*/printf( (x), ##__VA_ARGS__)/*;printf("\n")*/
int num_lines = 1, num_chars = 0; 
%}  


%x COMMENT

    
DIGIT    [0-9]
ID       [a-zA-Z_][a-zA-Z0-9_]*

TYPE_NAME_DEFAULT_TYPE int|float|bool|void
TYPE_NAME_VECTOR_TYPE vec2|vec3|vec4|ivec2|ivec3|ivec4|bvec2|bvec3|bvec4
TYPE_NAME_BUILT_IN_TYPE rt_Primitive|rt_Camera|rt_Material|rt_Texture|rt_Light
TYPE_NAME bool|void|vec2|vec3|vec4|ivec2|ivec3|ivec4|bvec2|bvec3|bvec4|rt_Primitive|rt_Camera|rt_Material|rt_Texture|rt_Light

QUALIFIER_NAME_VARIABLE_QUALIFIERS attribute|uniform|varying|const
QUALIFIER_NAME_CLASS_MODIFIERS const|public|private|scratch
QUALIFIER_NAME attribute|uniform|varying|const|public|private|scratch

KEYWORD class|break|const|continue|do|double|enum|extern|for|goto|sizeof|static|struct|typedef|union|unsigned|while|illuminance|ambient|dominantAxis|dot|hit|inside|inverse|luminance|max|min|normalize|perpendicularTo|pow|rand|reflect|sqrt|trace

PLUS "+"
MUL "*"
MINUS "-"
DIV "/"
ASSIGN "="
EQUAL "=="
NOT_EQUAL "!="
LT "<"
LE "<="
GT ">"
GE ">="
COMMA ","
COLON ":"
SEMICOLON ";"
LPARENTHESIS "("
RPARENTHESIS ")"
LBRACKET "["
RBRACKET "]"
LBRACE "{"
RBRACE "}"
AND "&&"
OR "||"
INC "++"
DEC "--"



%%     


\n	{num_lines++;}/*increased the line num*/

"/*"		{/*printf("COMMENT BLOCK BEGIN ===>\n");*/BEGIN(COMMENT);}
<COMMENT>"/*"
<COMMENT>\n	{num_lines++;}/*increased the line num*/
<COMMENT>"*"\n 		{num_lines++;}
<COMMENT>[^"*"]
<COMMENT>"*"[^"/"]	   

<COMMENT>"*/"		{/*printf("COMMENT BLOCK END <===\n");*/BEGIN(INITIAL);}


"//".*	{}


true|false	{ yylval = (strcmp(yytext, "true") == 0); return BOOL;}

if {return IF;}
else {return ELSE;}
case {return CASE;}
default {return DEFAULT;}
switch {return SWITCH;}

int {return TYPE_INT;}
float {return TYPE_FLOAT;}
{PLUS}	{return PLUS;} 
{MUL}	{return MUL;} 
{MINUS}	{return MINUS;} 
{DIV}	{return DIV;} 
{ASSIGN}	{return ASSIGN;} 
{EQUAL}	{return EQUAL;} 
{NOT_EQUAL}	{return NOT_EQUAL;} 
{LT}	{return LT;} 
{LE}	{return LE;} 
{GT}	{return GT;} 
{GE}	{return GE;} 
{COMMA}	{return COMMA;} 
{COLON}	{return COLON;} 
{SEMICOLON}	{return SEMICOLON;} 
{LPARENTHESIS}	{return LPARENTHESIS;} 
{RPARENTHESIS}	{return RPARENTHESIS;} 
{LBRACKET}	{return LBRACKET;} 
{RBRACKET}	{return RBRACKET;} 
{LBRACE}	{return LBRACE;} 
{RBRACE}	{return RBRACE;} 
{AND}	{return AND;} 
{OR}	{return OR;} 
{INC}	{return INC;} 
{DEC}	{return DEC;} 



0[0-7]+(u|U)?	{ return INT; }
0[xX][0-9A-Fa-f]+(u|U)?	{ return INT; }
{DIGIT}+(u|U)?	{ return INT; }
     


{DIGIT}+"."{DIGIT}*(lf|f|LF|F)?	{ return FLOAT; }
{DIGIT}*"."{DIGIT}+[eE]?("-"|"+")?{DIGIT}*(lf|f|LF|F)?	{ return FLOAT; }
{DIGIT}+"."{DIGIT}*[eE]("-"|"+")?{DIGIT}*(lf|f|LF|F)?	{ return FLOAT; }
{DIGIT}+[eE]("-"|"+")?{DIGIT}*(lf|f|LF|F)?	{ return FLOAT; }
     

{TYPE_NAME}		{return TYPE ;}
{QUALIFIER_NAME}	{return QUALIFIER ;}
{KEYWORD}	{return KEYWORD ;}     




"."{ID}		{yytext++;return SWIZZLE ;}


     



{ID}	{return IDENTIFIER;}     

rt_{ID}		{return STATE;}
     
[ \t\r]+	/* eat up whitespace */
     
.	yylog( "ERROR(%d): Unrecognized symbol \"%s\"\n", num_lines,yytext  );


%%


int main(int argc,char* argv[])
{
	
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
