//Partner Name: Ryan Teves Username: rteves
//Partner Name: Matthew Kim Username: madkim

#include "symtable.h"
#include <iostream>
#include "astree.h"

symbol* new_symbol(astree* node){
   symbol* sym = new symbol();
   sym->filenr = node->filenr;
   sym->linenr = node->linenr;
   sym->blocknr = node->blocknr;
   sym->attr = node->attr;
   sym->parameters = nullptr;
   return sym;
}

void insert_symbol(symbol_table* st,  astree* node){
	symbol* sym = new_symbol(node);
	symbol_entry entry = 
	symbol_entry(const_cast<string*>(node->lexinfo), sym);
	st->insert(entry);
}

symbol* search_symbol(symbol_table* st,  astree* node){
	string* lexinfo = const_cast<string*>(node->lexinfo);
	if(!st->count(lexinfo)) return nullptr;
	symbol_entry entry = 
	*st->find(const_cast<string*>(node->lexinfo));
	node->def_linenr = entry.second->linenr;
	node->def_filenr = entry.second->filenr;
	node->def_offset = entry.second->offset;
	return entry.second;
}

void dump(symbol_table* st){
	cout<<"\tTable Size: "<<st->size()<<endl;
	int i = 0;
	for(auto iter = st->begin(); iter != st->end(); iter++){
		cout<<"\tEntry "<<i++<<" "<<*(*iter).first<<endl;
	}
}
