/*Jiapeng Li 0387565, Dongmin Wu 0387563, Yuan Zhang 0387552 */
/*Count and prints the number of lines. */
%option noyywrap
%option yylineno
%{
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
TYPE_NAME int|float|bool|void|vec2|vec3|vec4|ivec2|ivec3|ivec4|bvec2|bvec3|bvec4|rt_Primitive|rt_Camera|rt_Material|rt_Texture|rt_Light

QUALIFIER_NAME_VARIABLE_QUALIFIERS attribute|uniform|varying|const
QUALIFIER_NAME_CLASS_MODIFIERS const|public|private|scratch
QUALIFIER_NAME attribute|uniform|varying|const|const|public|private|scratch

KEYWORD class|break|case|const|continue|default|do|double|else|enum|extern|for|goto|if|sizeof|static|struct|switch|typedef|union|unsigned|while|illuminance|ambient|dominantAxis|dot|hit|inside|inverse|luminance|max|min|normalize|perpendicularTo|pow|rand|reflect|sqrt|trace

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

"/*"		{/*yylog("COMMENT BLOCK BEGIN ===>\n");*/BEGIN(COMMENT);}
<COMMENT>"/*"
<COMMENT>\n	{num_lines++;}/*increased the line num*/
<COMMENT>"*"\n 		{num_lines++;}
<COMMENT>[^"*"]
<COMMENT>"*"[^"/"]	   

<COMMENT>"*/"		{/*yylog("COMMENT BLOCK END <===\n");*/ BEGIN(INITIAL);}


"//".*	{}


true|false	{yylog("BOOL %s\n", yytext);}


{PLUS}	yylog("PLUS\n"); 
{MUL}	yylog("MUL\n"); 
{MINUS}	yylog("MINUS\n"); 
{DIV}	yylog("DIV\n"); 
{ASSIGN}	yylog("ASSIGN\n"); 
{EQUAL}	yylog("EQUAL\n"); 
{NOT_EQUAL}	yylog("NOT_EQUAL\n"); 
{LT}	yylog("LT\n"); 
{LE}	yylog("LE\n"); 
{GT}	yylog("GT\n"); 
{GE}	yylog("GE\n"); 
{COMMA}	yylog("COMMA\n"); 
{COLON}	yylog("COLON\n"); 
{SEMICOLON}	yylog("SEMICOLON\n"); 
{LPARENTHESIS}	yylog("LPARENTHESIS\n"); 
{RPARENTHESIS}	yylog("RPARENTHESIS\n"); 
{LBRACKET}	yylog("LBRACKET\n"); 
{RBRACKET}	yylog("RBRACKET\n"); 
{LBRACE}	yylog("LBRACE\n"); 
{RBRACE}	yylog("RBRACE\n"); 
{AND}	yylog("AND\n"); 
{OR}	yylog("OR\n"); 
{INC}	yylog("INC\n"); 
{DEC}	yylog("DEC\n"); 



0[0-7]+(u|U)?	{ yylog( "INT %s\n", yytext ); }
0[xX][0-9A-Fa-f]+(u|U)?	{ yylog( "INT %s\n", yytext ); }
{DIGIT}+(u|U)?	{ yylog( "INT %s\n", yytext ); }
     


{DIGIT}+"."{DIGIT}*(lf|f|LF|F)?	{ yylog( "FLOAT %s\n", yytext); }
{DIGIT}*"."{DIGIT}+[eE]?("-"|"+")?{DIGIT}*(lf|f|LF|F)?	{ yylog( "FLOAT %s\n", yytext ); }
{DIGIT}+"."{DIGIT}*[eE]("-"|"+")?{DIGIT}*(lf|f|LF|F)?	{ yylog( "FLOAT %s\n", yytext ); }
{DIGIT}+[eE]("-"|"+")?{DIGIT}*(lf|f|LF|F)?	{ yylog( "FLOAT %s\n", yytext ); }
     

{TYPE_NAME}		{yylog("TYPE %s\n", yytext);}

{QUALIFIER_NAME}	{yylog("QUALIFIER %s\n", yytext);}

{KEYWORD}	{yylog("KEYWORD %s\n", yytext);}
     




"."{ID}	yylog( "SWIZZLE %s\n",++yytext );



     

rt_{ID}		yylog( "STATE %s\n", yytext );

{ID}	yylog( "IDENTIFIER %s\n", yytext );
     
     
[ \t]+	/* eat up whitespace */
     
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
    yylex();
    return 0;
}

int yyerror(char* msg)
{

	printf("yyError: %s, @ %d",msg,num_lines);
	return 0;
}
