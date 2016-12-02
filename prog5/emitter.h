// $Id: emitter.h,v 1.1 2015-07-09 14:08:38-07 - - $

#ifndef __EMIT_H__
#define __EMIT_H__

#include <iostream>
#include <string>

#include "astree.h"
#include "auxlib.h"


string mangle_struct(astree* node);
string mangle_ident(astree* node);
string new_vreg(char type); 

void emit_code(FILE* file, astree* root);

#endif

