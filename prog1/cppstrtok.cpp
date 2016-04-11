// $Id: cppstrtok.cpp,v 1.4 2016-03-24 14:34:10-07 - - $

// Use cpp to scan a file and print line numbers.
// Print out each input line read in, then strtok it for
// tokens.

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

#include "auxlib.h"
#include "stringset.h"

static char taskl; //commandline option select
static char tasky;

const string CPP = "/usr/bin/cpp";
constexpr size_t LINESIZE = 1024;

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
         //printf ("DIRECTIVE: line %d file \"%s\"\n", linenr, filename);
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

//scan options 
void scan_opts (char** argv) {

if(strcmp(argv[1], "-l") == 0){
      taskl = 'l';
         cout << "***-l selected***" << endl;
      } else if(strcmp(argv[1], "-y") == 0){
      tasky = 'y';
         cout << "***-y selected***" << endl;
      } else if(strcmp(argv[1], "-ly") == 0){
      taskl = 'l';
      tasky = 'y';
      cout << "***-ly selected***" << endl;
      }

   }


int main (int argc, char** argv) {
   set_execname (argv[0]);
   
   for (int argi = 1; argi < argc; ++argi) {
      char* filename = argv[argi];
      string command = CPP + " " + filename;
      //printf ("command=\"%s\"\n", command.c_str());
      FILE* pipe = popen (command.c_str(), "r");
      if (pipe == NULL) {
         syserrprintf (command.c_str());
      }else {
         cpplines (pipe, filename);
         int pclose_rc = pclose (pipe);
         eprint_status (command.c_str(), pclose_rc);
         if (pclose_rc != 0) set_exitstatus (EXIT_FAILURE);
      }
      string test = str(filename);

      ofstream file;
      file.open (test + ".str"); //assign to file name
      dump_stringset(file);
      file.close();
   }

//scan options
scan_opts (argv);

if(taskl == 'l'){
   cout << "yy_flex_debug = 1 yydebug = 0" << endl;
} else if(tasky == 'y'){
   cout << "yy_flex_debug = 0 yydebug = 1" << endl;
} else if(taskl == 'l' || tasky == 'y'){
   cout << "yy_flex_debug = 1 yydebug = 1" << endl;
} else {
   cout << "yy_flex_debug = 0 yydebug = 0" << endl;
}

//Create .str file here

   //print hashtable
   //dump_stringset(cout);

   return get_exitstatus();
}