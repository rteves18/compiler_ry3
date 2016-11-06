%{
//Partner Name: Ryan Teves Username: rteves
//Partner Name: Matthew Kim Username: madkim

#include <assert.h>
#include "lyutils.h"
#include "astree.h"

%}

%debug
%defines
%error-verbose
%token-table
%verbose

%token TOK_VOID TOK_BOOL TOK_CHAR TOK_INT TOK_STRING
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_FALSE TOK_TRUE TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD
%token TOK_VARDECL TOK_DECLID TOK_INDEX TOK_NEWSTRING
%token TOK_ORD TOK_CHR TOK_ROOT TOK_RETURNVOID
%token TOK_FUNCTION TOK_PARAMLIST TOK_PROTOTYPE

%right  TOK_IF TOK_ELSE TOK_IFELSE
%right  '='
%left   TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left   '+' '-'
%left   '*' '/' '%'
%right  TOK_POS TOK_NEG '!' TOK_ORD TOK_CHR
%left TOK_NEWARRAY '[' '.' TOK_FIELD TOK_FUNCTION
%nonassoc TOK_NEW
%nonassoc TOK_PAREN ')'

%start start

%%
start     : program                 { yyparse_astree = $1;  }
           
program   : program structdef       { $$ = adopt1 ($1, $2); }
          | program function        { $$ = adopt1 ($1, $2); }
          | program statement       { $$ = adopt1 ($1, $2); }
          | program error '}'       { $$ = $1; }
          | program error ';'       { $$ = $1; }
          |                         { $$ = new_parseroot ();}
          ;   

structdef : TOK_STRUCT TOK_IDENT '{' '}' 
                       { free_ast2($3, $4);
                         $$ = adopt_sym($1, $2, TOK_TYPEID); }
          | TOK_STRUCT TOK_IDENT '{' fielddeclar '}' 
                       { free_ast2($3, $5);
                         $$ = adopt_sym2($1, $2, $4, TOK_TYPEID);  }

fielddeclar : fielddeclar fielddecl ';'   { free_ast($3);
                                            $$ = adopt1($1, $2);   }
            | fielddecl ';'               { free_ast($2); $$ = $1; }
  
fielddecl : basetype TOK_ARRAY TOK_IDENT  
                       { $$ = adopt_sym2($2, $3, $1, TOK_FIELD); }
          | basetype TOK_IDENT            
                       { $$ = adopt_sym ($1, $2, TOK_FIELD); }
          ;

basetype : TOK_VOID                 { $$ = $1; }
         | TOK_BOOL                 { $$ = $1; }
         | TOK_CHAR                 { $$ = $1; }
         | TOK_INT                  { $$ = $1; }
         | TOK_STRING               { $$ = $1; }
         | TOK_TYPEID               { $$ = $1; }
         | TOK_IDENT                { $$ = $1; }
         ;     
   
function : identdecl parameter ')' block       
                { free_ast($3); $$ = new_func($1, $2, $4); }
         ;   
   
parameter : parameter ',' identdecl { $$ = adopt1($1, $3); }
          | '(' identdecl           { $$ = adopt_sym($1, $2, 
                                      TOK_PARAMLIST); }
          | '('                     {$1->symbol = TOK_PARAMLIST; }
          ;  
    
   
identdecl : basetype TOK_ARRAY TOK_IDENT 
                    { $$ = adopt_sym2 ( $2, $3, $1, TOK_DECLID); }
          | basetype TOK_IDENT     
                    { $$ = adopt_sym( $1, $2, TOK_DECLID); }
          ;  
   
     
statement : block                   { $$ = $1; }             
          | vardecl                 { $$ = $1; }          
          | while                   { $$ = $1; }         
          | ifelse                  { $$ = $1; }         
          | return                  { $$ = $1; }         
          | expr ';'                { free_ast($2);
                                      $$ = $1; }
          ;
               
     
block     : blockhead '}'           { free_ast($2);
                                      $1->symbol = TOK_BLOCK; }
          | '{' '}'                 { free_ast($2);
                                      $1->symbol = TOK_BLOCK; }
          | ';'                     { $1->symbol = TOK_BLOCK; }
          ;  
   
blockhead : blockhead statement     { $$ = adopt1 ($1, $2); }
          | '{' statement           { $$ = adopt1 ($1, $2); }
     
     
vardecl   : identdecl '=' expr ';'  {  free_ast($4);
                                       $2->symbol = TOK_VARDECL; 
                                       $$ = adopt2 ($2, $1, $3); }
          ;  
     
