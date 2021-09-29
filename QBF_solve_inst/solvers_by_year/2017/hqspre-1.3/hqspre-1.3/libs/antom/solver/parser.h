/********************************************************************************************
parser.hpp -- Copyright (c) 2013-2016, Tobias Schubert, Sven Reimer

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

#ifndef ANTOM_PARSER_HPP
#define ANTOM_PARSER_HPP

#include "antom.h"

namespace antom 
{
  struct Parser 
  {

  public:

	Parser(void);
	~Parser(void) {};

	uint32_t ParseCommandline(int argc, char** argv);

	void SetSettings(antom::Antom& myAntom) const;
  
	bool LoadCNF(const std::string& file, uint32_t& maxIndexOrigVars, Antom& antom);
  
	void PrintSettings(int argc, char** argv) const;
	void PrintUsage(void) const;


	// general
	bool storeResult; 
	std::string resultFile;
	uint32_t mode;
	uint32_t threads; 
	uint32_t verbose;
	double cpuLimit;
	uint32_t memLimit;
	bool verify;
	SolverType solver;

	// core 
	RestartStrategy restartStrategy; 
	uint32_t unitFactor; 
	double decayFactor; 
	SimplifyStrategy simplifyStrategy;
	SimplifyActivity simplifyActivity;
	DecisionStrategy decisionStrategy; 
	bool ternary;
	bool lhbr; 

	// prepro 
	PreproType preprocess;
	bool inprocess;
	bool maxinprocess;
	uint32_t maxLoops;
	bool upla;
	bool subsumption;
	bool varElim;
	bool bce;
	bool hte;
	bool hse;
	bool bva;
	bool bvatwolitdiff;
	bool vivify;
	uint32_t satconst;
  
	// maxsat
	bool incomplete;
	SearchMode searchMode;
	SorterType networktype;
	uint32_t decstrat;
	bool encode01;
	uint32_t sortsoftclauses;
	uint32_t gridmode;
	uint32_t bypassWidth;
	uint32_t csc;
	bool skip;
	int32_t target;
	bool preciseTarget;
  
	// partial maxsat
	PartialMode partialMode;
	uint32_t width;
	uint32_t maxWidth;
	uint32_t maxParts;
	bool relaxation;

	// weighted maxsat
	uint32_t base;
  };
}

#endif
