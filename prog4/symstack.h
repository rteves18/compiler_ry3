//Partner Name: Ryan Teves Username: rteves
//Partner Name: Matthew Kim Username: madkim

#ifndef __symstack_h
#define __symstack_h
#include "symtable.h"
extern size_t next_block;

class symstack
{
public:
	vector<size_t> block_stack;
	vector<symbol_table*> stack;

	void push_block();
	void pop_block();
	void define_ident(astree* node);
	symbol* search_ident(astree* node);
	void dump();
	};

#endif