while     : TOK_WHILE '(' expr ')' statement     
                                    { free_ast2($2, $4);
                                      $$ = adopt2 ($1, $3, $5); }
          ;
  
ifelse    : TOK_IF '(' expr ')' statement TOK_ELSE 
                                          statement   
                                    { free_ast2($2, $4); free_ast($6);
                                      $1->symbol = TOK_IFELSE;  }  
                                    { $$ = adopt2 ($1, $3, $5); } 
                                    { $$ = adopt1 ($1, $7);     }
          | TOK_IF '(' expr ')' statement %prec TOK_IF
                                    { free_ast2($2, $4);
                                      $$ = adopt2 ($1, $3, $5); }    
          ;
  
return    : TOK_RETURN expr ';'     { $$ = adopt1 ($1, $2); }
          | TOK_RETURN ';'          { $$ = adopt_sym ($1, $2,  
                                      TOK_RETURNVOID); } 
          ;    
       
expr      : binop                   { $$ = $1; }
          | unop                    { $$ = $1; }
          | allocator               { $$ = $1; }
          | call                    { $$ = $1; } 
          | '(' expr ')'            { free_ast2($1, $3);
                                      $$ = $2; }
          | variable                { $$ = $1; }
          | constant                { $$ = $1; }
          ;          
           
binop     : expr '=' expr           { $$ = adopt2 ($2, $1, $3); }
          | expr '+' expr           { $$ = adopt2 ($2, $1, $3); }
          | expr '-' expr           { $$ = adopt2 ($2, $1, $3); }
          | expr '*' expr           { $$ = adopt2 ($2, $1, $3); }
          | expr '/' expr           { $$ = adopt2 ($2, $1, $3); }
          | expr '%' expr           { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_EQ expr        { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_NE expr        { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_LT expr        { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_LE expr        { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_GT expr        { $$ = adopt2 ($2, $1, $3); }
          | expr TOK_GE expr        { $$ = adopt2 ($2, $1, $3); }
          ;        
           
unop      : '!' expr                { $$ = adopt1 ($1, $2); }
          | '+' expr %prec TOK_POS  { $1->symbol = TOK_POS;
                                      $$ = adopt1 ($1, $2); }
          | '-' expr %prec TOK_NEG  { $1->symbol = TOK_NEG;
                                      $$ = adopt1 ($1, $2); }
          | TOK_ORD expr            { $$ = adopt1 ($1, $2); }  
          | TOK_CHR expr            { $$ = adopt1 ($1, $2); }
          ;        
         
allocator : TOK_NEW TOK_IDENT '(' ')'  
                     { $$ = adopt_sym ($1, $2, TOK_TYPEID); }
          | TOK_NEW basetype '[' expr ']'            
                     { free_ast2($3, $5);
                       $$ = adopt_sym2 ($1, $2, $4, TOK_NEWARRAY); }
          | TOK_NEW TOK_STRING '(' expr ')'          
                     { free_ast2($3, $5);
                       $$ = adopt_sym2 ($1, $2, $4, TOK_NEWSTRING); }
          ;      
         
call      : TOK_IDENT '(' expr ')'  { $2->symbol = TOK_CALL; 
                                      $$ = adopt2($2, $1, $3); }
          | TOK_IDENT '(' ')'       { free_ast($3);
                                      $2->symbol = TOK_CALL;
                                      $$ = adopt1($2, $1);     }
          ;        
         
variable  : TOK_IDENT               { $$ = $1; }
          | expr '[' expr ']'       { $2->symbol = TOK_INDEX;
                                      $$ = adopt2($2, $1, $3); }
          | expr '.' TOK_IDENT      
                    { $$ = adopt_sym2( $2, $3, $1, TOK_FIELD); }
          ;      
                   
         
constant  : TOK_INTCON               { $$ = $1; }
          | TOK_CHARCON              { $$ = $1; }
          | TOK_STRINGCON            { $$ = $1; }      
          | TOK_FALSE                { $$ = $1; } 
          | TOK_TRUE                 { $$ = $1; }
          | TOK_NULL                 { $$ = $1; }
          ;
%%


const char *get_yytname (int symbol) {
   return yytname [YYTRANSLATE (symbol)];
}


bool is_defined_token (int symbol) {
   return YYTRANSLATE (symbol) > YYUNDEFTOK;
}

/*
static void* yycalloc (size_t size) {
   void* result = calloc (1, size);
   assert (result != nullptr);
   return result;
}
*/

