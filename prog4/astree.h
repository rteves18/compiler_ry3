//Partner Name: Ryan Teves Username: rteves
//Partner Name: Matthew Kim Username: madkim

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
#include <bitset>
#include <unordered_map>
#include "auxlib.h"

using namespace std;

struct symbol;
using symbol_table = unordered_map<string*,symbol*>;

enum { attr_void, attr_bool, attr_char, 
       attr_int, attr_null, attr_string, 
       attr_struct, attr_array, attr_function,
       attr_variable, attr_field, attr_typeid, 
       attr_param, attr_lval, attr_const, 
       attr_vreg, attr_vaddr, attr_bitset_size,
};

using attr_bitset = bitset<attr_bitset_size>;

struct astree {
   int symbol;               // token code
   size_t filenr;            // index into filename stack
   size_t linenr;            // line number from source code
   size_t offset;            // offset of token with current line
   const string* lexinfo;    // pointer to lexical information
   vector<astree*> children; // children of this n-way node
   size_t blocknr;
   attr_bitset attr;
   symbol_table* struct_table;
   size_t def_filenr, def_linenr, def_offset;
   astree (int symbol, int filenr, int linenr,
           int offset, const char* clexinfo);
};

// Append one child to the vector of children.
astree* adopt1 (astree* root, astree* child);

// Append two children to the vector of children.
astree* adopt2 (astree* root, astree* left, astree* right);

astree* adopt_sym (astree* root, astree* child, int sym);

astree* adopt_sym2 (astree* root, astree* child1, 
astree* child2, int sym);

astree* new_func(astree* left, astree* mid, astree* right);

astree* new_proto(astree* left, astree* right);

// Dump an astree to a FILE.
void dump_astree (FILE* outfile, astree* root);

// Debug print an astree.
void yyprint (FILE* outfile, unsigned short toknum,
              astree* yyvaluep);

// Recursively free an astree.
void free_ast (astree* tree);

// Recursively free two astrees.
void free_ast2 (astree* tree1, astree* tree2);
string enum_bitset(attr_bitset attr_array);

#endif

