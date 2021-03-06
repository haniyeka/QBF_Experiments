
/********************************************************************************************
main.cpp -- Copyright (c) 2013-2016, Tobias Schubert, Sven Reimer

Permission is hereby granted, free of charge, to any person obtaining a copy of this 
software and associated documentation files (the "Software"), to deal in the Software 
without restriction, including without limitation the rights to use, copy, modify, merge, 
publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons 
to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
********************************************************************************************/

// Include standard headers.
#include <sys/resource.h>
#include <cassert>
#include <csignal>
#include <vector>
#include <fstream>
#include <omp.h>
#include <unistd.h>

// Include antom related headers.
#include "antom.h"
#include "parser.h"

//#define COMPETITION

// Some more headers. 
void SIGSEGV_handler(int signum);
void printUsage( void );

// An example demonstrating how to use antom.
int main (int argc, char** argv)
{ 
  // Define signal handling functions.
  signal(SIGSEGV,SIGSEGV_handler);
  antom::Parser parser;

  // Initialization.
  std::cout.precision(2);
  std::cout.setf(std::ios::unitbuf);
  std::cout.setf(std::ios::fixed);
  
  // Output.
  std::cout << "c =================================================================" << std::endl
			<< "c antom; Tobias Schubert, Sven Reimer; University of Freiburg, 2016" << std::endl
			<< "c =================================================================" << std::endl;

  // Wrong number of command line parameters?
  if (argc < 3)
    {

	  parser.PrintUsage();
      // Return UNKNOWN.
      return ANTOM_UNKNOWN;
    }


  // Parse command lines
  if( parser.ParseCommandline(argc,argv) == ANTOM_UNKNOWN )
	{ return ANTOM_UNKNOWN; }

  parser.PrintSettings(argc, argv);

#ifndef NDEBUG
  std::cout << "c running in DEBUG mode" << std::endl;
#endif

  // Get a first timestamp.
#ifdef PARALLEL
  double start(omp_get_wtime());
#endif

  // Initialize an "antom" object.
  antom::Antom myAntom(parser.threads); 

  // Set all collected options
  parser.SetSettings(myAntom);
 
  // Initialization.
  uint32_t maxIndexOrigVars(0); 

  // Load the benchmark file specified by the user.
  if (!parser.LoadCNF(argv[(uint32_t) argc - 1], maxIndexOrigVars, myAntom))
	{
      // Output.
      std::cout << "s UNSATISFIABLE" << std::endl; 
      
      // Return UNSAT.
      return ANTOM_UNSAT;
    }
  
  // Solve the benchmark specified by the user.
  uint32_t result(ANTOM_UNKNOWN); 

  int32_t optimum(-1);

  // First preprocess the formula
  // Do not apply Preprocessing with max-antom, since it's performed within "maxSolve()" incrementally
  if( parser.mode < 10 && parser.preprocess != antom::NOPREPRO )
	{ result = myAntom.Preprocess(); }

  //std::vector< uint32_t > assumptions;

  if( result == ANTOM_UNKNOWN )
	{
	  switch(parser.mode)
		{ 
		case  0: result = myAntom.Solve(/*assumptions*/); break; 
		case  1: myAntom.SetVerbosity(1); result = myAntom.SolveSATzilla(); break; 
		case 10: result = myAntom.MaxSolve(optimum); break;
		case 20: result = myAntom.MaxSolve(optimum); break; 
		case 30: result = myAntom.MaxSolveWeightedPartial(optimum); break;
		case 31: result = myAntom.MaxSolve(optimum); break;
		case 99: myAntom.GetDataRegressionAnalysis(); break; 
		default : std::cout << "unknown mode" << std::endl; return 0; break;
		}
	}

  // Get the CPU time. 
  struct rusage resourcesS;
  getrusage(RUSAGE_SELF, &resourcesS); 
  double timeS((double) resourcesS.ru_utime.tv_sec + 1.e-6 * (double) resourcesS.ru_utime.tv_usec);
  timeS += (double) resourcesS.ru_stime.tv_sec + 1.e-6 * (double) resourcesS.ru_stime.tv_usec;

  // Get the wall clock time.
  double residentset = resourcesS.ru_maxrss*1024;
#ifdef PARALLEL
  double timeW(omp_get_wtime() - start);
#endif

  // Output.
  std::cout << "c #ID fastest thread.....: " << myAntom.SolvingThread()            << std::endl
			<< "c #variables.............: " << myAntom.Variables()                << std::endl
			<< "c    #used...............: " << myAntom.UsedVariables()            << std::endl
			<< "c #clauses...............: " << myAntom.Clauses()                  << std::endl
			<< "c    #binary.............: " << myAntom.CurrentBinaryClauses()     << std::endl
			<< "c    #ternary............: " << myAntom.CurrentTernaryClauses()    << std::endl
			<< "c    #nary...............: " << myAntom.CurrentNaryClauses()       << std::endl
			<< "c #literals..............: " << myAntom.Literals()                 << std::endl
			<< "c #decisions.............: " << myAntom.Decisions()                << std::endl
			<< "c #bcp operations........: " << myAntom.Bcps()                     << std::endl
			<< "c #implications..........: " << myAntom.Implications()             << std::endl
			<< "c #conflicts.............: " << myAntom.Conflicts()                << std::endl
			<< "c #restarts..............: " << myAntom.Restarts()                 << std::endl
			<< "c #simplifications.......: " << myAntom.Simplifications()          << std::endl
			<< "c #synchronizations......: " << myAntom.Synchronizations()         << std::endl
			<< "c #lhbr clauses..........: " << myAntom.Lhbr()                     << std::endl
			<< "c #learnt unit clauses...: " << myAntom.LearntUnitClauses()        << std::endl
			<< "c #learnt binary clauses.: " << myAntom.LearntBinaryClauses()      << std::endl
			<< "c #learnt ternary clauses: " << myAntom.LearntTernaryClauses()     << std::endl
			<< "c average lbd............: " << myAntom.AvgLBD()                   << std::endl
			<< "c average cc length......: " << myAntom.AvgCCLength()              << std::endl
			<< "c average dec. level.....: " << myAntom.AvgDL()                    << std::endl
			<< "c average lev. cleared...: " << myAntom.AvgDLclearedCA()           << std::endl
			<< "c average vars unassigned: " << myAntom.AvgVarsUnassignedCA()      << std::endl
			<< "c #inprocessings.........: " << myAntom.Inprocessings()            << std::endl;
  if( parser.preprocess || ( (parser.inprocess || parser.maxinprocess) && (myAntom.Inprocessings() > 0) ) )
	{
	  std::cout << "c pre/inpro stats---------" << std::endl
				<< "c #binary constants......: " << myAntom.BinaryConstants() << std::endl
				<< "c #binary equivalances...: " << myAntom.BinaryEquivalences()<< std::endl
				<< "c #upla constants........: " << myAntom.UplaConstants() << std::endl
				<< "c #upla equivalances.....: " << myAntom.UplaEquivalences() << std::endl
				<< "c #constants by SAT check: " << myAntom.SatConstants() << std::endl
				<< "c #variable eliminations.: " << myAntom.VariableEliminations() << std::endl
				<< "c literal reduction elim.: " << myAntom.LiteralEliminations() << std::endl
				<< "c #blocked clauses.......: " << myAntom.BlockedClauses() << std::endl
				<< "c #hidden tautologies....: " << myAntom.HiddenTautologies() << std::endl
				<< "c #hidden subsumptions...: " << myAntom.HiddenSubsumptions() << std::endl
				<< "c #monotone variables....: " << myAntom.MonotoneVariables() << std::endl
				<< "c #dc variables..........: " << myAntom.DcVariables() << std::endl
				<< "c #subsumed clauses......: " << myAntom.SubsumedClauses() << std::endl
				<< "c #selfsubsumed literals.: " << myAntom.SelfsubsumedLiterals() << std::endl
				<< "c #bva variables.........: " << myAntom.BvaVariables() << std::endl
				<< "c literal reduction bva..: " << myAntom.BvaLiterals() << std::endl
				<< "c #vivify subsumptions...: " << myAntom.VivifySubsumptions() << std::endl
				<< "c #vivify units..........: " << myAntom.VivifyUnits() << std::endl
				<< "c #vivify literal diff...: " << myAntom.VivifyDiff() << std::endl
				<< "c #unit propagations.....: " << myAntom.UnitPropagations() << std::endl
				<< "c runtime upla...........: " << myAntom.RuntimeUPLA() << std::endl
				<< "c runtime subsumptions...: " << myAntom.RuntimeSubsumption() << std::endl
				<< "c runtime varEliminations: " << myAntom.RuntimeVarElim() << std::endl
				<< "c runtime bce............: " << myAntom.RuntimeBCE() << std::endl
				<< "c runtime hte............: " << myAntom.RuntimeHTE() << std::endl
				<< "c runtime bva............: " << myAntom.RuntimeBVA() << std::endl
				<< "c runtime vivification...: " << myAntom.RuntimeVivify() << std::endl
				<< "c pre/inpro time.........: " << myAntom.RuntimePrepro() << std::endl
				<< "c ------------------------" << std::endl;
	}
  std::cout << "c cpu time...............: " << timeS << "s"                       << std::endl
#ifdef PARALLEL
			<< "c wall clock time........: " << timeW << "s"                       << std::endl
			<< "c cpu utilization........: " << ((timeS / timeW) * 100.0) << "%"   << std::endl
#endif
			<< "c resident set memory....: " << (residentset/(1024*1024)) << " MB" << std::endl
			<< "c ===================================================="            << std::endl;

  // Satisfiable formula?
  if (result == ANTOM_SAT) 
    {
      // Output.

	  if (!parser.storeResult)
		{ std::cout << "s SATISFIABLE" << std::endl; }
	  
	  if (parser.mode > 1)
		{ 
		  assert(parser.mode >= 10 && parser.mode < 40); 
		  std::cout << "o " << optimum << std::endl 
					<< "s OPTIMUM FOUND" << std::endl; 
		}

      // Get the satisfying variable assignment.
      const std::vector<uint32_t>& model(myAntom.Model());

      // Consistency check.
      assert(maxIndexOrigVars > 0); 
      assert(maxIndexOrigVars < model.size()); 

      // Should we print or save the model?
      if (!parser.storeResult || parser.mode > 1)
		{
		  // Output.
		  std::cout << "v ";
		  for (uint32_t m = 1; m <= maxIndexOrigVars; ++m)
			{
			  if (model[m] != 0)
				{
				  if ((model[m] & 1) == 1)
					{ std::cout << "-"; }
				  std::cout << (model[m] >> 1) << " "; 
				}
			}
		  if (parser.mode <= 1)
			{ std::cout << "0"; } 
		  std::cout << std::endl; 
		}
      else
		{
		  // Store our solution so that SatELite is able to extend the partial model. 
		  assert(parser.mode <= 1 && parser.storeResult); 
		  std::ofstream satELite(parser.resultFile);
		  satELite << "SAT\n";
		  for (uint32_t m = 1; m <= maxIndexOrigVars; ++m)
			{ 
			  assert(model[m] != 0);
			  if ((model[m] & 1) == 1)
				{ satELite << "-"; }
			  satELite << (model[m] >> 1) << " "; 
			}
		  satELite << "0\n";
		  satELite.close();
		}


	  // Check model with a new and plain antom core
	  if( parser.verify )
		{
		  antom::Antom verify(0); 
		  verify.SetPreprocessing(antom::NOPREPRO);
		  verify.SetInprocessing(false);

		  //std::vector<uint32_t> verifytriggerVars;
		  uint32_t maxverifyindex(0);
		  if (!parser.LoadCNF(argv[(uint32_t) argc - 1], maxverifyindex, verify))
			{
			  std::cout << "Bad input" << std::endl;
			  assert(false);
			}

		  std::vector< uint32_t > verifyassumptions;
		  for( uint32_t j = 1; j <= maxverifyindex; ++j )
			{
			  if( model[j] != 0 )
				{ 
				  verifyassumptions.push_back(model[j]); 
				}
			}
		  uint32_t verifyresult = verify.Solve(verifyassumptions);

		  if( verifyresult == ANTOM_SAT )
			{
			  std::cout << "Everything okay with model!" << std::endl;
			}
		  else
			{
			  assert( verifyresult == ANTOM_UNSAT );
			  std::cout << "WRONG MODEL" << std::endl;
			  assert(false);
			}
		}

      // Return SAT.
      return ANTOM_SAT; 
    }

  if( result == ANTOM_UNKNOWN )
	{
	  std::cout << "s TIMEOUT" << std::endl; 
	  return ANTOM_UNKNOWN; 
	}
  // Output.
  if (parser.mode != 0 || !parser.storeResult)
    { std::cout << "s UNSATISFIABLE" << std::endl; }

  // Return UNSAT.
  return ANTOM_UNSAT; 
}


// Terminate antom in case of a segmentation fault.
void SIGSEGV_handler(int signum) 
{
  // Output.
  std::cout << "c segmentation fault (signal " << signum << ")" << std::endl
			<< "s UNKNOWN" << std::endl; 

  // Terminate with UNKNOWN.
  exit(ANTOM_UNKNOWN); 
} 
