#include "typecheck.h"
#include <iostream>
#define UNUSED(x) (void)(x)
using namespace std;
astree* last_struct = nullptr;

bool compare_primitive(astree* left, astree* right){
   for(size_t i = 0; i < attr_function; ++i){
      if(left->attr[i]==1 && right->attr[i]==1)
         return true;
   }
   return false;
}

bool compare_reference(astree* left, astree* right){
   if(left->struct_table == nullptr || 
      right->struct_table == nullptr)
      return false;
   if(left->children[0]->lexinfo == right->children[0]->lexinfo)
      return true;
   if(right->attr[attr_null])
      return true;
   return false;
}

//update type
void update_type(astree* parent, astree* child){
   for(size_t i=0; i < attr_function; ++i){
      if(child->attr[i])
         parent->attr[i] = 1;   
   }
}

//update attribute
void update_attribute(astree* parent, astree* child){
   for(size_t i=0; i < attr_bitset_size; ++i){
      if(child->attr[i])
         parent->attr[i] = 1;   
   }
}

void attribute_check(astree* node){
         if(node->children[0]->attr[attr_array] == 1) {
            node->children[0]->children[1]->attr[attr_array]=1;
            node->children[0]->children[1]->attr[attr_function]=1;
         
         if(node->children[0]->children[0]->symbol == TOK_NE)
            node->children[0]->children[1]->attr[attr_string] =1;
         
         if(node->children[0]->children[0]->symbol == TOK_ARRAY)
            node->children[0]->children[1]->attr[attr_char] =1;
         
         if(node->children[0]->children[0]->symbol == TOK_EQ)
            node->children[0]->children[1]->attr[attr_int] =1;
         
         if(node->children[0]->children[0]->symbol == TOK_GT)
            node->children[0]->children[1]->attr[attr_struct] =1;
         }
}

//print symbol table
void print_sym(FILE* outfile, astree* node){
      if((node->blocknr == 0 && !node->attr[attr_field]) 
         || node->attr[attr_struct])
         fprintf(outfile, "\n");
      else
         fprintf(outfile, "   ");
      if(!node->attr[attr_field]){
      fprintf(outfile, 
         "%s (%zu.%zu.%zu) {%zu} %s",
         (node->lexinfo)->c_str(), 
         node->filenr,
         node->linenr,
         node->offset,
         node->blocknr,
         enum_bitset(node->attr).c_str());
      }
      else{
      fprintf(outfile, 
         "%s (%zu.%zu.%zu) {%s} %s",
         (node->lexinfo)->c_str(), 
         node->filenr,
         node->linenr,
         node->offset,
         last_struct->lexinfo->c_str(),
         enum_bitset(node->attr).c_str()
      );
      }
      if(node->attr[attr_struct]){
         fprintf(outfile, "\"%s\"", node->lexinfo->c_str());
         last_struct = node;
      }
   fprintf(outfile, "\n");
}

void block_recurse(astree* node, symstack* s){
   if(node->symbol == TOK_BLOCK)
      s->push_block();
   node->blocknr = next_block;
   for(auto child : node->children){
      block_recurse(child, s);
   }
}

