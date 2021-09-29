#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "ReadQ.hh"
#include "MySolver.hh"
#include "debug.hh"
#include <stdio.h>

using std::endl;
using std::cerr;
using std::cin;
using std::cout;
using std::string;

void print_usage(char* instance);
void print_exit(bool sat);

int main(int argc, char** argv)
{
  SolverOptions opt;
  if(!opt.parse(argc, argv))
  {
    print_usage(argv[0]);
    exit(100);
  }

  const string file_name = argv[argc - 1];
  gzFile file = Z_NULL;

  file = gzopen(file_name.c_str(), "rb");
  if (file == Z_NULL) {
    cerr << "Unable to open file: " << file_name << endl;
    exit(100);
  }

  Reader* file_reader = new Reader(file);
  ReadQ qbf_reader(*file_reader);

  try {
    qbf_reader.read();
  } catch (ReadException &rex) {
    cerr << rex.what() << endl;
    delete file_reader;
    exit(100);
  }

  if (!qbf_reader.get_header_read()) {
    if (qbf_reader.get_prefix().size() == 0) {
      cerr << "ERROR: Formula has empty prefix and no problem line." << endl;
      delete file_reader;
      exit(100);
    }
    cerr << "WARNING: Missing problem line in the input file." << endl;
  }

  if (qbf_reader.get_prefix().size() == 0) {
    delete file_reader;
    print_exit(qbf_reader.get_clauses().empty());
  }

  delete file_reader;

  MySolver s(qbf_reader.get_prefix(), qbf_reader.get_clauses(), &opt);

  debugn( "Calling the solver." );
  const bool sat = s.solve();
  s.printStats();
  /*
  if(sat && qbf_reader.get_prefix()[0].first == EXISTENTIAL)
  {
    for(const Lit& l : s.getSolution())
      cout << l << " ";
    cout << endl;
  }
  */

  if(opt.logging_phi)
    if(std::remove(opt.tmp_phi_log.c_str()) != 0)
      cout << "c removing tmp file failed" << endl;

  print_exit(sat);
}

void print_exit(bool sat)
{
  cout << "s cnf " << (sat ? '1' : '0') << endl;
  exit(sat ? 10 : 20);
}

void print_usage(char * instance)
{
  cerr << "Usage:  " << instance <<  " <OPTIONS> <FILENAME>" << endl;
  cerr << "<OPTIONS> may contain the following:\n" << USAGE_TEXT;
}
