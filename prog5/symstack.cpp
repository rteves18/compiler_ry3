//Partner Name: Ryan Teves Username: rteves
//Partner Name: Matthew Kim Username: madkim

#include "symstack.h"
#include <iostream>

void symstack::push_block(){
    ++next_block;
    stack.push_back(nullptr);
}

void symstack::pop_block(){
    stack.pop_back();
}

void symstack::define_ident(astree* node){
    if(stack.back() == nullptr)
        stack.back() = new symbol_table;
    insert_symbol(stack.back(), node);
}

symbol* symstack::search_ident(astree* node){
    for(auto type_sym : stack){
        if(type_sym == nullptr || type_sym->empty())
            continue;
        symbol* sym = search_symbol(type_sym, node);
        if(sym != nullptr)
            return sym;
    }
    return nullptr;
}

void symstack::dump(){
    int i = 0;
    for(auto type_sym : stack){
        cout<<"Table "<<i++<<endl;
        if(type_sym == nullptr)
            continue;
        ::dump(type_sym);
    }
}
