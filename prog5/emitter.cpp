// $Id: emitter.cpp,v 1.2 2015-07-09 18:25:23-07 - - $

#include <cstring>
#include <cstdlib>

#include <assert.h>
#include <stdio.h>

#include "astree.h"
#include "emitter.h"
#include "auxlib.h"
#include "lyutils.h"
#include "symtable.h"

string vreg;
string ident;
size_t int_counter = 0;
size_t ptr_counter = 0;
size_t inptr_counter = 0;
size_t char_counter = 0;
size_t string_counter = 0;
symbol_table* symtable = new symbol_table;

void emitter_node(FILE* file, astree* node);
void emitter_expr(FILE* file, astree* node);
void emitter_return(FILE*, astree* node);
void emitter_new_vreg(FILE* file, astree* node);

string mangle_struct(astree* node){
	string linebuild;
	linebuild += "struct s_" + *node->children[0]->lexinfo + "{" + "\n";
	for(auto field = node->children.cbegin()+1;
		field != node->children.cend(); field++){
		linebuild += "        ";
		linebuild += *(*field)->lexinfo;
		linebuild += "_f";
		linebuild += *(*field)->children[0]->lexinfo;
		linebuild += "_";
		linebuild += *(*field)->children[0]->lexinfo;
		linebuild += ";\n";
	}
	linebuild += "};\n";
	return linebuild;
}

string mangle_ident(astree* node){
	string linebuild = *node->lexinfo + " ";
	astree* child = nullptr;
	if (!node->children.empty())
		child = node->children[0];
	else 
		child = node;
	if(node->blocknr == 0){
		linebuild += "__";
		linebuild += *child->lexinfo;
	}
	else{
		linebuild += "_";
		linebuild += child->blocknr;
		linebuild += "_";
		linebuild += *child->lexinfo;
	}
	linebuild += ";\n";
	return linebuild;
}

string mangle_param(astree* node){
	string linebuild = *node->lexinfo + " ";
	astree* child = node->children[0];
	linebuild += "_";
	linebuild += to_string(child->blocknr);
	linebuild += "_";
	linebuild += *child->lexinfo;
	return linebuild;
}


void emitter_binop(FILE* file, astree* node){
	symbol* symbol =  search_symbol(symtable, node);
	if(symbol != nullptr){
		vreg = symbol->vreg;
		fprintf(file, "%s %s %s",
			node->children[0]->vreg.c_str(),
			node->lexinfo->c_str(),
			node->children[1]->vreg.c_str());
	}
	else{
		fprintf(file, "%s %s %s",
			node->children[0]->vreg.c_str(),
			node->lexinfo->c_str(),
			node->children[1]->vreg.c_str());
	}
}

void emitter_ident(FILE* file, astree* node){
	printf("Emit ident: %s\n", node->lexinfo->c_str());
	ident = mangle_ident(node);
	fprintf(file, ident.c_str());
	node->vreg = ident;

}

string new_vreg(char type){
	string linebuild;
	switch(type){
		case 'i':
			linebuild += type;
			linebuild += to_string(++int_counter);
			break;
		case 'p':
			linebuild += type;
			linebuild += to_string(++ptr_counter);
			break;
		case 'a':
			linebuild += type;
			linebuild += to_string(++inptr_counter);
			break;
		case 'c':
			linebuild += type;
			linebuild += to_string(++char_counter);
			break;
		case 's':
			linebuild += type;
			linebuild += to_string(++string_counter);
			break;
		default:
		errprintf("Invalid register type: %c \n", type);
	}
	return linebuild;
}

void emitter_new_vreg(FILE* file, astree* node){
	char* type_buf;
	if(node->attr[attr_bool] || node->attr[attr_char]){
		node->vreg = new_vreg('c');
		type_buf = strdup("char");
	}
	else if(node->attr[attr_int]){
		node->vreg = new_vreg('i');
		type_buf = strdup("int");
	}
	else if(node->attr[attr_struct]){
		node->vreg = new_vreg('p');
		type_buf = (char*) node->children[0]->lexinfo->c_str();
	}
	else{
		node->vreg = new_vreg('a');
		type_buf = strdup("unknown");
	}
	fprintf(file, "%s %s", type_buf, node->vreg.c_str());
	insert_symbol(symtable, node);

}

