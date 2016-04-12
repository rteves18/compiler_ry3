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

//static char taskl; //commandline option select
//static char tasky;
int yy_flex_debug = 0;
int yydebug = 0;
char* auxlib_value = NULL;
char* D_string = NULL;
int D_value = 0;

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
         auxlib_value = optarg;
         break;
      default:
        abort ();
      }
   }


int main (int argc, char** argv) {
   set_execname (argv[0]);
   for (int argi = 1; argi < argc; ++argi) {
      char* filename = argv[argc - 1];
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

   string filein = filename;
      int last = filein.find_last_of(".");
      string filename_ns = filein.substr(0, last);
      filename_ns += ".str";
      string newfile_out = filename_ns;
      
      ofstream newfile; 
      newfile.open(newfile_out);
      dump_stringset(newfile);
      newfile.close();
   }

scan_opts(argc,argv);

if(auxlib_value != NULL){
   set_debugflags(auxlib_value);
}

   return get_exitstatus();
}
