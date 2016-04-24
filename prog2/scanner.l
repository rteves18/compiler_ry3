

%{
#include "lyutils.h"
#include "auxlib.h"
#define YY_USER_ACTION  { lexer_useraction(); }

%}

%option 8bit
%option debug
%option nodefault
%option nounput
%option noyywrap
%option warn

LETTER          [A-Za-z_]
DIGIT           [0-9]
MANTISSA        ({DIGIT}+\.?{DIGIT}*|\.{DIGIT}+)
EXPONENT        ([Ee][+-]?{DIGIT}+)
NUMBER          {DIGIT}({DIGIT})*
BADNUMBER       ({MANTISSA}[Ee][+-]?)
IDENT           ({LETTER}({LETTER}|{DIGIT})*)
BADINDENT       ({DIGIT}({LETTER}|{DIGIT})*)
STRING          (\"([^\\"\n]|\\[\\'"0nt])*\")
BADSTRING       (\"([^\\"\n]|\\[\\'"0nt])*\n)
CHAR            ('([^\\'\n]|\\[\\'"0nt])*\")
BADCHAR         ('([^\\'\n]|\\[\\'"0nt])*\n)

%%

"#".*           { lexer_include(); }
[ \t]+          { }
\n              { lexer_newline(); }
"void"          { return yylval_token (TOK_VOID); }
"bool"          { return yylval_token (TOK_BOOL); }
"char"          { return yylval_token (TOK_CHAR); }
"int"           { return yylval_token (TOK_INT); }
"string"        { return yylval_token (TOK_STRING); }
"struct"        { return yylval_token (TOK_STRUCT); }
"if"            { return yylval_token (TOK_IF); }
"else"          { return yylval_token (TOK_ELSE); }
"while"         { return yylval_token (TOK_WHILE); }
"return"        { return yylval_token (TOK_RETURN); }
"false"         { return yylval_token (TOK_FALSE); }
"true"          { return yylval_token (TOK_TRUE); }
"null"          { return yylval_token (TOK_NULL); }
"ord"           { return yylval_token (TOK_ORD); }
"chr"           { return yylval_token (TOK_CHR); }
"new"           { return yylval_token (TOK_NEW); }

{NUMBER}        { return yylval_token (TOK_INTCON); }
{IDENT}         { return yylval_token (TOK_IDENT); }
"[]"            { return yylval_token (TOK_ARRAY); }
"["             { return yylval_token ('['); }
"]"             { return yylval_token (']'); }
"{"             { return yylval_token ('{'); }
"}"             { return yylval_token ('}'); }
","             { return yylval_token (','); }
"."             { return yylval_token ('.'); }
"=="            { return yylval_token (TOK_EQ); }
"!="            { return yylval_token (TOK_NE); }
"<"             { return yylval_token (TOK_LT); }
"<="            { return yylval_token (TOK_LE); }
">"             { return yylval_token (TOK_GT); }
">="            { return yylval_token (TOK_GE); }
"%"             { return yylval_token ('%'); }
"!"             { return yylval_token ('!'); }
"="             { return yylval_token ('='); }
"+"             { return yylval_token ('+'); }
"-"             { return yylval_token ('-'); }
"*"             { return yylval_token ('*'); }
"/"             { return yylval_token ('/'); }
"^"             { return yylval_token ('^'); }
"("             { return yylval_token ('('); }
")"             { return yylval_token (')'); }
";"             { return yylval_token (';'); }

{BADINDENT}     { lexer_badtoken (yytext); return yylval_token (TOK_IDENT); }
{BADNUMBER}     { lexer_badtoken (yytext); return yylval_token (TOK_INTCON); }
{STRING}        { return yylval_token (TOK_STRINGCON); }
{BADSTRING}     { lexer_badtoken (yytext); return yylval_token (TOK_STRINGCON); }
{CHAR}          { return yylval_token (TOK_CHARCON); }
{BADCHAR}       { lexer_badtoken (yytext); return yylval_token (TOK_CHARCON); }

.               { lexer_badchar (*yytext); }

%%
