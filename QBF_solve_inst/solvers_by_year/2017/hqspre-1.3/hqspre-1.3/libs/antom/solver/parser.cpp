/********************************************************************************************
parser.cpp -- Copyright (c) 2013-2016, Tobias Schubert, Sven Reimer

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

#include "parser.h"
#include "antom.h"
//#include "solverproxy.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace antom 
{
  Parser::Parser(void) :
	storeResult(false),
	resultFile(""),
	mode(0),
	threads(1),
	verbose(0),
	cpuLimit(0.0),
	memLimit(0),
	verify(false),
	solver(ANTOMSOLVER),
	restartStrategy(LUBY),
	unitFactor(8),
	decayFactor(1.05),
	simplifyStrategy(ANTOM),
	simplifyActivity(SIMP_LBD),
	decisionStrategy(CACHEANDTOGGLE),
	ternary(false),
	lhbr(false),
	preprocess(NOPREPRO),
	inprocess(false),
	maxinprocess(false),
	maxLoops(5),
	upla(true),
	subsumption(true),
	varElim(true),
	bce(true),
	hte(true),
	hse(true),
	bva(true),
	bvatwolitdiff(false),
	vivify(true),
	satconst(0),
	incomplete(false),
	searchMode(SATBASED),
	networktype(BITONIC),
	decstrat(0),
	encode01(true),
	sortsoftclauses(0),
	gridmode(0),
	bypassWidth(4),
	csc(0),
	skip(false),
	target(-1),
	preciseTarget(false),
	partialMode(NONE),
	width(32),
	maxWidth(0),
	maxParts(0),
	relaxation(false),
	base(2)
  {}


  uint32_t Parser::ParseCommandline(int argc, char** argv)
  {
	// Get the additional command line parameters.
	for (uint32_t c = 1; c < ((uint32_t) argc - 1); ++c)
	  {
		// Initialization.
		std::string argument(argv[c]); 
		bool matched(false);      

		// Basic options
		// Result file
		if (argument.compare(0, 14, "--result-file=") == 0)
		  { std::stringstream ss(argument.substr(14, argument.length())); ss >> resultFile; storeResult = true; matched = true; continue; }
		// Operating mode
		else if (argument == "--mode=0")
		  { mode = 0; matched = true; continue;}
		else if (argument == "--mode=1")
		  { mode = 1; matched = true; continue; }
		else if (argument == "--mode=10")
		  { mode = 10; matched = true; continue; }
		else if (argument == "--mode=20")
		  { mode = 20; matched = true; continue; }
		else if (argument == "--mode=30")
		  { mode = 30; matched = true; continue; }
		else if (argument == "--mode=31")
		  { mode = 31; matched = true; continue; }
		else if (argument == "--mode=99")
		  { mode = 99; matched = true; continue; }
		// Number of threads
		else if (argument == "--threads=1")
		  { threads = 1; matched = true; continue; }
		else if (argument == "--threads=2")
		  { threads = 2; matched = true; continue; }
		else if (argument == "--threads=3")
		  { threads = 3; matched = true; continue; }
		else if (argument == "--threads=4")
		  { threads = 4; matched = true; continue; }
		else if (argument == "--threads=5")
		  { threads = 5; matched = true; continue; }
		else if (argument == "--threads=6")
		  { threads = 6; matched = true; continue; }
		else if (argument == "--threads=7")
		  { threads = 7; matched = true; continue; }
		else if (argument == "--threads=8")
		  { threads = 8; matched = true; continue; }
		else if (argument == "--verbose" || argument == "--v")
		  { ++verbose; matched = true; continue; }
		else if (argument == "--verbose=0")
		  { verbose = 0; matched = true; continue; }
		// CPU Time limit
		else if (argument.compare(0,11,"--cpuLimit=") == 0)
		  { std::stringstream sCPU(argument.substr(11,argument.length())); sCPU >> cpuLimit; matched = true; continue; }
		// Memory Limit
		else if (argument.compare(0,11,"--memLimit=") == 0)
		  { std::stringstream sMem(argument.substr(11,argument.length())); sMem >> memLimit; matched = true; continue; }
		// Verify model 
		else if (argument == "--verify=true")
		  { verify = true; matched = true; continue; }
		else if (argument == "--verify=false")
		  { verify = false; matched = true; continue; }
		// Backend solver
		else if (argument == "--solver=antom")
		  { solver = ANTOMSOLVER; matched = true; continue; }
		else if (argument == "--solver=cryptominisat")
		  { solver = CRYPTOMINISATSOLVER; matched = true; continue; }
		// Core Options
		// Restart strategy
		else if (argument == "--restart=luby")
		  { restartStrategy = LUBY; matched = true; continue; }
		else if (argument == "--restart=glucose")
		  { restartStrategy = GLUCOSE; matched = true; continue; }
		// What about the unit factor?
		else if (argument.compare(0, 13, "--unitFactor=") == 0)
		  { std::stringstream ss(argument.substr(13, argument.length())); ss >> unitFactor; matched = true; continue; }      // What about the decay factor?
		else if (argument.compare(0, 14, "--decayFactor=") == 0)
		  { std::stringstream ss(argument.substr(14, argument.length())); ss >> decayFactor; matched = true; continue; }
		// Simplifaction strategy
		else if (argument == "--simplify=antom")
		  { simplifyStrategy = ANTOM; matched = true; continue; }
		else if (argument == "--simplify=minisat")
		  { simplifyStrategy = MINISAT; matched = true; continue; }
		// Base for simplifaction activity 
		else if (argument == "--activity=lbd")
		  { simplifyActivity = SIMP_LBD; matched = true; continue; }
		else if (argument == "--activity=conf")
		  { simplifyActivity = SIMP_CONF; matched = true; continue; }
		// Decision strategy?
		else if (argument == "--decision=0")
		  { decisionStrategy = CACHEANDTOGGLE; matched = true; continue; }
		else if (argument == "--decision=1")
		  { decisionStrategy = CACHE; matched = true; continue; }
		else if (argument == "--decision=2")
		  { decisionStrategy = ALWAYSFALSE; matched = true; continue; }
		else if (argument == "--decision=3")
		  { decisionStrategy = ALWAYSTRUE; matched = true; continue; }
		// Special treatment for ternary clauses?
		else if (argument == "--ternary=true")
		  { ternary = true; matched = true; continue; }
		else if (argument == "--ternary=false")
		  { ternary = false; matched = true; continue; }
		// Lazy Hyper Binary Resolution
		else if (argument == "--lhbr=true") 
		  { lhbr = true; matched = true; continue; }
		else if (argument == "--lhbr=false")
		  { lhbr = false; matched = true; continue; }
		// Preprocessor
		// Perfoming preprocessing
		else if (argument == "--prepro=false")
		  { preprocess = NOPREPRO; matched = true; continue; }
		else if (argument == "--prepro=true")
		  { preprocess = PREPROCESS; matched = true; continue; }
		else if (argument == "--prepro=incremental")
		  { preprocess = INCREMENTAL; matched = true; continue; }
		// Perfoming inprocessing
		else if (argument == "--inpro=true")
		  { inprocess = true; matched = true; continue; }
		else if (argument == "--inpro=false")
		  { inprocess = false; matched = true; continue; }
		// inprocessing during MaxSAT
		else if (argument == "--maxInpro=true")
		  { maxinprocess = true; matched = true; continue; }
		else if (argument == "--maxInpro=false")
		  { maxinprocess = false; matched = true; continue; }
		// Maximum preprocessor loops
		else if (argument.compare(0, 11, "--maxLoops=") == 0)
		  { std::stringstream ss(argument.substr(11, argument.length())); ss >> maxLoops; matched = true; continue; }
		// UPLA
		else if (argument == "--upla=true")
		  { upla = true; matched = true; continue; }
		else if (argument == "--upla=false")
		  { upla = false; matched = true; continue; }
		// Subsumption checks
		else if (argument == "--subsumption=true")
		  { subsumption = true; matched = true; continue; }
		else if (argument == "--subsumption=false")
		  { subsumption = false; matched = true; continue; }
		// Variable elimination
		else if (argument == "--varElim=true")
		  { varElim = true; matched = true; continue; }
		else if (argument == "--varElim=false")
		  { varElim = false; matched = true; continue; }
		// Blocked Clause Elimination
		else if (argument == "--bce=true")
		  { bce = true; matched = true; continue; }
		else if (argument == "--bce=false")
		  { bce = false; matched = true; continue; }
		// Hidden Tautology Elimination
		else if (argument == "--hte=true")
		  { hte = true; matched = true; continue; }
		else if (argument == "--hte=false")
		  { hte = false; matched = true; continue; }
		// Hidden Subsumption Elimination
		else if (argument == "--hse=true")
		  { hse = true; matched = true; continue; }
		else if (argument == "--hse=false")
		  { hse = false; matched = true; continue; }
		// Bounded variable addition
		else if (argument == "--bva=true")
		  { bva = true; matched = true; continue; }
		else if (argument == "--bva=false")
		  { bva = false; matched = true; continue; }
		else if (argument == "--2litdiff=true")
		  { bvatwolitdiff = true; matched = true; continue; }
		else if (argument == "--2litdiff=false")
		  { bvatwolitdiff = false; matched = true; continue; }
		// Vivification
		else if (argument == "--vivify=true")
		  { vivify = true; matched = true; continue; }
		else if (argument == "--vivify=false")
		  { vivify = false; matched = true; continue; }
		// Constant checking with SAT
		else if (argument == "--satconst=full")
		  { satconst = 2; matched = true; continue; }
		else if (argument == "--satconst=true")
		  { satconst = 1; matched = true; continue; }
		else if (argument == "--satconst=false")
		  { satconst = 0; matched = true; continue; }
		// MaxSAT options
		// Applying incomplete mode
		else if (argument == "--incomplete=false")
		  { incomplete = false; matched = true; continue; }
		else if (argument == "--incomplete=true")
		  { incomplete = true; matched = true; continue; }
		// Search mode
		else if (argument == "--search=0")
		  { searchMode = UNSATBASED; matched = true; continue; }
		else if (argument == "--search=1")
		  { searchMode = SATBASED; matched = true; continue; }
		else if (argument == "--search=2")
		  { searchMode = BINARYBASED; matched = true; continue; }
		// Network type
		else if (argument == "--network=0")
		  { networktype = BITONIC; matched = true; continue; }
		else if (argument == "--network=1")
		  { networktype = ODDEVEN; matched = true; continue; }
		else if (argument == "--network=3")
		  { networktype = TOTALIZER; matched = true; continue; }
		// Decision strategies for MaxSAT
		else if (argument == "--decstrat=0")
		  { decstrat = 0; matched = true; continue; }
		else if (argument == "--decstrat=1")
		  { decstrat = 1; matched = true; continue; }
		else if (argument == "--decstrat=2")
		  { decstrat = 2; matched = true; continue; }
		// Encode 01?
		else if (argument == "--encode01=false")
		  { encode01 = false; matched = true; continue; }
		else if (argument == "--encode01=true")
		  { encode01 = true; matched = true; continue; }
		else if (argument == "--sortsoft=0")
		  { sortsoftclauses = 0; matched = true; continue; }
		else if (argument == "--sortsoft=1")
		  { sortsoftclauses = 1; matched = true; continue; }
		else if (argument == "--sortsoft=2")
		  { sortsoftclauses = 2; matched = true; continue; }
		else if (argument == "--sortsoft=3")
		  { sortsoftclauses = 3; matched = true; continue; }
		// Bypasses
		else if (argument == "--gridMode=0")
		  { gridmode = 0; matched = true; continue; }
		else if (argument == "--gridMode=1")
		  { gridmode = 1; matched = true; continue; }
		else if (argument == "--gridMode=2")
		  { gridmode = 2; matched = true; continue; }
		else if (argument == "--gridMode=3")
		  { gridmode = 3; matched = true; continue; }
		// Width for horizontal bypasses
		else if (argument.compare(0,12,"--bypassWidth=") == 0)
		  { std::stringstream sWidth(argument.substr(12,argument.length())); sWidth >> bypassWidth; matched = true; continue; }
		// Find conflicting soft clauses
		else if (argument == "--csc=false")
		  { csc = 0; matched = true; continue; }
		else if (argument == "--csc=true")
		  { csc = 1; matched = true; continue; }
		else if (argument == "--csc=all")
		  { csc = 2; matched = true; continue; }
		// Skip comparators
		else if (argument == "--skip=true")
		  { skip = true; matched = true; continue; }
		else if (argument == "--skip=false")
		  { skip = false; matched = true; continue; }
		// Partial MaxSAT Mode
		// Type of partial mode
		else if (argument == "--partial=0")
		  { partialMode = NONE; matched = true; continue; }
		else if (argument == "--partial=1")
		  { partialMode = DEPTHFIRST; matched = true; continue; }
		else if (argument == "--partial=2")
		  { partialMode = BREADTHFIRST; matched = true; continue; }
		// Fixed splitting width for partial mode
		else if (argument.compare(0,13,"--splitWidth=") == 0)
		  { std::stringstream sWidth(argument.substr(13,argument.length())); sWidth >> width; matched = true; continue; }
		// Maximal splitting width for partial mode
		else if (argument.compare(0,11,"--maxWidth=") == 0)
		  { std::stringstream sWidth(argument.substr(11,argument.length())); sWidth >> maxWidth; matched = true; continue; }
		// Maximial number of subparts for partial mode
		else if (argument.compare(0,11,"--maxParts=") == 0)
		  { std::stringstream sParts(argument.substr(11,argument.length())); sParts >> maxParts; matched = true; continue; }
		// Use fixed relaxation lits
		else if (argument == "--relax=true")
		  { relaxation = true; matched = true; continue; }
		else if (argument == "--relax=false")
		  { relaxation = false; matched = true; continue; }
		// Define target optimum
		else if (argument.compare(0, 12, "--targetOpt=") == 0)
		  { std::stringstream ss(argument.substr(12, argument.length())); ss >> target; matched = true; continue; }
		// Find closed solution to target value
		else if (argument == "--preciseTarget=true")
		  { preciseTarget = true; matched = true; continue; }
		else if (argument == "--preciseTarget=false")
		  { preciseTarget = false; matched = true; continue; }
		// Base for weighted MaxSAT
		else if (argument.compare(0, 7, "--base=") == 0)
		  { std::stringstream ss(argument.substr(7, argument.length())); ss >> base; matched = true; continue; }
  
		// Unknown option?
		if (!matched)
		  {
			// Output. 
			std::cout << "c Unknown option: " << argv[c] << std::endl;
			PrintUsage();
			return ANTOM_UNKNOWN;
		  }
	  }

	// Check inconsistencies
	if( incomplete && (partialMode == 0 ) )
	  {
		std::cout << "c Incomplete mode only supported in combination with a partial mode" << std::endl;
		return ANTOM_UNKNOWN;
	  }
	if( !encode01 && (networktype != TOTALIZER ) )
	  {
		std::cout << "c encoding just 0's only suppted in combination with totalizer network" << std::endl;
		return ANTOM_UNKNOWN;
	  }

	return ANTOM_SAT;
  }

  void Parser::SetSettings(antom::Antom& myAntom) const
  {
	// Set antom's various parameters.

	myAntom.SetMemoryLimit(memLimit);
	myAntom.SetVerbosity(verbose);
	myAntom.SetVerbosity(verbose);
	myAntom.SetCPULimit(cpuLimit);
	myAntom.SetPreprocessing(preprocess);

	myAntom.SetRestartStrategy(restartStrategy); 
	myAntom.SetLuby(unitFactor); 
	myAntom.SetDecayFactor(decayFactor); 
	myAntom.SetSimplifyStrategy(simplifyStrategy);
	myAntom.SetSimplifyActivity(simplifyActivity);
	myAntom.SetDecisionStrategy(decisionStrategy, 0); 
	myAntom.UseTernaryClauses( ternary );
	myAntom.SetLHBR(lhbr); 

	myAntom.SetPreprocessing( preprocess );
	myAntom.SetInprocessing( inprocess );
	myAntom.SetMaxInprocessing( maxinprocess );
	myAntom.SetMaxLoops( maxLoops );
	myAntom.SetUPLA( upla );
	myAntom.SetSubsumption( subsumption );
	myAntom.SetVarElim( varElim );
	myAntom.SetBCE( bce );
	myAntom.SetHTE( hte );
	myAntom.SetHSE( hse );
	myAntom.SetBVA( bva );
	myAntom.SetTwoLiteralDiffBVA( bvatwolitdiff );
	myAntom.SetVivification( vivify );
	myAntom.SetSatConst( satconst );

	myAntom.SetIncompleteMode(incomplete);
	myAntom.SetSearchMode(searchMode);
	myAntom.SetNetworktype(networktype);
	myAntom.SetDecStratMode(decstrat);
	myAntom.SetEncode01Mode(encode01);
	myAntom.SetSortSoftClauses(sortsoftclauses);
	myAntom.SetGridMode(gridmode);
	myAntom.SetHorizontalWidth(bypassWidth);
	myAntom.SetCSC(csc);
	myAntom.SetSkip(skip);
	myAntom.SetOptTarget(target);
	myAntom.SetPreciseTarget(preciseTarget);

	myAntom.SetPartialMode(partialMode);
	myAntom.SetSplittedWidth(width);
	myAntom.SetMaxWidth(maxWidth);
	myAntom.SetMaxParts(maxParts);
	myAntom.SetRelaxationLits(relaxation);

	myAntom.SetBaseMode(base);
  }

  // Loads a formula from file, returns FALSE if the formula is unsatisfiable.
  // "type" has to be set according to the type of benchmark:
  // type = 0 --> SAT 
  // type = 1 --> MaxSAT
  // type = 2 --> partial MaxSAT
  bool Parser::LoadCNF(const std::string& file, uint32_t& maxIndexOrigVars, Antom& antom)
  {
	uint32_t type(0);
	if (mode >= 10 && mode < 20) { type = 1; }
	if (mode >= 20 && mode < 30 ) { type = 2; }
	if (mode >= 30 && mode < 40 ) { type = 3; }

	// Open the file.
	std::ifstream source;
	source.open(file.c_str());

	// Any problems while opening the file?
	if (!source) 
	  {
		// Output. 
		std::cout << "c Unable to open file" << std::endl
				  << "s UNKNOWN" << std::endl; 
      
		// Return UNKNOWN.
		exit(ANTOM_UNKNOWN); 
	  }

	// Variables.
	std::vector<uint32_t> clause;
	uint32_t maxVars(0);
	//uint32_t numClauses(0); 
	uint32_t literal(0); 
	uint32_t sign(0); 
	uint64_t topWeight(2);
	unsigned int weight(0);
	uint32_t threshold(0); 
	bool firstLit(true); 
	char c('0'); 

	// Process the file.
	while (source.good())
	  {
		// Get the next character.
		c = (char)source.get();
   
		// No more clauses?
		if (!source.good())
		  { break; }

		// Statistics?
		if (c == 'p')
		  {
			// Get the next char. 
			c = (char)source.get(); 

			// Remove whitespaces.
			while (c == ' ') 
			  { c = (char)source.get(); }

			// In case of a partial MaxSAT benchmark file, the next character has to be "w".
			if (type == 2 || type == 3)
			  { assert(c == 'w'); c = (char)source.get(); }

			// The next three characters have to be "c", "n", and "f".
			assert(c == 'c');
			c = (char)source.get();
			assert(c == 'n');
			c = (char)source.get(); 
			assert(c == 'f');
			c = (char)source.get(); 

			// Remove whitespaces.
			while (c == ' ') 
			  { c = (char)source.get(); }

			// Let's get the number of variables within the current CNF.
			while (c != ' ' && c != '\n') 
			  { maxVars = (maxVars * 10) + (unsigned int) c - '0'; c = source.get(); }

			// Update "maxIndexOrigVariables".
			maxIndexOrigVars = maxVars; 

			// Remove whitespaces.
			while (c == ' ') 
			  { c = source.get(); }

			// Get top weight in case of weighted partial MaxSAT benchmark file.
			if (type == 3)
			  {
				// Remove the number of clauses within the current CNF.
				while (c != ' ' && c != '\n')
				  { c = source.get(); }

				// Remove whitespaces.
				while (c == ' ')
				  { c = source.get(); }

				// Let's get the top weight - to differ later on hard & soft clauses.
				topWeight = 0;
				while (c != ' ' && c != '\n')
				  { topWeight = (topWeight * 10) + (unsigned int) c - '0'; c = source.get(); }

				std::cout << "topweight: " << topWeight << std::endl;

				if ( topWeight == 0 )
				  { topWeight = (unsigned int)-1; }
			  }

			// Remove whitespaces.
			while (c == ' ') 
			  { c = (char)source.get(); }

			
			// Set the maximum number of variables the solver has to deal
			antom.SetMaxIndex(maxVars);
		  }
		else
		  {
			// Clause? 
			if (c != 'c' && c != '%')
			  {
				// Reset "clause".
				clause.clear();

				// Initialize "firstLit".
				firstLit = true; 
	      
				// Initialize "threshold".
				threshold = 0; 

				// Do we have to solve a MaxSAT benchmark?
				if (type == 1)
				  {
					// Update "threshold".
					threshold = 1; 
				  }
				// Get the next clause.
				while (true)
				  {
					// Initialization.
					literal = 0;
					sign    = 0; 
					// Remove whitespaces.
					while (c == ' ') 
					  { c = (char)source.get(); }

					// Have we reached the clause stopper?
					if (c == '0')
					  {
						if( threshold == 1 )
						  {
							// Add "clause" to the clause database of "solver".	  
							if (clause.size() >= threshold && !antom.AddSoftClause(clause,weight))
							  { source.close(); return false; }		      
						  }
						// Add "clause" to the clause database of "solver".	  
						else if (clause.size() > threshold && !antom.AddClause(clause))
						  { source.close(); return false; }		      
						break; 
					  }
					// Let's get the next literal.
					while (c != ' ') 
					  {
						if (c == '-')
						  { sign = 1; }
						else
						  { literal = (literal * 10) + (uint32_t) c - '0'; }
						c = (char)source.get();
					  }

					// Consistency check.
					assert(literal != 0); 

					// In case we are loading a partial MaxSAT benchmark, the first literal
					// specifies whether the current clause is a "soft" or "hard" one.
					if ( (type == 2 || type == 3 ) && firstLit)
					  {
						// Reset "firstLit".
						firstLit = false;
						// Consistency check.
						assert(sign == 0);

						// Soft clause?
						if (literal < topWeight )
						  {
							// Update "threshold".
							threshold = 1; 

							weight = literal;
						  }
					  }
					else
					  {
						// Another consistency check, in this case due to (partial) MaxSAT solving. 
						assert((type != 1 && type != 2 && type != 3) || literal <= maxVars); 
						// Add "literal" to "clause".		      
						clause.push_back((literal << 1) + sign); 
						// Update "maxVars" if necessary.
						if (literal > maxVars)
						  {
							maxVars = literal;
							antom.SetMaxIndex(maxVars);
						  }
					  }
				  }
			  }
		  }

		// Go to the next line of the file. 
		while (c != '\n') 
		  { c = (char)source.get(); } 	  
	  }

	// Close the file.
	source.close();

	// Everything went fine.
	return true;
  }

  void Parser::PrintSettings(int argc, char** argv) const
  {
	// Output.
	switch (mode)
	  {
	  case  0: std::cout << "c operating mode.........: SAT (multi-threaded: portfolio)"                                                                    << std::endl; break; 
	  case  1: std::cout << "c operating mode.........: single-threaded, SATzilla-like"                                                                     << std::endl; break; 
	  case 10: std::cout << "c operating mode.........: MaxSAT, (multi-threaded: internal portfolio)"                                << std::endl; break; 
	  case 11: std::cout << "c operating mode.........: partialMode MaxSAT, (multi-threaded: internal portfolio"                      << std::endl; break; 
	  case 20: std::cout << "c operating mode.........: partial MaxSAT, (multi-threaded: internal portfolio)"                        << std::endl; break; 
	  case 21: std::cout << "c operating mode.........: partialMode partial MaxSAT, (multi-threaded: internal portfolio)"          << std::endl; break;
	  case 30: std::cout << "c operating mode.........: partial weighted MaxSAT, satisfiability-based (..:..)"	                        					<< std::endl; break;
	  case 31: std::cout << "c operating mode.........: partial weighted MaxSAT, naive Version, satisfiability-based (..:..)"	                        					<< std::endl; break;
	  case 99: std::cout << "c operating mode.........: collecting data for regression analysis (--> SATzilla-like SAT solving)"                            << std::endl; break; 
	  }
	std::cout << "c benchmark file.........: " << argv[(uint32_t) argc - 1] << std::endl;
	if (storeResult)
	  { std::cout << "c result file............: " << resultFile << std::endl; }

	std::cout << "c #threads...............: " << threads << std::endl; 
	if( cpuLimit > 0.0 )
	  {
		std::cout << "c CPU limit..............: " << cpuLimit << " s" << std::endl;
	  }
	if( memLimit > 0 )
	  {
		std::cout << "c Memory limit...........: " << memLimit << " MB" << std::endl;
	  }
	std::cout << "c ------------------------" << std::endl;
	std::cout << "c core options...........:" << std::endl;

	std::cout << "c restart strategy.......: ";
	if (restartStrategy == LUBY)
	  { std::cout << "luby" << std::endl; }
	else if (restartStrategy == GLUCOSE)
	  { std::cout << "glucose" << std::endl; }
	else
	  { std::cout << "undefined" << std::endl; }

	std::cout << "c restart unit factor....: " << unitFactor << std::endl
			  << "c decay factor...........: " << decayFactor << std::endl; 

	std::cout << "c simplify strategy......: ";
	if (simplifyStrategy == ANTOM)
	  { std::cout << "antom" << std::endl; }
	else if (simplifyStrategy == MINISAT)
	  { std::cout << "minisat" << std::endl; }
	else
	  { std::cout << "undefined" << std::endl; }

	std::cout << "c simplify activity......: ";
	if (simplifyActivity == SIMP_LBD)
	  { std::cout << "lbd" << std::endl; }
	else if (simplifyActivity == SIMP_CONF)
	  { std::cout << "conflicts" << std::endl; }
	else
	  { std::cout << "undefined" << std::endl; }

	std::cout << "c decision strategy......: ";
	switch (decisionStrategy)
	  {
	  case CACHEANDTOGGLE: std::cout << "using cached polarity together with antom's 'polarity toggling scheme'" << std::endl; break;
	  case CACHE: std::cout << "using cached polarity only" << std::endl; break;
	  case ALWAYSFALSE: std::cout << "setting polarity to FALSE regardless of the cached value" << std::endl; break;
	  case ALWAYSTRUE: std::cout << "setting polarity to TRUE regardless of the cached value" << std::endl; break;
	  }

	std::cout << "c use ternary clauses....: " << (ternary?"true":"false") << std::endl;
	std::cout << "c lhbr...................: " << (lhbr?"true":"false") << std::endl;

	std::cout << "c using preprocessing....: ";
	switch(preprocess)
	  {
	  case NONE: std::cout << "false" << std::endl; break;
	  case PREPROCESS: std::cout << "true" << std::endl; break;
	  case INCREMENTAL: std::cout << "incremental" << std::endl; break;
	  default: assert(false);
	  }
	std::cout << "c using inprocessing.....: " << (inprocess?"true":"false") << std::endl;
	if ( mode >= 10 )
	  {
		std::cout << "c using maxinprocessing..: " << (maxinprocess?"true":"false") << std::endl;
	  }
	if( (preprocess != NOPREPRO ) || inprocess || ((mode >= 10) && maxinprocess ) )
	  {
		std::cout << "c ------------------------" << std::endl;
		std::cout << "c preprocessing options..:" << std::endl;
		std::cout << "c max prepro loops.......: " << maxLoops << std::endl;
		std::cout << "c UPLA...................: " << (upla?"true":"false") << std::endl;
		std::cout << "c subsumption............: " << (subsumption?"true":"false") << std::endl;
		std::cout << "c variable elimination...: " << (varElim?"true":"false") << std::endl;
		std::cout << "c bce....................: " << (bce?"true":"false") << std::endl;
		std::cout << "c hte....................: " << (hte?"true":"false") << std::endl;
		std::cout << "c hse....................: " << (hse?"true":"false") << std::endl;
		std::cout << "c bva....................: " << (bva?"true":"false") << std::endl;
		std::cout << "c two literal diff.......: " << (bvatwolitdiff?"true":"false") << std::endl;
		std::cout << "c vivification...........: " << (vivify?"true":"false") << std::endl;
		std::cout << "c const SAT checks.......: ";
		switch ( satconst )
		  {
		  case 0: std::cout << "false" << std::endl; break;
		  case 1: std::cout << "true" << std::endl; break;
		  case 2: std::cout << "full" << std::endl; break;
		  }
	  }
  
	if ( mode >= 10 )
	  {
		std::cout << "c ------------------------" << std::endl;
		std::cout << "c MaxSAT options.........: " << std::endl;;
		std::cout << "c incomplete mode........: " << (incomplete?"true":"false") << std::endl;
		std::cout << "c seach mode.............: ";
		switch ( searchMode )
		  {
		  case UNSATBASED: std::cout << "Unsat-based" << std::endl; break;
		  case SATBASED: std::cout << "Sat-based" << std::endl; break;
		  case BINARYBASED: std::cout << "Binary-based" << std::endl; break;
		  }
		std::cout << "c network type...........: ";
		switch ( networktype )
		  {
		  case BITONIC: std::cout << "Bitonic sorter" << std::endl; break;
		  case ODDEVEN: std::cout << "Odd-Even sorter" << std::endl; break;
		  case TOTALIZER: std::cout << "Totalizer" << std::endl; break;
		  }
		std::cout << "c decision strategies....: ";
		switch ( decstrat )
		  {
		  case 0: std::cout << "Sorter outputs + relaxation + tare variables" << std::endl; break;
		  case 1: std::cout << "Sorter outputs" << std::endl; break;
		  case 2: std::cout << "None" << std::endl; break;
		  }
		std::cout << "c encode 01..............: " << (encode01?"true":"false") << std::endl;
		std::cout << "c sort soft clauses......: ";
		switch (sortsoftclauses )
		  {
		  case 0: std::cout << "None" << std::endl; break;
		  case 1: std::cout << "Most conflicting first" << std::endl; break;
		  case 2: std::cout << "Least conflicting first" << std::endl; break;
		  case 3: std::cout << "Random" << std::endl; break;
		  }
		std::cout << "c bypasses...............: ";
		switch (gridmode)
		  {
		  case 0: std::cout << "None" << std::endl; break;
		  case 1: std::cout << "Horizontal" << std::endl; break;
		  case 2: std::cout << "Vertical" << std::endl; break;
		  case 3: std::cout << "Horizontal and Vertical" << std::endl; break;
		  }
		if ( (gridmode == 2 ) || (gridmode ==3 ) )
		  {
			std::cout << "c bypass width...........: " << bypassWidth << std::endl;
		  }
		std::cout << "c find csc...............: ";
		switch ( csc )
		  {
		  case 0: std::cout << "false" << std::endl; break;
		  case 1: std::cout << "only for current sorter" << std::endl; break;
		  case 2: std::cout << "true" << std::endl; break;
		  }
		std::cout << "c skip comparators.......: " << (skip?"true":"false") << std::endl;

		std::cout << "c using partial mode.....: ";
		switch ( partialMode )
		  {
		  case NONE: std::cout << "None" << std::endl; break;
		  case DEPTHFIRST: std::cout << "Depth first search" << std::endl; break;
		  case BREADTHFIRST: std::cout << "Breadth first search" << std::endl; break;
		  }

		if ( partialMode != NONE )
		  {
			if( maxWidth != 0 )
			  {
				std::cout << "c max splitting width..: " << maxWidth << std::endl;
			  }
			else
			  {
				std::cout << "c fixed splitting width: " << width << std::endl;
			  }

			if( maxParts != 0 )
			  {
				std::cout << "c max splitting parts.: " << maxParts << std::endl;
			  }
			std::cout << "c use fixed relax lits..: " << (relaxation?"true":"false") << std::endl;
		  }
		if( mode == 30 )
		  {
			std::cout << "c base...................: " << base << std::endl;
		  }
		if (target >= 0)
		  {
			std::cout << "c defined target optimum.: " << target << std::endl;
			std::cout << "c find precise target....: " << preciseTarget << std::endl;
		  }
	  }
  }

  void Parser::PrintUsage(void) const
  {
	// Ouput. 
	std::cout << "c usage: ./antom --mode=<0..99> [--result-file=<file>] [options] <wcnf/cnf>"                                        << std::endl
			  << "c"                                                                                                                  << std::endl
			  << "c mode = 0 --> SAT (multi-threaded: portfolio)"                                                                     << std::endl
			  << "c mode = 1 --> SAT (single-threaded, SATzilla-like)"                                                                << std::endl
			  << "c mode = 10 --> MaxSAT (multi-threaded: internal portfolio)"                                                        << std::endl
			  << "c mode = 20 --> partial MaxSAT (multi-threaded: internal portfolio)"                                                << std::endl
			  << "c mode = 30 --> weighted partial MaxSAT (multi-threaded: internal portfolio)"                                       << std::endl
			  << "c mode = 31 --> naiv weighted partial MaxSAT (multi-threaded: internal portfolio)"                                  << std::endl
			  << "c mode = 99 --> collecting data for regression analysis (--> SATzilla-like SAT solving)"                            << std::endl
			  << "c"                                                                                                                  << std::endl
			  << "c general options:"                                                                                                 << std::endl
			  << "c --threads=<1..8>           --> number of threads for multithreaded mode (default 1)"                              << std::endl 
			  << "c --v | --verbose            --> increase verbosity"                                                                << std::endl
			  << "c --verbose=0                --> reset verbosity"                                                                   << std::endl
			  << "c --cpuLimit=[>=0.0]         --> CPU limit in seconds"                                                              << std::endl
			  << "c                                0.0: no CPU limit at all (default)"                                                << std::endl
			  << "c --memLimit=[>=0]           --> Memory limit in MB"                                                                << std::endl
			  << "c                                0: no memory limit at all (default)"                                               << std::endl
			  << "c --verify=<true/false>      --> verifies model in SAT-case with a second antom-instance"                           << std::endl

			  << "c solver option:"                                                                                                   << std::endl
			  << "c --restart=<luby/glucose>   --> sets the restart strategy to either Luby or Glucose (default: Luby)"               << std::endl
			  << "c --unitFactor=<value>       --> sets the unit factor of both restart strategies to 'value' (default: 8)"           << std::endl
			  << "c --decayFactor=<value>      --> sets the decay factor (variable activities) to 'value' (default: 1.05)"            << std::endl
			  << "c --decision=<0..3>          --> sets the decision strategy to mode 'value' (default: 0)"                           << std::endl
			  << "c                                0: use the cached polarity together with antom's 'polarity toggling scheme'"       << std::endl
			  << "c                                1: use the cached polarity only"                                                   << std::endl
			  << "c                                2: the polarity will be set to FALSE regardless of the cached value"               << std::endl
			  << "c                                3: the polarity will be set to TRUE regardless of the cached value"                << std::endl
			  << "c --ternary=<true/false>     --> enables/disables special treatment for ternary clauses' (default: false)"          << std::endl
			  << "c --lhbr=<true/false>        --> enables/disables 'Lazy Hyper Binary Resolution' (default: true)"                   << std::endl

			  << "c preprocessor options:"                                                                                            << std::endl
			  << "c --prepro=<true/false>      --> enables/disables preprocessor (default: false)"                                    << std::endl
			  << "c --inpro=<true/false>       --> enables/disables inprocessor during solve (default: false)"                        << std::endl
			  << "c --maxInpro=<true/false>    --> enables/disables inprocessor during maxsolve (default: false)"                     << std::endl
			  << "c --maxloops=<value>         --> sets the maximum number of preprocessing main loops (default: 5)"                  << std::endl
			  << "c --upla=<true/false>        --> enables/disables 'UPLA' during pre/inprocessing (default: true)"                   << std::endl
			  << "c --subsumption=<true/false> --> enables/disables full subsumption check during pre/inprocessing (default: true)"   << std::endl
			  << "c --varElim=<true/false>     --> enables/disables variable elimination during pre/inprocessing (default: true)"     << std::endl
			  << "c --bce=<true/false>         --> enables/disables 'BCE' during pre/inprocessing (default: true)"                    << std::endl
			  << "c --bva=<true/false>         --> enables/disables 'BVA' during pre/inprocessing (default: true)"                    << std::endl
			  << "c --2litdiff=<true/false>    --> enables/disables two literals difference for 'BVA' (default: false)"               << std::endl
			  << "c --vivify=<true/false>      --> enables/disables 'Vivification' during pre/inprocessing (default: true)"           << std::endl
			  << "c --satconst=<0..2>          --> enables/disables constant checks with SAT (default: 0)"                            << std::endl
			  << "c                                0: false"                                                                          << std::endl
			  << "c                                1: true (only deduction)"                                                          << std::endl
			  << "c                                2: full (with solver calls)"                                                       << std::endl 

			  << "c maxSAT options:"                                                                                                  << std::endl
			  << "c --incomplete=<true/false>  --> enables/disables incomplete mode (default: false)"                                 << std::endl 
			  << "c --search=<0..2>            --> search mode (default: 1)"                                                          << std::endl
			  << "c                                0: Unsat-based"                                                                    << std::endl
			  << "c                                1: Sat-based"                                                                      << std::endl
			  << "c                                2: Binary-based"                                                                   << std::endl 
			  << "c --network=<0..3>           --> network type (default: 0)"                                                         << std::endl
			  << "c                                0: Bitonic Sorter"                                                                 << std::endl
			  << "c                                1: Odd-Even-Sorter"                                                                << std::endl
			  << "c                                2: __currently unused__"                                                           << std::endl 
			  << "c                                3: Totalizer"                                                                      << std::endl 
			  << "c --decstrat=<0..2>          --> special decision strategy for MaxSAT related variables (default: 2)"               << std::endl
			  << "c                                0: For sorter outputs + relaxation + tare variables"                               << std::endl
			  << "c                                1: For sorter outputs"                                                             << std::endl
			  << "c                                2: none"                                                                           << std::endl
			  << "c --encode01=<true/false>    --> encode complete 01-comparators or just half (defualt:true)"                        << std::endl
			  << "c --sortsoft=<value>         --> sort soft clause parts for partialMode (default: 0)"                               << std::endl
			  << "c                                0: no sorting"                                                                     << std::endl
			  << "c                                1: sort soft clauses due to the larger number of conflicting soft clauses"         << std::endl
			  << "c                                2: sort soft clauses due to the smaller number of conflicting soft clauses"        << std::endl 
			  << "c                                3: random sort"                                                                    << std::endl
			  << "c --gridMode=<value>         --> activates bypass grid (default: 0)"                                                << std::endl 
			  << "c                                0: no grid"                                                                        << std::endl
			  << "c                                1: horizontal bypasses"                                                            << std::endl
			  << "c                                2: vertical bypasses"                                                              << std::endl
			  << "c                                3: bypass grid (horizontal+vertical)"                                              << std::endl
			  << "c --bypassWidth=<value>      --> sets width of horizontal grid (default: 4)"                                        << std::endl 
			  << "c --csc=<true/false/all>     --> enables/disables 'conflicting soft clauses' (default: false)"                      << std::endl 
			  << "c --skip=<true/false>        --> enables/disables skipping of comperators (default: false)"                         << std::endl 

			  << "c partial maxSAT options:"                                                                                          << std::endl
			  << "c --partial=<value>          --> partial mode (default: 0)"                                                         << std::endl
			  << "c                                0: none"                                                                           << std::endl
			  << "c                                1: Depth-first"                                                                    << std::endl
			  << "c                                2: Breadth-first"                                                                  << std::endl 
			  << "c --splitWidth=<value>       --> sets splitting width for partialMode (default: 32)"                                << std::endl 
			  << "c --maxWidth=<value>         --> sets maximum splitting width for partialMode (default: 0 = none)"                  << std::endl 
			  << "c --maxParts=<value>         --> sets maximum number of splitting parts for partialMode (default: 0 = none)"        << std::endl 
			  << "c --relax=<true/false>       --> enables/disables setting of relaxation literals (default: false)"                  << std::endl
			  << "c --targetOpt=<value>        --> solve maxSAT instance until target optimum is reached"                             << std::endl
			  << "c --preciseTarget=<value>    --> try to find target as close as possible. Must be used together wird 'targetOpt'"   << std::endl
			  << "c weighted maxSAT options:"                                                                                         << std::endl
			  << "c --base=<value>             --> base for sorter buckets (default: 2)"                                              << std::endl 
			  << "s UNKNOWN"                                                                                                          << std::endl;
  }
}
