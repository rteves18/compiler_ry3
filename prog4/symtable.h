//Partner Name: Ryan Teves Username: rteves
//Partner Name: Matthew Kim Username: madkim

#include <vector>
#include <unordered_map>
#include <string>
#include <utility>
#include <bitset>
#include "astree.h"
struct symbol;

using symbol_table = unordered_map<string*,symbol*>;
using symbol_entry = pair<string*, symbol*>;


struct symbol{
   attr_bitset attr;
   symbol_table* fields;
   size_t filenr, linenr, offset;
   size_t blocknr;
   vector<symbol*>* parameters;
   size_t def_filenr, def_linenr, def_offset;
};

symbol* new_symbol(astree* node);
void insert_symbol(symbol_table* st,  astree* node);
symbol* search_symbol(symbol_table* st,  astree* node);
void dump(symbol_table* st);