void emitter_expr(FILE* file, astree* node){
	if(node->attr[attr_vreg]){
		emitter_binop(file, node);
	}
	else{
		switch(node->symbol){
			case TOK_IDENT:
				emitter_ident(file, node);
				break;
			case TOK_INTCON:
			case TOK_CHARCON:
				emitter_new_vreg(file, node);
				break;
		}
	}
	fprintf(file, "\n");
}

void emitter_while(FILE* file, astree* node){
	fprintf(file, "while_%zu_%zu_%zu:;\n",
		node->filenr,
		node->linenr,
		node->offset);
	switch(node->children[0]->symbol){
		case TOK_IDENT:
		case TOK_INTCON:
		case TOK_CHARCON:
			break;
		default:
			fprintf(file, "        ");
			emitter_expr(file, node->children[0]);
	}
	fprintf(file, "        ");
	fprintf(file, "if (!%s) goto break_%zu_%zu_%zu\n",
		node->children[0]->vreg.c_str(),
		node->children[1]->filenr,
		node->children[1]->linenr,
		node->children[1]->offset);
	emitter_node(file, node->children[1]);
	fprintf(file, "        ");
	fprintf(file, "goto while_%zu_%zu_%zu\n",
		node->filenr,
		node->linenr,
		node->offset);
	fprintf(file, "break_%zu_%zu_%zu):;\n",
		node->children[1]->filenr,
		node->children[1]->linenr,
		node->children[1]->offset);
}

void emitter_ifelse(FILE* file, astree* node){
	switch(node->children[0]->symbol){
		case TOK_IDENT:
		case TOK_INTCON:
		case TOK_CHARCON:
			break;
		default:
			fprintf(file, "        ");
			emitter_expr(file, node->children[0]);
	}
	fprintf(file, "        ");
	fprintf(file, "if (!%s) goto else_%zu_%zu_%zu;\n",
		node->children[0]->vreg.c_str(),
		node->children[1]->filenr,
		node->children[1]->linenr,
		node->children[1]->offset);
	//find simpler way to implement this
	for(auto child : node->children[0]->children){
		fprintf(file, "        ");
		emitter_node(file, child);
	}
	fprintf(file, "goto fi_%zu_%zu_%zu:;\n",
		node->children[0]->filenr,
		node->children[0]->linenr,
		node->children[0]->offset);
	fprintf(file, "else_%zu_%zu_%zu:;\n",
		node->children[0]->filenr,
		node->children[0]->linenr,
		node->children[0]->offset);
	for(auto child : node->children[1]->children){
		fprintf(file, "        ");
		emitter_node(file, child);
	}
	fprintf(file, "fi_%zu_%zu_%zu:;\n",
		node->children[0]->filenr,
		node->children[0]->linenr,
		node->children[0]->offset);
}

void emitter_if(FILE* file, astree* node){
	switch(node->children[0]->symbol){
		case TOK_IDENT:
		case TOK_INTCON:
		case TOK_CHARCON:
			break;
		default:
			fprintf(file, "        ");
			emitter_expr(file, node->children[0]);
	}
	fprintf(file, "        ");
	fprintf(file, "if (!%s) goto fi_%zu_%zu_%zu;\n",
		node->children[0]->vreg.c_str(),
		node->children[0]->filenr,
		node->children[0]->linenr,
		node->children[0]->offset);
	for(auto child : node->children[0]->children){
		fprintf(file, "        ");
		emitter_node(file, child);
	}
	fprintf(file, "fi_%zu_%zu_%zu:;\n",
		node->children[0]->filenr,
		node->children[0]->linenr,
		node->children[0]->offset);
}

void emitter_return(FILE* file, astree* node){
	if(node->children.empty()){
		fprintf(file, "       ");
		fprintf(file, "return;\n");
	}
	else{
		fprintf(file, "return %s;\n", 
			node->children[0]->vreg.c_str());
	}
}

void emitter_rec(FILE* file, astree* node){
	for(auto child : node->children)
		emitter_rec(file, child);
	emitter_node(file, node);
}