void type_check_body(FILE* outfile, astree* node, symstack* s, 
   symbol_table *type_table, size_t depth){
   UNUSED(depth);
   astree* left_child = nullptr;
   astree* right_child = nullptr;
   symbol *sym;
   if(node->children.size() >= 1)
      left_child = node->children[0];
   if(node->children.size() >= 2)
      right_child = node->children[1];
   switch(node->symbol){
      case TOK_ROOT:
      case TOK_DECLID:
      case TOK_PARAMLIST:
      case TOK_RETURN:
      case TOK_RETURNVOID:
         break;
      case TOK_FIELD:
      node->attr[attr_field] = 1;
         if(left_child != nullptr){
            left_child->attr[attr_field] = 1;
            update_type(node, left_child);
          }
         break;
      case TOK_NEWARRAY:
      node->attr[attr_vreg] = 1;
         node->attr[attr_array] = 1;
         update_type(node, left_child);
         break;
      case TOK_NEW:
      update_attribute(node, left_child); 
         break;
      case TOK_CALL:
      {
            sym = search_symbol(s->stack[0],  node->children.back());
            if(sym == nullptr){
               if (!left_child->attr[attr_string]){
               errprintf("Error at  (%d %d %d):"
                  "No matching function %s\n",
                  node->filenr, node->linenr, node->offset, 
                  node->children.back()->lexinfo->c_str());
                  }
               break;
            }
            for(size_t i=0; i < attr_function; ++i){
               if(sym->attr[i])
                  node->attr[i] = 1;   
            }
            break;
         }
      case TOK_FUNCTION:
         s->push_block();
         attribute_check(node);
         node->children[0]->children[0]->attr[attr_function]=1;
         insert_symbol(s->stack[0], node->children[0]->children[0]);
         print_sym(outfile, node->children[0]->children[0]);
         for(auto param : node->children[1]->children){
           param->children[0]->attr[attr_variable] = 1;
           param->children[0]->attr[attr_lval] = 1;
           param->children[0]->attr[attr_param] = 1;
           param->children[0]->blocknr = next_block;
           s->define_ident(param->children[0]);
           print_sym(outfile, param->children[0]); 
         }
         block_recurse(node->children[2], s);
         s->pop_block();
         print_sym(outfile, node);
         break;
      case TOK_PROTOTYPE:
      attribute_check(node);
      node->children[0]->children[0]->attr[attr_function] = 1;
      insert_symbol(s->stack[0], node->children[0]->children[0]);
      print_sym(outfile, node->children[0]->children[0]);
      s->push_block();
      for(auto param : node->children[1]->children){
         param->children[0]->attr[attr_variable] = 1;
         param->children[0]->attr[attr_lval] = 1;
         param->children[0]->attr[attr_param] = 1;
         param->children[0]->blocknr = next_block;
         s->define_ident(param);
         print_sym(outfile, param->children[0]);
         }
         s->pop_block();
          break;
      case TOK_BLOCK:
         block_recurse(node, s);
         s->pop_block();
         break;
      case TOK_VOID:
         left_child->attr[attr_void] = 1;
         break;
      case TOK_BOOL:
         if(left_child == nullptr)
         break;
         left_child->attr[attr_bool] = 1;
         update_type(node, left_child);
         break;
      case TOK_CHAR:
         if(left_child == nullptr)
         break;
         left_child->attr[attr_char] = 1;
         left_child->attr[attr_lval] = 1;
         update_type(node, left_child);
         break;
      case TOK_STRING:
         if(left_child == nullptr)
         break;
         left_child->attr[attr_string] = 1;
         update_type(node, left_child);
         break;
      case TOK_ARRAY:
         node->attr[attr_array];
         if(node == nullptr || node->children.empty())
         break;
         node->children[0]->attr[attr_array] = 1;
         break;
      case TOK_INDEX:
         node->attr[attr_lval] = 1;
         node->attr[attr_vaddr] = 1;
         break;
      case TOK_VARDECL:
         left_child->children[0]->attr[attr_lval] = 1;
         left_child->children[0]->attr[attr_variable] = 1;
         update_attribute(node, left_child);
         if(s->search_ident(left_child->children[0]))
            errprintf("Error at (%d %d %d): Mulitple declarations %s\n",
               node->filenr, node->linenr, node->offset, 
               left_child->children[0]->lexinfo->c_str());
         s->define_ident(left_child->children[0]);
         print_sym(outfile, left_child->children[0]);
         break;
      case TOK_IDENT:
         sym = s->search_ident(node);
         if(sym == nullptr)
            sym = search_symbol(type_table, node);
         if(sym == nullptr){
            errprintf("Error at (%d %d %d): Reference undefined %s\n",
               node->filenr, node->linenr, node->offset, 
               node->lexinfo->c_str());
         break;
         }
         node->attr = sym->attr;
         break;
      case TOK_STRUCT:
         {
         left_child->attr[attr_struct] = 1;
         insert_symbol(type_table,  left_child);            
         print_sym(outfile, left_child);
         symbol* sym = search_symbol(type_table,  left_child);
         sym->fields = new symbol_table;
         for(auto child = node->children.begin()+1;
            child != node->children.end(); ++child){
            insert_symbol(sym->fields,  *child);
            print_sym(outfile, (*child)->children[0]);
            }
         break;
         }
       case TOK_TYPEID:
         node->attr[attr_typeid] = 1;
         break;
      case TOK_ORD:
         node->attr[attr_vreg] = 1;
         node->attr[attr_char] = 1;
         break;
      case TOK_TRUE:
         node->attr[attr_bool] = 1;
         node->attr[attr_const] = 1;
         break;
      case TOK_FALSE:
      case TOK_NULL:
         node->attr[attr_null] = 1;
         node->attr[attr_const] = 1;
         break;
      case TOK_WHILE:
      case TOK_INT:
         if(left_child == nullptr)
         break;
         left_child->attr[attr_int] = 1;
         update_type(node, left_child);
         break;
      case TOK_INTCON:
      case TOK_CHARCON:
      case TOK_STRINGCON:
         node->attr[attr_string] = 1;
         node->attr[attr_const] = 1;
         break;
      case TOK_ELSE:
      case TOK_IFELSE:
         if(!left_child->attr[attr_bool])
            errprintf("Error at (%d %d %d): "
               "Improper If Else usage\n", 
               node->filenr, node->linenr, node->offset);
         break;
      case TOK_LT: case TOK_LE: case TOK_GT: case TOK_GE:
      {
            left_child = node->children[0];
            right_child = node->children[1];
            if(compare_primitive(left_child, right_child)){
               node->attr.set(attr_bool);
               return;
            }
            break;
         }
      case TOK_NE: case TOK_EQ:
      {
            if(compare_primitive(left_child, right_child) || 
               compare_reference(left_child, right_child)){
               update_type(node, left_child);
               node->attr[attr_vreg] = 1;
               return;
            }
            break;
         }
      case '.': 
      case '(': case ')': case '}': case ']': case ';': case '=':
      {
            if(left_child == nullptr)
               break;
            if(left_child->attr[attr_lval] && 
               right_child->attr[attr_vreg]){
               update_type(node, left_child); 
               node->attr[attr_vreg] = 1;
               return;
            }
            if (!left_child->attr[attr_void]){
             errprintf("Error %d %d %d: Invalid types\n", 
                node->filenr, node->linenr, node->offset);
               }
            break;
               }
      case '+': case '-':
      {
            node->attr[attr_vreg] = 1;
            node->attr[attr_int] = 1;
            if(right_child == nullptr){
               if(left_child == nullptr)
                  break;
               if(!left_child->attr[attr_int]){
                  errprintf("Error %d %d %d: "
                     "Non int arithmetic operator\n", 
                     node->filenr, node->linenr, node->offset);
               }
            }
            else{
               if(!left_child->attr[attr_int] || 
                  !right_child->attr[attr_int]){
                  errprintf("Error %d %d %d: "
                     "Non int arithmetic operator\n", 
                     node->filenr, node->linenr, node->offset);
               }
            }
               break;
         }
      case '/': case '*': case '%':
      {
            node->attr[attr_vreg] = 1;
            node->attr[attr_int] = 1;
            if(!left_child->attr[attr_int] || 
               !right_child->attr[attr_int]){
               errprintf("Error %d %d %d: "
                  "Non int with arithmetic operator\n", 
                  node->filenr, node->linenr, node->offset);
            }
            break;
         }
      case '!':
      node->attr[attr_bool] = 1;
         node->attr[attr_vreg] = 1;
         if(!(left_child->attr[attr_bool])){
            errprintf("Error %d %d %d: "
               "Non bool with bool operator\n",
               node->filenr, node->linenr, node->offset);
         }
         break;

   default:
         errprintf("Invalid symbol %s\n", 
            get_yytname(node->symbol));
   }  
   if(node->attr[attr_lval])
      node->attr[attr_variable] = 1;
}


void type_check_recursive(FILE* outfile, astree* root, symstack* s, 
   symbol_table *type_table, size_t depth){
   for(auto child : root->children){
      type_check_recursive(outfile, child, s, type_table, depth+1);
   }
   type_check_body(outfile, root, s, type_table, depth);
}

void type_check(FILE* outfile, astree* root, symstack* s, 
   symbol_table *type_table){
   type_check_recursive(outfile, root, s, type_table, 0);
   while(!s->stack.empty())
      s->stack.pop_back();
}