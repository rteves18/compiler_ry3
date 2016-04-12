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

#include <ctype.h>
#include <unistd.h>

#include "auxlib.h"
#include "stringset.h"

//static char taskl; //commandline option select
//static char tasky;
int yy_flex_debug = 0;
int yydebug = 0;
char* cvalue = NULL;

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

//scan options **old**
/* 
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
      } else {
         cout << "***Invalid Entry***" << endl;
         return;
      }

   }
*/

void scan_opts (int argc, char **argv) {


int index;
int c;
  opterr = 0;
  while ((c = getopt (argc, argv, "lyc:")) != -1)
    switch (c)
      {
      case 'l':
        yy_flex_debug = 1;
        break;
      case 'y':
        yydebug = 1;
        break;
      case 'c':
        cvalue = optarg;
        break;
      //case '?':
        //if (optopt == 'c')
          //fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        //else if (isprint (optopt))
          //fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        //else
          //fprintf (stderr,
                   //"Unknown option character `\\x%x'.\n",
                   //optopt);
        //return 1;
      default:
        abort ();
      }
  //printf ("yy_flex_debug = %d, yydebug = %d, cvalue = %s\n",
          //yy_flex_debug, yydebug, cvalue);

  //for (index = optind; index < argc; index++)
    //printf ("Non-option argument %s\n", argv[index]);
  //return 0;
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
      /*
      string test = str(filename);

      ofstream file;
      file.open (test + ".str"); //assign to file name
      dump_stringset(file);
      file.close();
      */
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

//scan options **old**
   /*
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
*/


//other .ctr file
/*
char* filename = argv[1];
      for(int i=2; i < argc; i++){
         strcat(filename, argv[i]);
      }
*/
// Vanessa's str
      /*
      string file_string = filename;
      int end = file_string.find_last_of(".");
      string file_rawname = file_string.substr(0, end);
      file_rawname += ".str";
      string file_out = file_rawname;
      std::ofstream out (file_out, std::ofstream::out);
   // dump
      dump_stringset(out);
      */
//New str
      /*
      string filein = filename;
      int last = filein.find_last_of(".");
      string filename_ns = filein.substr(0, last);
      filename_ns += ".str";
      string newfile_out = filename_ns;
      
      ofstream newfile; 
      newfile.open(newfile_out);
      dump_stringset(newfile);
      newfile.close();
      */
//old str
      /*
      ofstream file;
      char* filename = argv[1];
      for(int i=2; i < argc; i++){
         strcat(filename, argv[i]);
      }

      std::string sfilename(filename);

      if(sfilename.find(".oc")) {
         chomp(filename, 'c');
         chomp(filename, 'o');
         strcat(filename, "str");
         file.open (filename); //assign to file name
         dump_stringset(file);
      }
      else{
         cout << "Invalid Input" << endl;
      }

      file.close();
      */

   //print hashtable
   //dump_stringset(cout);

   return get_exitstatus();
}