void emitter_func(FILE* file, astree* node){
	string linebuild = "__" + *node->children[0]->lexinfo + " ";
	linebuild += *node->children[0]->children[0]->lexinfo + " (\n";
	for(auto param : node->children[1]->children){
		linebuild += "        " + mangle_param(param) + ",\n";
	}
	linebuild.pop_back();
	linebuild.pop_back();
	linebuild += ")\n{\n";
	fprintf(file, linebuild.c_str());
	for(auto function_block : node->children[2]->children){
		emitter_rec(file, function_block);
	}
	fprintf(file, "}\n");
}

void emitter_call(FILE* file, astree* node){
	if(!node->attr[attr_void]){
		emitter_new_vreg(file, node);
	}
	fprintf(file, "%s ( ", node->children.back()->lexinfo->c_str());
	for(auto child = node->children.begin();
		child != node->children.end()-1; child++){
		fprintf(file, "%s", (*child)->vreg.c_str());
		if(child != node->children.end()-2)
			fprintf(file, ", ");
		else
			fprintf(file, " )\n");
	}
}

void emitter_assign(FILE* file, astree* node){
	symbol* symbol = search_symbol(symtable, node);
	char* type_buf = nullptr;
	fprintf(file, "        ");
	if(node->attr[attr_bool] || node->attr[attr_char]){
		node->vreg = new_vreg('c');
		type_buf = strdup("char");
	}
	else if(node->attr[attr_int]){
		node->vreg = new_vreg('i');
		type_buf = strdup("int");
	}
	else if(node->attr[attr_struct]){
		node->vreg = new_vreg('p');
		type_buf = (char*) node->children[0]->lexinfo->c_str();
	}
	if(symbol == nullptr){
		fprintf(file, "%s %s %s %s;\n",
			type_buf, node->children[0]->vreg.c_str(),
			node->lexinfo->c_str(), node->vreg.c_str());
	}
	else{
		fprintf(file, "%s %s %s;\n",
			node->children[0]->vreg.c_str(),
			node->lexinfo->c_str(),
			node->vreg.c_str());
	}
}

void emitter_node(FILE* file, astree* node){
	switch(node->symbol){
		case '+':
		case '-':
		case '*':
		case '%':
		case TOK_EQ:
		case TOK_NE:
		case TOK_LT:
		case TOK_GT:
		case TOK_LE:
		case TOK_GE:
		case TOK_POS:
		case TOK_NEG:
		case TOK_IDENT:
		case TOK_INTCON:
		case TOK_CHARCON:
			emitter_expr(file, node);
			break;
		case TOK_WHILE:
			emitter_while(file, node);
			break;
		case TOK_IFELSE:
			emitter_ifelse(file, node);
			break;
		case TOK_IF:
			emitter_if(file, node);
			break;
		case TOK_VARDECL:
			emitter_assign(file, node);
			break;
		case TOK_CALL:
			emitter_call(file, node);
			break;
		case TOK_RETURN:
			emitter_return(file, node);
			break;
	}
}

void emitter_stringcon(FILE* file, astree* node){
	vreg = new_vreg('s');
	node->vreg = vreg;
	fprintf(file, "char* %s = %s;\n",
		vreg.c_str(), node->lexinfo->c_str());
	insert_symbol(symtable, node);
}

void emit_code(FILE* file, astree* root){
	for(auto child : root->children){
		if(child->symbol == TOK_STRUCT){
			fprintf(file, mangle_struct(child).c_str());
		}
	}
	fprintf(file, "void __ocmain (void) {\n");
	for(auto node : stringcon_list){
		fprintf(file, "        ");
		emitter_stringcon(file, node);
	}
	for(auto child : root->children){
		if(child->symbol == TOK_VARDECL){
			fprintf(file, "        ");
			child->vreg = mangle_ident(child->children[0]);
			fprintf(file, child->vreg.c_str());
		}
		else if(child->symbol != TOK_FUNCTION && 
				child->symbol != TOK_PROTOTYPE){
				emitter_rec(file, root);
		}
	}
	fprintf(file, "}\n");
	for(auto child : root->children){
		if(child->symbol == TOK_FUNCTION){
			emitter_func(file, child);
		}
	}
}