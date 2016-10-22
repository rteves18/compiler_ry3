// $Id: cppstrtok.cpp,v 1.4 2016-03-24 14:34:10-07 - - $

// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.
//Partner Name: Ryan Teves Username: rteves
//Partner Name: Matthew Kim Username: madkim

#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#include <ctype.h>
#include <unistd.h>

#include "auxlib.h"
#include "stringset.h"
#include "astree.h"
#include "lyutils.h"

char* auxlib_value = NULL;
char* D_string = NULL;
int D_value = 0;

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;
extern FILE *yyin;
FILE* tokfile;
string cpp_command;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}


// Run cpp against the lines of the file.
void cpplines (FILE* pipe, char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy (inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == NULL) break;
      chomp (buffer, '\n');
      //printf ("%s:line %d: [%s]\n", filename, linenr, buffer);
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, filename);
      if (sscanf_rc == 2) {
         
         continue;
      }
      char* savepos = NULL;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if (token == NULL) break;
         intern_stringset(token);
         //printf ("token %d.%d: [%s]\n",
                 //linenr, tokenct, token);
      }
      ++linenr;
   }
}

void cpp_popen (char* filename) {
   cpp_command = CPP + " " + filename;
   yyin = popen (cpp_command.c_str(), "r");
   if (yyin == NULL) {
      syserrprintf (cpp_command.c_str());
      exit (get_exitstatus());
   }else {
      cpplines (yyin, filename);
      if (yy_flex_debug) {
         fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
                  cpp_command.c_str(), fileno (yyin));
      }
      lexer_newfilename (cpp_command.c_str());
   }
   fclose(yyin);
   yyin = popen (cpp_command.c_str(), "r");
}

void cpp_pclose() {
   int pclose_rc = pclose (yyin);
   eprint_status (cpp_command.c_str(), pclose_rc);
   if (pclose_rc != 0)  set_exitstatus(EXIT_FAILURE);
}

void scan_opts (int argc, char **argv) {
int c;
  opterr = 0;
  while ((c = getopt (argc, argv, "ly@D:")) != -1)
    switch (c)
      {
      case 'l':
        yy_flex_debug = 1;
        break;
      case 'y':
        yydebug = 1;
        break;
      case 'D':
         D_string = optarg;
         D_value = 1;
         break;         
      case '@':
         set_debugflags(optarg);
         break;
      default:
        abort ();
      }
      /*
      if (yy_flex_debug == 0) {
      printf ("Usage: oc [-ly] [filename]\n");
                 //exec::execname.c_str());
      set_exitstatus(EXIT_FAILURE);
      return;
      }
   */
   }

int main (int argc, char** argv) {
  set_execname (argv[0]);
  scan_opts(argc,argv);
  
  char* filename = argv[argc - 1];

      //Append file name to .str and .tok      
      string filein = filename;
      int last = filein.find_last_of(".");
      string filename_str = filein.substr(0, last);
      string filename_tok = filein.substr(0, last);
      filename_str += ".str";
      filename_tok += ".tok";
      string newfile_str = filename_str;
      string newfile_tok = filename_tok;
      
      //open .tok file
      tokfile = fopen(newfile_tok.c_str(), "w"); 

      cpp_popen(filename);

       for(;;){
        int token = yylex();
        if(token == YYEOF)   break;
      }
      //make .str file
      ofstream strfile; 
      strfile.open(newfile_str);
      dump_stringset(strfile);

      cpp_pclose();

      //close .str and .tok file
      strfile.close();
      fclose(tokfile);

   return get_exitstatus();
}
