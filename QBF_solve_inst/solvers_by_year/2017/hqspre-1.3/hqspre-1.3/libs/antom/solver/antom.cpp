
/********************************************************************************************
antom.cpp -- Copyright (c) 2014-2016, Sven Reimer

dPermission is hereby granted, free of charge, to any person obtaining a copy of this 
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

#include <stdlib.h>
#include <cmath>

// Include antom related headers.
#include "antom.h"
#include "sorter.h"
#include "control.h"
#include "preprocessor.h"
#include "core.h"
//#include "solverproxy.h"

namespace antom
{
  Antom::Antom(uint32_t threads) : 
	AntomBase(threads),
	_softClauses(),
	_sorterTree(),
	_bestModel(),
	_maxSorterDepth(0),
	_searchMode(SATBASED),
	_maxsatResult(ANTOM_UNKNOWN),
   	_partialMode(NONE),
	_lastpartialsorter(false),
	_splittedWidth(32),
	_maxWidth(0),
	_maxParts(0),
	_horizontalwidth(4),
	_bypassGrid(0),
	_horizontalbypasses(0),
	_verticalbypasses(0),
	_sortsoftclauses(0),
	_comparator(0),
	_skipped(0),
	_setCSC(0),
	_doskipping(false),
	_setRelaxationLits(false),
	_incompleteMode(false),
	_maxInprocess(false),
	_satconst(0),
	_networkType(BITONIC),
	_encode01Mode(true), 
	_triggervar(0),
	_lastIndex(0),
	_low(0),
	_high(0),
	_target(-1),
	_preciseTarget(false),
	_currentBucketForTare(0),
	_baseMode(2),
	_sumOfSoftWeights(0),
	_satWeight(0),
	_highestBucketMultiplicator(0),
	_currentBucketMultiplicator(0), 
	_estimatedSatWeight(0), 
	_diffToSatWeight(0), 
	_collectedAssumptions(), 
	_decStratMode(0)
  {
	_sorterTree.resize(1);
  }

  Antom::~Antom(void) 
  {
	for (uint32_t i = 0; i != _sorterTree.size(); ++i)
	  {
		for (uint32_t j = 0; j != _sorterTree[i].size(); ++j)
		  {
			delete _sorterTree[i][j];
		  }
	  }
	for (uint32_t i = 0; i != _softClauses.size(); ++i)
	  {
		delete _softClauses[i];
	  }
  }

  void Antom::InstanceReset(void)
  {
	AntomBase::InstanceReset();
	DataReset();
  }
  
  void Antom::DataReset(void)
  {
	for (uint32_t i = 0; i != _softClauses.size(); ++i)
	  {
		delete _softClauses[i];
	  }
	
	_softClauses.clear();
	for (uint32_t i = 0; i != _sorterTree.size(); ++i)
	  {
		for (uint32_t j = 0; j != _sorterTree[i].size(); ++j)
		  {
			delete _sorterTree[i][j];
		  }
	  }
	_sorterTree.clear();
	_sorterTree.resize(1);

	_bestModel.clear();

	_maxSorterDepth = 0;
   	_maxsatResult = ANTOM_UNKNOWN;
	_lastpartialsorter = false;

	_horizontalbypasses = 0;
	_verticalbypasses = 0;	
	_comparator = 0;
	_skipped = 0;

	_triggervar = 0;
	_lastIndex = 0;
	_low = 0;
	_high = 0;
	_target = -1;

	_currentBucketForTare = 0;
	_sumOfSoftWeights = 0;
	_satWeight = 0;
	_highestBucketMultiplicator = 0;
	_currentBucketMultiplicator = 0;
	_estimatedSatWeight = 0;
	_diffToSatWeight = 0;
	_decStratMode = 0;

	_collectedAssumptions.clear();
  }

  // Resets SAT and MaxSAT related data
  void Antom::Reset(void)
  {
	AntomBase::Reset();
	DataReset();

	// TODO: add further status resets
	_satconst = 0;
	_networkType = BITONIC;
  }

  void Antom::SetMaxBounds(uint32_t low, uint32_t high)
  {
	assert( low <= high );
	_low = low;
	_high = high;
  }

  void Antom::SetLowerBound(uint32_t low)
  {
	_low = low;
  }

  void Antom::SetHigherBound(uint32_t high)
  {
	_high = high;
  }

  void Antom::SetOptTarget(int32_t target)
  {
	_target = target;
  }

  void Antom::SetPreciseTarget(bool val)
  {
	_preciseTarget = val;
  }

  void Antom::SetSearchMode(SearchMode val)
  { 
	_searchMode = val;
  }

  void Antom::SetPartialMode(PartialMode val)
  { 
	_partialMode = val;
  }
  
  // Returns the last used index. Does not necessary meet with "variables()"
  uint32_t Antom::LastIndex(void) const 
  { return _lastIndex; }
	  
  // Add a clause to the soft clause database
  bool Antom::AddSoftClause(std::vector<uint32_t>& clause, uint32_t weight)
  {
	std::vector < uint32_t > sclause( clause.size() );
	uint32_t i = 0;
	for( ; i < clause.size(); ++i )
	  { 
        sclause[i] = clause[i]; 
		SetDontTouch(clause[i]>>1); 
	  }

	if( _control->GetIncrementalMode() > 0 )
	  {
		// In incremental mode every softclause is triggered by a global assumption
		if ( _globalPropertyTrigger[_stacksize] == 0 )
		  { 
			_globalPropertyTrigger[_stacksize] = NewVariable(); 
		  }
		clause.push_back(_globalPropertyTrigger[_stacksize]<<1);
	  }

	uint32_t trigger( NewVariable() );
	SetDontTouch(trigger);
	clause.push_back(trigger<<1);


	// Create a new variable which is added to each clause in sorter network
	// Used in incremental mode where the sorter network and all conflict clauses resulting
	// from the network has to be deleted after every step
	SoftClause* sclaus = new SoftClause (trigger<<1, sclause, weight);
	_softClauses.push_back( sclaus );

	_sumOfSoftWeights += weight;

	return AddClause(clause);
  }

  // Proceed all soft clauses
  // Count positive and negative occurences of all variables in soft clauses
  // Set decision strategy for the variables such that the polarity with more occurences is prefered
  // If there is no larger occurence value for a polarity, set the decision strategy to "0"
  // If "pos" is false the polarity with less occurences is prefered
  void Antom::SetStrategyforSoftVariables(bool pos)
  {
	std::vector< uint32_t > occur( ((_lastIndex<<1)+2), 0 );

	for( uint32_t i = 0; i != _softClauses.size(); ++i )
	  {

		std::vector <uint32_t > lits( _softClauses[i]->clause);
		for ( uint32_t pos = 0; pos != lits.size(); ++pos )
		  { ++occur[lits[pos]]; }
	  }

	for( uint32_t v = 1; v <= _lastIndex; ++v )
	  {
		uint32_t poslit( v<<1 );
		uint32_t neglit( (v<<1)^1 );
		if( occur[poslit] > occur[neglit] ) 
		  {
			if( pos )
			  { SetDecisionStrategyForVariable(3,v); }
			else
			  { SetDecisionStrategyForVariable(2,v); }
		  }
		else if ( occur[poslit] < occur[neglit] )
		  {
			if( pos )
			  { SetDecisionStrategyForVariable(2,v); }
			else
			  { SetDecisionStrategyForVariable(3,v); }
		  }
		else 
		  {
			SetDecisionStrategyForVariable(0,v);
		  }
	  }
  }

  void Antom::SetIncrementalMode(uint32_t val)
  { _control->SetIncrementalMode(val); }

  // De-/activates constant detection with SAT
  void Antom::SetSatConst(uint32_t val)
  {
	_satconst = val; 
  }

  // Check for constants in the candidates list by applying the SAT solver
  // Attention! Use with care, process might be costly
  // If quickcheck is set, only assumptions are deduced, instead of solving the instance
  // -> less powerful, but much faster
  bool Antom::FindConstantsWithSat(std::vector< uint32_t >& candidates, bool quickcheck)
  {
	assert( _satconst > 0);
	std::cout << __func__ << "currently not fully implemented" << std::endl;
	// Need to distinguish between lastmodel and bestmodel for maxsat 
	assert(false);
	exit(0);

	if( _control->GetVerbosity() > 2 )
	  { std::cout << "c constant check with SAT..." << std::endl; }

	std::vector< uint32_t > seenvalues(Variables()+1,0);
	std::vector< DecisionStrategy > tempstrategy(Variables()+1,CACHEANDTOGGLE);
	std::vector< bool > checkvariable(Variables()+1,true);

	TrivialAssignment();
	// Fill variables to proceed with current variables
	uint32_t w = 0;
	for( uint32_t i = 0; i != candidates.size(); ++i )
	  {
		assert( candidates[i] != 0 );
		assert( candidates[i] < Model().size() );

		if( Model()[candidates[i]] == 0 )
		  {
			candidates[w] = candidates[i];
			++w;
		  }
		tempstrategy[candidates[i]] = _core[_sID]->_modeDSForVariables[candidates[i]];
	  }
	candidates.resize(w);
	
	uint32_t satcalls(0);
	std::vector< uint32_t > assumptions;

	bool quickresult(true);
	uint32_t result = Solve(assumptions);
	++satcalls;

	if( result != ANTOM_SAT )
	  {
		// TODO: handle timeout and unsat case
		assert(false);
	  }

	_lastModel = Model();

	if( _control->GetApplication() == WEIGHTEDMAXSAT )
	  {
		uint64_t satWeight = CountSatisfiedSoftClauses(nullptr,_lastModel);
		if( satWeight > _satWeight )
		  { _satWeight = satWeight; }
	  }

	// add initial model
	for( uint32_t i = 0; i != candidates.size(); ++i )
	  {
		assert( _lastModel[candidates[i]] != 0 );
		seenvalues[candidates[i]] = _lastModel[candidates[i]];
		// force solver to take opposite polarity
		// 2: always false, 3: always true
		DecisionStrategy strategy( (_lastModel[candidates[i]]&1)?ALWAYSTRUE:ALWAYSFALSE);
		SetDecisionStrategyForVariable(strategy, candidates[i]);
	  }

	// Now proceed all candidates

	uint32_t constants(0);
	uint32_t size( candidates.size());
	for( uint32_t i = 0; i != size; ++i )
	  {
		uint32_t nextvar( candidates[i] );
		if ( !checkvariable[nextvar] )
		  { continue; }
		bool polarity = !(seenvalues[nextvar]&1);

		assert( assumptions.empty());
		// push opposite of last seen value as assumption
		assumptions.push_back( (nextvar << 1)^polarity);

		if( quickcheck )
		  {
			quickresult = DeduceAssumptions(assumptions);
		  }
		else
		  {
			result = Solve(assumptions);
		  }
		++satcalls;

		assumptions.pop_back();

		// "nextvar" is constant
		if ( (result == ANTOM_UNSAT) || !quickresult  )
		  {

			TrivialAssignment();
			_lastModel = Model();

			if ( _lastModel[nextvar] == 0 ) 
			  {
				++constants;
				SetDecisionStrategyForVariable(tempstrategy[nextvar],nextvar);
				if ( !AddUnit( (nextvar << 1)^!polarity ) )
				  { return false; }
			  }
			
			for( uint32_t j = i+1; j < size; ++j )
			  {
				uint32_t checkvar( candidates[j] );
				// remove already assigned variables from list
				if( _lastModel[checkvar] != 0 )
				  {
					SetDecisionStrategyForVariable(tempstrategy[checkvar],checkvar);
					candidates[j--] = candidates[--size];
					candidates.pop_back();
				  }
			  }
		  }
		else if ( (result == ANTOM_SAT) || quickcheck )
		  {
			assert( quickresult );
			_lastModel = Model();

			if( _control->GetApplication() == WEIGHTEDMAXSAT )
			  {
				uint64_t satWeight = CountSatisfiedSoftClauses(nullptr,_lastModel);
				if( satWeight > _satWeight )
				  { _satWeight = satWeight; }
			  }

			// remove candidates which cannot be constant
			for ( uint32_t j = i; j != size; ++j )
			  {
				uint32_t checkvar( candidates[j] );
				uint32_t seenvalue( _lastModel[checkvar] );
				uint32_t lastseenvalue( seenvalues[checkvar] );
				assert( quickcheck || (seenvalue != 0 ) );
				
				if( (seenvalue != 0) && (lastseenvalue != seenvalue) )
				  {
					SetDecisionStrategyForVariable(tempstrategy[checkvar],checkvar);
					checkvariable[checkvar] = false;
				  }
			  }
		  }
		else 
		  {
			assert( false );
		  }
	  }

	// redo decision strategies
	for( uint32_t i = 0; i != candidates.size(); ++i )
	  {
		_core[_sID]->SetDecisionStrategyForVariable(tempstrategy[candidates[i]],candidates[i]);
	  }

	_core[_sID]->_statistics.constantVariablesBySAT += constants;
	if( _control->GetVerbosity() > 2 )
	  {
		std::cout << "c found " << constants << " constants with ";
		if( quickcheck )
		  {
			std::cout << "1 SAT-solver call and " << (satcalls-1) << " assumption deductions ";
		  }
		else
		  {
			std::cout << satcalls << " SAT-solver calls ";
		  }
		std::cout << "(checking " << w << " variables)" << std::endl;
	  }

	// Do not propagate units...
	return true;
  }

  uint32_t Antom::MaxSolveLocal(int32_t& optimum)
  {
	std::vector< uint32_t > externalAssumptions;
	return MaxSolveLocal( externalAssumptions, optimum  ); 
  }
  
  // Solves the current (partial) MaxSAT formula. Returns SAT/UNSAT and modifies "optimum", representing the minimum number of 
  // unsatisfied clauses. The modes of operation are:
  // mode = 0 --> unsatisfiability-based (multi-threaded: internal portfolio),
  // mode = 1 --> satisfiability-based (multi-threaded: internal portfolio).
  // mode = 2 --> binary search (multi-threaded: internal portfolio).
  uint32_t Antom::MaxSolveLocal(const std::vector< uint32_t >& externalAssumptions, int32_t& optimum)
  {
	if( _core[_sID]->EmptyClause() )
	  { return ANTOM_UNSAT; }

	struct rusage resources;
	getrusage(RUSAGE_SELF, &resources); 
	double timeS  = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
	_control->SetStartTime(timeS);
	_control->SetSumTime(true);

	// Initialize triggervar
	if( _triggervar == 0 )
	  {
		_triggervar = NewVariable();
		SetDontTouch(_triggervar);
		AddUnit(_triggervar<<1);
	  }

	// There should be no additional assumption in non-incremental mode
	// exception: incomplete and partial mode
	assert( externalAssumptions.empty() || ( _control->GetIncrementalMode() != 0 ) || (_partialMode != NONE)) ;

	if( _control->GetIncrementalMode() > 0 )
	  {
		for( uint32_t i = 0; i != externalAssumptions.size(); ++i )
		  { 
			SetDontTouch(externalAssumptions[i]>>1); 
		  }
	  }

	uint32_t numberofsoftclauses((uint32_t)_softClauses.size());

	_collectedAssumptions = externalAssumptions;

	// No softclauses... nothing to do
	if( numberofsoftclauses == 0 )
	  { 
		uint32_t res = Solve(_collectedAssumptions); 
		_control->SetSumTime(false);
		optimum = 0;
		return res;
	  }

	if ( (_partialMode==NONE) && _control->GetVerbosity() > 1 )
	  { 
		std::cout << "c softclauses: " << numberofsoftclauses << std::endl
				  << "c hardclauses: " << (Clauses()-numberofsoftclauses) << std::endl;
	  }

	// Build sorter if not in partial mode 
	// Sorters are built seperatly in partialmode
	if( _partialMode==NONE )
	  {

		// Do preprocessing?
		if( _dopreprocessing != NOPREPRO )
		  {
			// (Incrementally) preprocess formula without sorter
			if ( Preprocess( PREPROCESS ) != ANTOM_UNKNOWN )
			  { 
				if( _control->GetIncrementalMode() > 0 )
				  { 
					InvalidateSoftClauses(); 
				  }
				_control->SetSumTime(false);
				return ANTOM_UNSAT; 
			  }

			if ( _control->GetTimeOutReached() || _control->GetMemOutReached() )
			  {
				ExitTimeout();
				return ANTOM_UNKNOWN;
			  }
		  }

		// Add the sorting network to the clause database. 
		// Collect trigger clauses of soft variables
		assert( _sorterTree[0].empty() );
		
		Sorter* sorter = new Sorter((uint32_t)_softClauses.size(), this);
		
		for( uint32_t i = 0; i != _softClauses.size(); ++i )
		  { 
			sorter->AddSoftClauseToSorter(_softClauses[i]);
		  }
		
		_sorterTree[0].push_back(sorter);
		
		CreateNextSorter();
	  }

	SetDecisionStrategiesForMaxSAT();

	if( _dopreprocessing == INCREMENTAL )
	  {
		// (Incrementally) preprocess formula including sorter
		if ( Preprocess( INCREMENTAL ) != ANTOM_UNKNOWN )
		  { 
			if( _control->GetIncrementalMode() > 0 )
			  { 
				InvalidateSoftClauses(); 
			  }
			_control->SetSumTime(false);
			return ANTOM_UNSAT; 
		  }

		if ( _control->GetTimeOutReached() || _control->GetMemOutReached() )
		  {
			ExitTimeout();
			return ANTOM_UNKNOWN;
		  }
	  }

	Sorter* sorter = GetNextSorter();
	uint32_t sortersize = sorter->size();

	// Handle user given bounds
	if( ( _low > 0 ) && _low <= sortersize )
	  {
		uint32_t lowerlit((sorter->GetOutputs()[_low-1]<<1)^1);
		bool rst = AddBound(lowerlit);
		
		if( !rst )
		  {
			_control->SetSumTime(false);
			return ANTOM_UNSAT;
		  }
	  }
	if( ( _high > 0 ) && ( _high <= sortersize ) )
	  {
		uint32_t higherlit(sorter->GetOutputs()[sortersize-_high]<<1);

#ifndef NDEBUG
		bool rst = AddBound(higherlit); assert( rst );
#else
		AddBound(higherlit);
#endif
	  }

	if( _control->GetVerbosity() > 3 )
	  {
		TrivialAssignment();
		sorter->Print();
	  }

	// Activate property in incremental mode
	if( ( _control->GetIncrementalMode() > 0 ) )
	  { 
		for( uint32_t i = 0; i <= _stacksize; ++ i )
		  {
			assert( _globalPropertyTrigger[i] != 0 );
			_collectedAssumptions.push_back( (_globalPropertyTrigger[i]<<1)^1 ); 
		  }
	  }

    // Initialization.
    uint32_t result(ANTOM_UNKNOWN);
    uint32_t pos(0); 

	// What about "mode"?
    switch (_searchMode)
      {
	
		// UNSAT based
      case UNSATBASED: 

		result = UnsatBasedSearch( sorter, pos );
		break;
	
		// SAT-based 
      case SATBASED:

		result = SatBasedSearch( sorter, pos );
		break;

		// Binary search
	  case BINARYBASED:
		
		result = BinaryBasedSearch( sorter, pos );
		break;
	  }

	// Add final unit in incomplete mode
	if (_incompleteMode && pos > 0)
	  {
#ifndef NDEBUG
		bool rst = AddUnit((sorter->GetOutputs()[pos-1] << 1) ^ 1); assert(rst);
#else
		AddUnit((sorter->GetOutputs()[pos-1] << 1) ^ 1);
#endif		
	  }

	// found a valid solution for precise target
	// now calculate value as close as possible to target
	if (_partialMode == NONE && _preciseTarget)
	  {
		int32_t gap = _target - static_cast<int>(sorter->Weight() - pos);
		assert(gap >= 0);
		result = GetPreciseTarget(sorter, pos, gap);
	  }

	optimum = sorter->Weight() - pos;
	if( _control->GetIncrementalMode() == 1 )
	  { 
		InvalidateSoftClauses(); 
	  }

	// Remind lower and upper bounds for this sorter
	// Needed for partial modes
	if ( (uint32_t)optimum < _softClauses.size() )
	  {
		sorter->SetMinSatisfied(sorter->NumberOfSoftclauses() - pos);
	  }

	// Reset collected assumptions for incremental mode
	_collectedAssumptions = externalAssumptions;

	// Return "result".
	_control->SetSumTime(false);

	if (result != ANTOM_UNKNOWN)
	  {
		sorter->SetProceeded(true);
	  }

	if (_partialMode == NONE)
	  {
		_control->SetExtendedResult(_maxsatResult);
	  }

    return result;
  }

  // sat based search for the optimal position, where outputs[pos] is sat and outputs[pos+1] is unsat
  uint32_t Antom::SatBasedSearch(Sorter* sorter, uint32_t& pos)
  {
	// Initialization.
	uint32_t result = ANTOM_UNSAT;
	uint32_t currentresult = ANTOM_UNKNOWN;

	struct rusage resources;

#ifndef NDEBUG
	uint32_t lastpos = 0;
#endif
	// Initialize lastmodel
	if (_partialMode == NONE)
	  {
		_lastModel = Model();
	  }
	
	uint32_t maxopt = sorter->Weight();
	
	// Solve the MaxSAT problem, using a satisfiability-based approach.  
	while (true)
	  {

		if( _maxInprocess && (_partialMode==NONE) )
		  {
			// Do inprocessing
			currentresult = Preprocess(INPROCESS);
		
			// Have we found the optimum?
			if (currentresult != ANTOM_UNKNOWN)
			  { break; }
		  }
		
		// Solve the CNF.
		currentresult = Solve(_collectedAssumptions);

		// Have we found the optimum?
		if (currentresult == ANTOM_UNSAT)
		  {
			if (_partialMode == NONE)
			  {
				_core[_sID]->SetModel(_lastModel);
				_maxsatResult = ANTOM_SAT;
			  }
			break;
		  }
		// Reached timeout?
		else if ( currentresult == ANTOM_UNKNOWN )
		  {
			if (_partialMode == NONE && _maxsatResult == ANTOM_UNKNOWN_WITH_PRE_RESULT)
			  {
				_core[_sID]->SetModel(_lastModel);
			  }
			return ANTOM_UNKNOWN;
		  }
		else
		  { 
			result = currentresult; 
		  }

		_lastModel = Model();

		// Try to satisfy more soft clauses by chance.
		pos = CountSatisfiedSoftClauses( sorter, _lastModel );
		_maxsatResult = ANTOM_UNKNOWN_WITH_PRE_RESULT;

#ifndef NDEBUG
		CheckGates();
#endif

#ifndef NDEBUG
		assert( lastpos < pos || pos == 0 );
		lastpos = pos;
#endif

		assert(pos <= maxopt);
		uint32_t localoptimum = maxopt-pos;
		// Output our current best result. 
		if ((_control->GetVerbosity() > 0) && (_lastpartialsorter || _partialMode==NONE))
		  {
			
			std::cout << "o " << localoptimum << std::endl; 
			if( _control->GetVerbosity()>1)
			  {
				getrusage(RUSAGE_SELF, &resources); 
				double timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
				std::cout << "c " << (timeC-_control->GetStartTime()) << "s" <<std::endl;
			  }
		  }

		// Have we found the very special case with all relaxation variables set to FALSE?
		if (localoptimum == 0)
		  { break; }

		// We found our demanded target => return result
		if (_target >= static_cast<int>(localoptimum))
		  {
			if ((_partialMode == NONE) )
			  {
				result = ANTOM_UNKNOWN;
				break;
			  }
			
			else
			  {
				unsigned int globalopt = UpdateSorterOptimum();
				if ( _target >= static_cast<int>(_sumOfSoftWeights - globalopt))
				  {
					break;
				  }
			  }
			
		  }

		uint32_t litbound( (sorter->GetOutputs()[pos] << 1) ^ 1 );

		if ( !AddBound(litbound) )
		  {
			_core[_sID]->SetModel(_lastModel);
			break;
		  }
	  }
	
	if( _control->GetIncrementalMode() == 1 )
	  { 
		InvalidateSoftClauses(); 
	  }

	// Return "result".
	return result; 
  }

  // unsat based search for the optimal position, where outputs[pos] is sat and outputs[pos+1] is unsat
  uint32_t Antom::UnsatBasedSearch(Sorter* sorter, uint32_t& pos)
  {
	// Initialization. 
	uint32_t currentresult = ANTOM_UNSAT; 
	uint32_t maxopt = sorter->Weight();

	pos = maxopt;

	if( _low > 0 )
	  { pos = maxopt-_low-1;  }

	// Now, solve the MaxSAT problem, using a unsatisfiability-based approach.  
	while (true)
	  {
		// Decrement "pos".
		--pos; 

		if( _maxInprocess && (_partialMode==NONE) )
		  {
			currentresult = Preprocess(INPROCESS);
		    
			// Have we found the optimum?
			if (currentresult == ANTOM_UNSAT)
			  { break; }
		  }

		// Output.
		if (_control->GetVerbosity() > 0) 
		  { std::cout << "c checking o = " << (maxopt - pos - 1) << "..." << std::endl; }

		_collectedAssumptions.push_back( (sorter->GetOutputs()[pos]<<1)^1 );

		// Solve the CNF, taking our assumption into account. 
		currentresult = Solve(_collectedAssumptions); 
		
		// Have we found the optimum?
		if (currentresult == ANTOM_SAT)
		  { 
			// Update "optimum".				
			break; 
		  }
		// Reached timeout?
		else if (currentresult == ANTOM_UNKNOWN)
		  {
			ExitTimeout();
			return ANTOM_UNKNOWN;
		  }

		if (( _control->GetIncrementalMode() > 0 ) || ( (_partialMode!=NONE) && !_incompleteMode ))
		  {
			assert( !_collectedAssumptions.empty() );
			_collectedAssumptions[_collectedAssumptions.size()-1] ^= 1;
		  }
		else 
		  {
			uint32_t assumption( _collectedAssumptions.back() );
			// Remove from collected assumption
			_collectedAssumptions.pop_back();
			// Flip the current assumption and add it as a unit clause to the clause database.
			if (!AddUnit(assumption^1) )
			  { assert(currentresult == ANTOM_UNSAT); break; }
		  }
		
		// Did we reached the last element?
		if( pos == 0 )
		  { 
			assert( currentresult == ANTOM_SAT );
			return currentresult; 
		  }
	  }
	// set pos to the last unsatisfiable value
	++pos;
	return currentresult;
  }

  uint32_t Antom::BinaryBasedSearch(Sorter* sorter, uint32_t& pos)
  {
	uint32_t maxopt = sorter->Weight();

	uint32_t min = 0;
	uint32_t max = maxopt;

	struct rusage resources;

	// Initialization.
	uint32_t currentresult = Solve(_collectedAssumptions);

	// Have we found the optimum?
	if (currentresult == ANTOM_UNSAT)
	  { 
		min=max; 
		_control->SetSumTime(false); 
		return ANTOM_UNSAT; 
	  }
	// Reached timeout?
	else if ( currentresult == ANTOM_UNKNOWN )
	  {
		ExitTimeout();
		return ANTOM_UNKNOWN;
	  }

	// a lazy quick and dirty way to preserve the best model
	_lastModel = Model();

	min = CountSatisfiedSoftClauses( sorter, _lastModel );

	uint32_t t = maxopt;
	bool res = true;
	// search for upper bound

	_collectedAssumptions.push_back( (sorter->GetOutputs()[maxopt-1]<<1)^1 );
	do {
	  --t;
	  _collectedAssumptions.pop_back();
	  _collectedAssumptions.push_back( (sorter->GetOutputs()[t]<<1)^1 );
	  res = DeduceAssumptions(_collectedAssumptions);
	} while ( !res );

	_collectedAssumptions.pop_back();
	max = t+1;

	pos = (min+max-1) >> 1;	

	// Output our current best result. 
	if ( _control->GetVerbosity() > 0 )
	  {
		std::cout << "o " << (maxopt-min) << std::endl; 
		if ( _control->GetVerbosity() > 1 )
		  { 
			getrusage(RUSAGE_SELF, &resources); 
			double timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
			std::cout << "c " << (timeC-_control->GetStartTime()) << "s" <<std::endl;
			std::cout << "c known bounds: (" << (maxopt-max) << " / " << (maxopt-min) << ")" << std::endl;
		  }
	  }
		

	if ( min > 0 )
	  { 
		if ( !AddBound( (sorter->GetOutputs()[min-1]<<1)^1 ) )
		  {
			max=min; 
		  }
	  }
	if ( max < maxopt )
	  {
#ifndef NDEBUG
		bool rst = AddBound(sorter->GetOutputs()[max]<<1);
		assert( rst );
#else
		AddBound(sorter->GetOutputs()[max]<<1);
#endif
	  }

	while ( min < max )
	  {
		if ( _control->GetVerbosity() > 1 )
		  { std::cout << "c TRY with " << (maxopt-pos-1) << std::endl; }

		_collectedAssumptions.push_back( (sorter->GetOutputs()[pos]<<1)^1);

		currentresult = Solve(_collectedAssumptions);

		if ( currentresult == ANTOM_SAT) // SAT
		  {
			_lastModel = Model();
			if ( _control->GetVerbosity() > 1 )
			  { std::cout << "c Found with " << (maxopt-pos-1) << std::endl; }
				
			uint32_t nxtmin = CountSatisfiedSoftClauses(sorter, _lastModel);
			assert( nxtmin > min );
			min = nxtmin;
				
			// Output our current best result. 
			if ( _control->GetVerbosity() > 0 )
			  { 
				std::cout << "o " << (maxopt-min) << std::endl; 
				if ( _control->GetVerbosity() > 1)
				  {
					getrusage(RUSAGE_SELF, &resources); 
					double timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
					std::cout << "c " << (timeC-_control->GetStartTime()) << "s" <<std::endl;
				  }
			  }
		  }
		else if ( currentresult == ANTOM_UNSAT ) // UNSAT
		  {
			if ( _control->GetVerbosity() > 1 )
			  { std::cout << "c Failed with " << (maxopt-pos-1) << std::endl; }
				
			max = pos;

			assert( !_collectedAssumptions.empty() );				
			_collectedAssumptions[_collectedAssumptions.size()-1] ^= 1;

		  }
		else if ( currentresult == ANTOM_UNKNOWN ) // Reached timeout?
		  {
			ExitTimeout();
			if( _control->GetIncrementalMode() > 0 )
			  { 
				for( uint32_t i = 0; i != _collectedAssumptions.size(); ++i )
				  { SetDontTouch((_collectedAssumptions[i]>>1),false); }
			  }
			return ANTOM_UNKNOWN;
		  }
		else
		  { assert(false); }
			
		if ( ( _control->GetIncrementalMode() == 0 ) && ( (_partialMode== NONE) || _incompleteMode ) )
		  {
#ifndef NDEBUG
			bool rst(false); rst = AddUnit( _collectedAssumptions.back() ); assert( rst );
#else
			AddUnit( _collectedAssumptions.back() );
#endif
			_collectedAssumptions.pop_back();
		  }

		// Do inprocessing
		if( _maxInprocess && (_partialMode==0)  )
		  {
			currentresult = Preprocess(INPROCESS);
				
			// instance unsatisfiable with added assumption?
			// then the currentoptimum is the best result
			if( currentresult != ANTOM_UNKNOWN )
			  { break; }
		  }
		
		pos = (min+max-1) >> 1;
		  
		if ( _control->GetVerbosity() > 1 )
		  { std::cout << "c known bounds: (" << (maxopt-max) << " / " << (maxopt-min) << ")" << std::endl; }

	  }

	if ( maxopt>pos )
	  { 
		std::vector< uint32_t > model_assumption;
		model_assumption.reserve(_lastIndex+1);
		// solve with the best found model, so that the antom-model is set correctly 
		// just add the model for the original clauses. 
		// Due to our "by chance" method the virtual best model is not complete
		for ( uint32_t m = 1; m <= _lastIndex; ++m )
		  {
			if( _lastModel[m] != 0 )
			  { model_assumption.push_back(_lastModel[m]);}
		  }

		// this is the quick and dirty way! TODO
		currentresult = Solve(model_assumption);

		assert( currentresult == ANTOM_SAT );
	  }
	++pos;
	return currentresult;
  }

  uint32_t Antom::GetPreciseTarget(Sorter* sorter, uint32_t& pos, int32_t gap)
  {
	// TODO: not really working yet
	// Check how to avoid by chance soft clauses
	assert(false);
	exit(0);
	if (gap==0)
	  {
		return ANTOM_SAT;
	  }

	// forbid last optimum
	uint32_t currentResult(ANTOM_UNKNOWN);
	int32_t lastGap(gap);
	do 
	  {
		if (pos == 0)
		  {
			break;
		  }
		uint32_t lit = sorter->GetOutputs()[pos-1]<<1;
		std::cout << "add unit " << helper::Lit(lit) << std::endl;		  
#ifndef NDEBUG
		bool rst = AddUnit(lit); assert(rst);
#else
		AddUnit(lit);
#endif
		lastGap = gap;
		currentResult = Solve();

		std::cout << "currentresult: " << currentResult << std::endl;
		// Timeout Result
		if (currentResult == ANTOM_UNKNOWN)
		  {
			_core[_sID]->SetModel(_lastModel);
			_maxsatResult = ANTOM_UNKNOWN_WITH_PRE_RESULT;
			return ANTOM_UNKNOWN;
		  }
		// There may be no solution less than found optimum -> we are done 
		else if(currentResult == ANTOM_UNSAT)
		  {
			break;
		  }
		_lastModel = Model();
		pos = CountSatisfiedSoftClauses(sorter, _lastModel);
		std::cout << "pos: " << pos << std::endl;
		gap = _target - static_cast<int>(sorter->Weight() - pos);
		std::cout << "lastgap: " << lastGap << " newgap: " << gap << std::endl;
	  }
	while (gap > 0);

	// readjust model if the lastGap is closer than the last result
	if (lastGap < -gap)
	  {
		_core[_sID]->SetModel(_lastModel);
	  }

	_maxsatResult = ANTOM_SAT;
	
	return ANTOM_SAT;
  }

  // Returns false if added bounded lead to unsat
  bool Antom::AddBound(uint32_t lit)
  {
	if( ( _control->GetIncrementalMode() > 0 ) || ( !_lastpartialsorter && (_partialMode!=NONE) ) )
	  {
		_collectedAssumptions.push_back( lit ); 
	  }
	else 
	  {
		assert(!_incompleteMode || _lastpartialsorter);
		// if the resulting problem is unsatisfied, we have found our optimum!
		if (!AddUnit( lit ) )
		  {
			return false; 
		  }
	  }
	return true;
  }


  uint32_t Antom::MaxSolve(int32_t& optimum)
  {
	std::vector< uint32_t > externalAssumptions;
	return MaxSolve( externalAssumptions, optimum );
  }

  uint32_t Antom::MaxSolve(const std::vector< uint32_t >& externalAssumptions, int32_t& optimum)
  {
	_control->SetApplication(MAXSAT);
	assert(!_preciseTarget || _target >= 0);
	assert(!_preciseTarget || _searchMode == SATBASED);
	
	if( _partialMode == NONE)
	  {
		return MaxSolveLocal(externalAssumptions, optimum);
	  }

	// Consistency checks.
	assert(!_incompleteMode || ( _searchMode == SATBASED ));

	if( _core[_sID]->EmptyClause() )
	  { 
		if( _control->GetIncrementalMode() > 0 )
		  { 
			InvalidateSoftClauses(); 
		  }
		return ANTOM_UNSAT; 
	  }

	struct rusage resources;
	getrusage(RUSAGE_SELF, &resources); 
	double timeS  = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
	_control->SetStartTime(timeS);
	_control->SetSumTime(true);

	// Set incrementalmode to "incremental + partial"
	if( _control->GetIncrementalMode() == 1 )
	  { _control->SetIncrementalMode(2); }

	for( uint32_t i = 0; i != externalAssumptions.size(); ++i )
	  { SetDontTouch(externalAssumptions[i]>>1); }

	uint32_t numberofsoftclauses((uint32_t)_softClauses.size());

	if( numberofsoftclauses == 0 )
	  { 
		uint32_t res = Solve(externalAssumptions);
		_control->SetSumTime(false);
		if( _control->GetIncrementalMode() > 0 )
		  { 
			InvalidateSoftClauses(); 
		  }
		return res;
	  }

	// Do preprocessing?
	if( _dopreprocessing != NOPREPRO )
	  {
		// (Incrementally) preprocess formula without sorter
		if ( Preprocess( PREPROCESS ) != ANTOM_UNKNOWN )
		  { 
			if( _control->GetIncrementalMode() > 0 )
			  { 
				InvalidateSoftClauses(); 
			  }
			_control->SetSumTime(false);
			return ANTOM_UNSAT; 
		  }

		if ( _control->GetTimeOutReached() || _control->GetMemOutReached() )
		  {
			ExitTimeout();
			return ANTOM_UNKNOWN;
		  }
	  }

	uint32_t result(ANTOM_UNKNOWN);
	uint32_t depth(0);
	int32_t currentoptimum(_sumOfSoftWeights);
	optimum = currentoptimum+1;

	if( _triggervar == 0 )
	  {
		_triggervar = NewVariable();
		SetDontTouch(_triggervar);
		AddUnit(_triggervar<<1);
	  }

	if ( _control->GetVerbosity() > 1 )
	  { 
		std::cout << "c softclauses: " << numberofsoftclauses << std::endl
				  << "c hardclauses: " << (Clauses()-numberofsoftclauses) << std::endl;
	  }

	// calc and set all trivially conflicting softclauses
	CheckAllConflictingSoftclauses();
	// Constant check
	CalcSplitWidth();

	uint32_t prepart(0);
	// build datastructure for parts
	while( prepart*_splittedWidth < numberofsoftclauses )
	  {
		uint32_t s ( prepart*_splittedWidth );
		uint32_t maxsize = s+_splittedWidth;

		if( numberofsoftclauses < maxsize )
		  { maxsize = numberofsoftclauses; }
		
		Sorter* sorter = new Sorter(_splittedWidth, this);

		for( ; s < maxsize; ++s )
		  { 
			sorter->AddSoftClauseToSorter(_softClauses[s]);
		  }
		_sorterTree[0].push_back(sorter);
		++prepart;
	  }

	if( _lastIndex == 0 )
	  {
		_lastIndex = Variables();
	  }

	// Add all original variables as candidates
	std::vector< uint32_t > satconstcandidates(_lastIndex,0);
	for( uint32_t v = 1; v <= _lastIndex; ++v )
	  { satconstcandidates[v-1] = v; }
	  
	if ( _control->GetVerbosity() > 1 )
	  { std::cout << "c start solving depth " << depth << " width: " << _splittedWidth << std::endl; }

	Sorter* currentsorter = nullptr;
	uint32_t currentresult(ANTOM_UNKNOWN);
	
	do
	  {
		currentsorter = MergeNextSorter();
		currentsorter->SetProceedNext(true);

		// Reached last sorter -> we can treat this "unpartially"
		if ( _sorterTree[_maxSorterDepth][0]->NumberOfSoftclauses() ==  numberofsoftclauses )
		  {
			_lastpartialsorter = true;
		  }

		std::vector< uint32_t > localassumptions = externalAssumptions;
		CollectRelaxationLits( localassumptions );

		// Add lower bounds of satisfied soft clauses as assumption for this run
		uint32_t minsat = SetLowerPartialParts(localassumptions);

		if ( minsat > (_softClauses.size()-currentsorter->NumberOfSoftclauses()) )
		  {
			uint32_t pos = minsat-(_softClauses.size()-currentsorter->NumberOfSoftclauses());

			uint32_t lit((currentsorter->GetOutputs()[pos-1]<<1)^1);
#ifndef NDEBUG
			bool rst = AddUnit(lit); assert(rst);
#else
			AddUnit(lit);
#endif			
		  }

		// solve (merged) instance 
		int32_t partialoptimum = -1;

		if( _control->GetVerbosity() > 1 )
		  {
			uint32_t sum(0);
			for( uint32_t i = 0; i != _sorterTree.size(); ++i )
			  {
				std::cout << "c " << i << ": ";
				for( uint32_t j = 0; j != _sorterTree[i].size(); ++j )
				  {
					std::cout << _sorterTree[i][j]->NumberOfSoftclauses() << ", ";
					sum += (uint32_t)_sorterTree[i][j]->NumberOfSoftclauses();
				  }
				std::cout << " (" << sum << ")" << std::endl;
			  }
			assert( sum <= _softClauses.size() );
		  }

		
		if( ( _satconst > 0 ) && (currentsorter->NumberOfSoftclauses() < numberofsoftclauses ) )
		  { 
#ifndef NDEBUG
			bool rst = FindConstantsWithSat(satconstcandidates, _satconst==1); assert(rst);
#else
			FindConstantsWithSat(satconstcandidates, true);
#endif
		  }

		if( _maxInprocess )
		  { currentresult = Preprocess(INPROCESS); }
		else
		  { currentresult = ANTOM_UNKNOWN; }

		currentresult = MaxSolveLocal( localassumptions, partialoptimum );
		_control->SetSumTime(true);

		if( currentresult == ANTOM_SAT )
		  {
			result = ANTOM_SAT;
			_maxsatResult = ANTOM_UNKNOWN_WITH_PRE_RESULT;

			currentoptimum = _sumOfSoftWeights - UpdateSorterOptimum();
			assert( currentoptimum >= 0 );

			if( currentoptimum < optimum )
			  {
				_bestModel = _lastModel;
				optimum = currentoptimum;
				if ( _control->GetVerbosity() > 0 )
				  { 
					std::cout << "o " << optimum << std::endl; 
					if( _control->GetVerbosity() > 1 )
					  {
						getrusage(RUSAGE_SELF, &resources); 
						double timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
						std::cout << "c " << (timeC-timeS) << "s" << std::endl;
					  }
				  }
			  }

			if (_control->GetVerbosity() > 1)
			  { std::cout << "c partial optimum: " << currentoptimum << std::endl; } 
		  }
		else if ( currentresult == ANTOM_UNSAT )
		  {
			_maxsatResult = ANTOM_UNSAT;
			_control->SetExtendedResult(ANTOM_UNSAT);
			_control->SetSumTime(false);
			if ( _control->GetIncrementalMode() > 0 )
			  { 
				InvalidateSoftClauses(); 
			  }
			return ANTOM_UNSAT;
		  }
		// Timeout
		else if ( currentresult == ANTOM_UNKNOWN )
		  {
			if (_maxsatResult == ANTOM_UNKNOWN_WITH_PRE_RESULT)
			  {
				// bestmodel might not have the correct size,
				// in this case lastmodel is also the best model
				if(_bestModel.size() < _lastModel.size() )
				  {
					_core[_sID]->SetModel(_lastModel);
				  }
				else
				  {	  
					_core[_sID]->SetModel(_bestModel);
				  }
			  }
			
			_control->SetExtendedResult(_maxsatResult);
			ExitTimeout();
			return ANTOM_UNKNOWN;				
		  }

		if ( _incompleteMode && _sorterTree[0].empty() )
		  { break; }

		// We have found our demanded optimum
		if (_target >= static_cast<int>(optimum))
		  {
			assert(_maxsatResult == ANTOM_UNKNOWN_WITH_PRE_RESULT);
			_core[_sID]->SetModel(_bestModel);
			_control->SetSumTime(false);
			if( _control->GetIncrementalMode() > 0 )
			  { InvalidateSoftClauses(); }
			_control->SetExtendedResult(_maxsatResult);
			return ANTOM_UNKNOWN;
		  }

		currentsorter->SetProceedNext(false);
		assert( !_sorterTree[_maxSorterDepth].empty() );
		// Do until one part is left and all parts are considered
	  } while ( currentsorter->NumberOfSoftclauses() < numberofsoftclauses );

	if ( _control->GetVerbosity() > 1 )
	  {
		std::cout << "c comparator: " << _comparator << " skipped: " << _skipped << std::endl;
	  }

	_core[_sID]->SetModel(_bestModel);
	_control->SetSumTime(false);
	if( _control->GetIncrementalMode() > 0 )
	  { InvalidateSoftClauses(); }

	_maxsatResult = ANTOM_SAT;
	_control->SetExtendedResult(_maxsatResult);
	return result;
  }

  #if 0
  void Antom::SetSolverProxy(SolverType type)
  {
	assert(_solver == nullptr);
	if (type == ANTOMSOLVER)
	  {
		_solver = new AntomSolverProxy(this);
	  }
	else
	  {
		assert(type == CRYPTOMINISATSOLVER);
		_solver = new AntomSolverProxy(this);
		//_solver = new CryptoMiniSatSolverProxy(this);
	  }
  }

  SolverProxy* Antom::GetSolverProxy(void) const
  {
	return _solver;
  }
  #endif

  uint32_t Antom::SetLowerPartialParts(std::vector< uint32_t >& localassumptions) const
  {
	uint32_t sum(0);
	// proceed current considered trigger parts
	for( uint32_t i = 1; i != _sorterTree.size(); ++i )
	  {
		for( uint32_t j = 0; j != _sorterTree[i].size(); ++j )
		  {
			uint32_t minsat( _sorterTree[i][j]->GetMinSatisfied() );
			if( minsat > 0 )
			  {
				uint32_t lit( (_sorterTree[i][j]->GetOutputs()[minsat-1]<<1)^1 );
				localassumptions.push_back(lit);
				sum += minsat;
			  }
		  }
	  }
	assert( sum <= _softClauses.size() );
	return sum;
  }

  int32_t Antom::UpdateSorterOptimum(void)
  {
	int32_t result(0);
	// proceed current considered trigger parts
	for( uint32_t i = 0; i != _sorterTree.size(); ++i )
	  {
		for( uint32_t j = 0; j != _sorterTree[i].size(); ++j )
		  {
			uint32_t pos = CountSatisfiedSoftClauses( _sorterTree[i][j], _lastModel );
			result += pos;
		  }
	  }
	return result;
  }

  // If sorter == nullptr, we want to count for all soft clauses
  uint32_t Antom::CountSatisfiedSoftClauses(Sorter* sorter, const std::vector<uint32_t>& model)
  {

	std::vector< SoftClause* > softclauses;
	if( sorter == nullptr )
	  {
		softclauses = _softClauses;
	  }
	else
	  {
		softclauses = sorter->GetSoftClauses();
	  }

	//std::cout << __func__ << std::endl;
	uint32_t result(0);
	// Proceed all soft clauses
	for( uint32_t i = 0; i != softclauses.size(); ++i )
	  {
		uint32_t relaxlit = softclauses[i]->relaxationLit;
		
		//std::cout << helper::Lit(relaxlit) << " " << helper::Lit(model[relaxlit>>1]);
		// Just proceed satisfied triggers
		if( model[relaxlit>>1] == relaxlit )
		  {
			std::vector< uint32_t> clause( softclauses[i]->clause );
			uint32_t pos = 0;
			for(; pos != clause.size(); ++pos )
			  {
				// clause satisfied without trigger?
				if( model[clause[pos]>>1] == clause[pos] )
				  {
					result += softclauses[i]->weight;
					softclauses[i]->lastassignment = relaxlit^1;
					//std::cout << "++" << result;
					break;
				  }
			  }
			if ( pos == clause.size() )
			  {
				softclauses[i]->lastassignment = relaxlit;
			  }
		  }
		else if ( model[relaxlit>>1] != 0 )
		  { 
			assert( model[relaxlit>>1] == (relaxlit^1));
			softclauses[i]->lastassignment = relaxlit^1;
			result += softclauses[i]->weight;
			//std::cout << " ++relax sat " << result;
		  }
		//std::cout << std::endl;
	  }

	if( sorter != nullptr )
	  {
		sorter->SetMinSatisfied(result);
	  }
	return result;
  }

  void Antom::CalcSplitWidth(void)
  {
	uint32_t parts(1);
	uint32_t splitwidth((uint32_t)_softClauses.size());

	if( _maxWidth != 0 )
	  {
		// First get largest power of 2 number below "_maxWidth"
		uint32_t factor( 1 );
		while ( ( factor * _maxWidth ) < splitwidth )
		  { factor <<= 1; }

		// If there is any left over split into "(largest power of 2 number) - 1 + rest" parts
		_splittedWidth = splitwidth/factor;

		if( (splitwidth%factor) != 0 )
		  { ++_splittedWidth; }
	  }

	if ( _maxParts != 0 )
	  {
		while ( parts > _maxParts )
		  { splitwidth = splitwidth<<1; parts = parts>>1; }
		_splittedWidth = splitwidth;
	  }
  }
  
  void Antom::SetSplittedWidth(uint32_t width)
  { assert( width > 0 ); _splittedWidth = width; }

  void Antom::SetMaxWidth(uint32_t width)
  { _maxWidth = width; }

  void Antom::SetMaxParts(uint32_t parts)
  { _maxParts = parts; }

  void Antom::SetSortSoftClauses(uint32_t val)
  { assert( val < 4 ); _sortsoftclauses = val; }

  void Antom::SetSkip(bool val) 
  { _doskipping = val; }
  void Antom::SetCSC(uint32_t val) 
  { _setCSC = val; }
  void Antom::SetRelaxationLits(bool val) 
  { _setRelaxationLits = val; }
  void Antom::SetIncompleteMode(bool val) 
  { 
	_incompleteMode = val; 
	if ( val )
	  { _partialMode = BREADTHFIRST; }
  }

  void Antom::SetGridMode(uint32_t val)
  { assert( val < 4 ); _bypassGrid = val; }
  void Antom::SetHorizontalWidth(uint32_t val)
  { assert( val > 0 ); _horizontalwidth = val; }

  void Antom::SetNetworktype(SorterType val) 
  { 
	_networkType = val;
  }

  void Antom::SetMaxInprocessing(bool val)
  { _maxInprocess = val; }

  void Antom::CheckAllConflictingSoftclauses(void) 
  {
	if( _setCSC != 2 )
	  { return; }

	size_t size = _softClauses.size();

	if( _control->GetVerbosity() > 1 )
	  { std::cout << "c " << __func__ << " " << size << std::endl; }

	uint32_t bypasses(0);
	uint32_t unsats(0);

	for( size_t i = 0; i != size; ++i )
	  {
		std::vector< uint32_t > assumptions;
		uint32_t lit( _softClauses[i]->relaxationLit );
		assumptions.push_back( lit^1 );

		// Deduce activation of current soft clause
		bool rst = DeduceAssumptions(assumptions);
		_softClauses[i]->contra = 0;

		// Instance is not solveable if current soft clause is activated
		if( !rst )
		  {
			rst = AddUnit( lit ); assert( rst );
			++unsats;
		  }			
		else
		  {
			const std::vector< uint32_t >& solvermodel = Model();

			// Check model
			for( uint32_t j = 0; j < size; ++j )
			  {
				uint32_t checklit( _softClauses[j]->relaxationLit );
				assert( (checklit>>1) < solvermodel.size() );
				// Contradiction... j'th soft clause is deactivated if i'th soft clause is activated
				if( solvermodel[checklit>>1] == checklit )
				  {
					++bypasses;
					++_softClauses[i]->contra;
					std::vector < uint32_t > clause( 2, 0 );
					clause[0] = lit;
					clause[1] = checklit;
					rst = AddClause(clause); assert(rst);
				  }
				// The i'th and j'th soft clause can be activated at once
				else if( ( i != j ) && ( solvermodel[checklit>>1] == (checklit^1) ) )
				  {
					++bypasses;
					std::vector < uint32_t > clause( 2, 0 );
					clause[0] = lit;
					clause[1] = (checklit)^1;
					rst = AddClause(clause); assert(rst);
				  }
			  }
		  }
	  }

	if( _control->GetVerbosity() > 1 )
	  { 
		std::cout << "c find overall " << bypasses << " bypasses and " << unsats << " unsats" << std::endl; }
	
	if( _sortsoftclauses == 1 )
	  { std::sort( _softClauses.begin(), _softClauses.end(), SoftClauseSorter() ); }
	else if( _sortsoftclauses == 2 )
	  {	std::sort( _softClauses.rbegin(), _softClauses.rend(), SoftClauseSorter() ); }
	else if ( _sortsoftclauses == 3 )
	  {	std::random_shuffle(_softClauses.begin(),_softClauses.end());  }
  }


  // Adds last assignment of soft triggers to "assumptions"
  void Antom::CollectRelaxationLits(std::vector< uint32_t >& assumptions)
  {
	if( !_setRelaxationLits )
	  { return; }

	// Add last assignment of soft triggers to assumptions

	// proceed proceeded parts
	for( uint32_t i = 0; i < _sorterTree.size(); ++i )
	  {
		for(uint32_t j = 0; j != _sorterTree[i].size(); ++j )
		  {
			Sorter* proceedSorter = _sorterTree[i][j];
			// Skip current sorter
			if( proceedSorter->GetProceedNext())
			  { continue; }

			for( uint32_t j = 0; j != proceedSorter->NumberOfSoftclauses(); ++j )
			  {
				SoftClause* sclause = proceedSorter->GetSoftClauses()[j];
				if ( sclause->lastassignment != 0 )
				  {
					assumptions.push_back( sclause->lastassignment );
				  }
				else
				  {
					assumptions.push_back( sclause->relaxationLit ); 
				  }
			  }
		  }
	  }
  }


  // Deletes the soft clause datastructure and all clauses containing these clauses
  void Antom::InvalidateSoftClauses(void)
  {
	assert( _control->GetIncrementalMode() > 0 );

	// Set triggers to true, i.e. invalid softclauses
	for ( uint32_t i = 0; i != _softClauses.size(); ++i )
	  {
		assert( _lastIndex < (_softClauses[i]->relaxationLit>>1) );

		// Remove "don't touch" status for soft clause variables
		for( uint32_t pos = 0; pos != _softClauses[i]->clause.size(); ++pos )
		  { SetDontTouch(_softClauses[i]->clause[pos]>>1, false); }
	  }

	// clear soft clause data structure
	_softClauses.clear();
	_sorterTree.clear();

	assert( _lastIndex < _globalPropertyTrigger[_stacksize] );
	// reset trigger
	_globalPropertyTrigger[_stacksize] = 0;

	// Delete variables and clauses from last time step
	ClearVariables(_lastIndex+1, _core[_sID]->Variables());

	// Reset max variable index to "_lastIndex", the datastructure capacity is conserved
	SetMaxIndex(_lastIndex);

	for (uint32_t t = 0; t < _threads; ++t) 
	  { 
		_preprocessor[t]->SetPreVarIndex(_lastIndex+1); 
		_preprocessor[t]->SetPreClauseIndex(); 
	  }

	// Checks, whether all clauses with variable index > "_lastIndex" are deleted
	assert( _core[_sID]->CheckMaxIndex(_lastIndex) );
  }

  // creates sorter for depth
  Sorter* Antom::CreateNextSorter(uint32_t depth, uint32_t targetdepth)
  {
  	assert( !_sorterTree[depth].empty() );
  	// clear model
  	TrivialAssignment();

	Sorter* sorter = _sorterTree[depth].back();
	_sorterTree[depth].pop_back();	

  	sorter->EncodeSorter(targetdepth);

	if( _maxSorterDepth < targetdepth )
	  { 
		_maxSorterDepth = targetdepth;
		_sorterTree.resize(targetdepth+1);
	  }
	_sorterTree[targetdepth].push_back( sorter );

	if( _control->GetVerbosity() > 1 )
	  {
		std::cout << "c comparator: " << _comparator << " skipped: " << _skipped << std::endl;
		std::cout << "c horizontal bypasses: " << _horizontalbypasses << " vertical bypasses: " << _verticalbypasses << std::endl;
	  }
	return _sorterTree[targetdepth].back();
  }

  Sorter* Antom::MergeNextSorter(void)
  {
	// Go throw sorters, starting with highest depth
	// If two sorters of the same depth are available -> merge them
	assert( _sorterTree.size() > _maxSorterDepth );
	uint32_t depth = _maxSorterDepth;
	uint32_t depth2 = 0;
	switch( _partialMode )
	  {
	  case DEPTHFIRST :
		for( ; depth > 0; --depth )
		  {
			if( _sorterTree[depth].size() > 1 )
			  {
				break;
			  }
		  }

		break;
	  case BREADTHFIRST :
		depth = 0;
		for( ; depth <= _maxSorterDepth; ++depth )
		  {
			if( _sorterTree[depth].size() > 0 )
			  {
				break;
			  }
		  }
		
		break;
	  default:
		assert(false);
		break;
	  }

	// All sorters are built, no merge on higher levels possible
	// -> We have to merge over different levels
	if( (depth == 0 && _sorterTree[0].empty()) || ( (depth > 0) && _sorterTree[depth].size() == 1 ) )
	  {
		for( ; depth <= _maxSorterDepth; ++depth )
		  {
			if( _sorterTree[depth].size() > 0 )
			  {
				if( depth2 != 0 )
				  { break; }
				depth2 = depth;
			  }
		  }
	  }
	else
	  { 
		depth2 = depth;
	  }

	// Reched level 0 
	// -> We have to create the next sorter for level 1
	if ( depth == 0 )
	  {
		return CreateNextSorter();
	  }

	Sorter* tempSorter1 = _sorterTree[depth].back();
	_sorterTree[depth].pop_back();
	Sorter* tempSorter2 = _sorterTree[depth2].back();
	_sorterTree[depth2].pop_back();

	tempSorter2->MergeWithSorter(*tempSorter1);

	delete tempSorter1;

	// Pushed merged list on next level
	if( depth == _maxSorterDepth )
	  {
		++_maxSorterDepth;
		_sorterTree.resize(_maxSorterDepth+1);
	  }
	_sorterTree[depth+1].push_back(tempSorter2);
	return _sorterTree[depth+1].back();
  }

  Sorter* Antom::GetNextSorter(void)
  {
	switch( _partialMode ) 
	  {
	  case NONE :
		assert( _sorterTree[1].size() == 1 );
		assert( _maxSorterDepth == 1 );
		return _sorterTree[1].back();

		break;
	  case DEPTHFIRST :
		for (uint32_t i = 1; i != _sorterTree.size(); ++i )
		  {
			if( !_sorterTree[i].empty() )
			  {
				assert( !_sorterTree[i].back()->GetProceeded() );
				return _sorterTree[i].back();
			  }
		  }
		assert(false);
		break;
	  case BREADTHFIRST :
		for (uint32_t i = _maxSorterDepth; i > 0; --i )
		  {
			if( !_sorterTree[i].empty() )
			  {
				assert( !_sorterTree[i].back()->GetProceeded() );
				return _sorterTree[i].back();
			  }
		  }
		assert(false);
		break;
	  default : 
		std::cout << "c unknown partial mode " << _partialMode << " => exit" << std::endl;
		exit(0);
		break;
	  }
	return nullptr;
  }

  void Antom::ExitTimeout(void)
  {
	assert( (_control->GetCPULimit() > 0.0 ) || ( _control->GetMemoryLimit() > 0 ) || _target >= 0);

	if( _control->GetIncrementalMode() > 0 )
	  { 
		InvalidateSoftClauses(); 
	  }
	_control->SetSumTime(false);
  }

  void Antom::SetDecisionStrategiesForMaxSAT(void) 
  {
	if (_decStratMode <= 1)
	  {
		// set outputs of last sorter to first group
		assert( !_sorterTree.empty());
		if( !_sorterTree[_maxSorterDepth-1].empty() )
		  {
			assert( _sorterTree[_maxSorterDepth-1][0] != nullptr );
			for (uint32_t i = 0; i < _sorterTree[_maxSorterDepth-1][0]->size(); ++i)
			  {
				uint32_t outputvar( _sorterTree[_maxSorterDepth-1][0]->GetOutputs()[i] );
				SetDecisionStrategyForVariable(2,outputvar);
			  }
		  }

		if (_decStratMode == 0)
		  {
			// set all relaxationliterals to second group
			for (uint32_t i = 0; i < _softClauses.size(); ++i)
			  {
				uint32_t relaxvar(_softClauses[i]->relaxationLit >> 1);
				SetDecisionStrategyForVariable(2,relaxvar);
			  }

			// set tare variables to third group
			for (int32_t j = _maxSorterDepth - 2; j >= 0; --j)
			  {
				assert(_sorterTree[j][0] != nullptr );
				if ( _sorterTree[j].empty() )
				  { continue; }
								
				for( uint32_t k = 0; k != _sorterTree[j][0]->GetTares().size(); ++k )
				  {
					uint32_t tarevar(_sorterTree[j][0]->GetTares()[k]);
					SetDecisionStrategyForVariable(3,tarevar);
				  }
			  }
		  }
	  }
  }


  // Weigthed maxsat stuff
  // ____________________________________________________________________________________________________________________
  uint32_t Antom::MaxSolveWeightedPartial(int32_t& optimum)
  {
	std::vector<uint32_t> externalAssumptions;
	return MaxSolveWeightedPartial(externalAssumptions, optimum);
  }

  // ____________________________________________________________________________________________________________________
  uint32_t Antom::MaxSolveWeightedPartial(const std::vector<uint32_t>& externalAssumptions, int32_t& optimum)
  {
	_control->SetApplication(WEIGHTEDMAXSAT);
	uint32_t clausesBefore = Clauses();

	std::cout << "c #softclauses ..........: " << _softClauses.size() << std::endl << "c #hardclauses before....: " << (Clauses() - _softClauses.size()) << std::endl;

	struct rusage resources;
	getrusage(RUSAGE_SELF, &resources);
	double timeS = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
	double timeC = timeS;
	_control->SetStartTime(timeS);
	_control->SetSumTime(true);

	uint32_t actualBitPos(0);
	uint32_t actualRemainder(0);
	uint32_t averageBucketEntries(0);

	// Do preprocessing?
	if ( _dopreprocessing != NOPREPRO )
	  {
		// (Incrementally) preprocess formula without sorter
		if (Preprocess( PREPROCESS ) != ANTOM_UNKNOWN)
		  { 
			if( _control->GetIncrementalMode() > 0 )
			  { 
				InvalidateSoftClauses(); 
			  }
			_control->SetSumTime(false);
			return ANTOM_UNSAT; 
		  }

		if ( _control->GetTimeOutReached() || _control->GetMemOutReached() )
		  {
			ExitTimeout();
			return ANTOM_UNKNOWN;
		  }
	  }

	uint32_t actualQuotient(1);
	std::vector< uint32_t > satconstcandidates;

	for (uint32_t i = 0; i != _softClauses.size(); ++i)
	  {
		actualQuotient = _softClauses[i]->weight;
		while (actualQuotient > 0)
		  {
			//actualRemainder = actualQuotient % _baseMode;
			//actualQuotient = actualQuotient / _baseMode;
			div_t divresult = std::div( static_cast<int>(actualQuotient), static_cast<int>(_baseMode) );
			actualRemainder = divresult.rem;
			actualQuotient = divresult.quot;
			if (_maxSorterDepth == actualBitPos)
			  {
				++_maxSorterDepth;
				_sorterTree.resize(_maxSorterDepth);
			  }

			if( _sorterTree[actualBitPos].empty() )
			  {
				Sorter* sorter = new Sorter(0, this);
				_sorterTree[actualBitPos].push_back(sorter);
			  }

			//satconstcandidates.push_back(_softclauses[i]->relaxationLit >> 1);
			for (uint32_t j = 0; j < actualRemainder; j++)
			  {
				_sorterTree[actualBitPos][0]->AddOutput(_softClauses[i]->relaxationLit >> 1);
				_sorterTree[actualBitPos][0]->AddSoftClause(_softClauses[i]);
			  }
			++actualBitPos;
		  }
		actualBitPos = 0;
	  }

	if( _satconst > 0 )
	  { 

		for ( uint32_t v = 1; v <= Variables(); ++v )
		  {
			satconstcandidates.push_back(v);
		  }
#ifndef NDEBUG
		bool rst = FindConstantsWithSat(satconstcandidates, _satconst==1); assert(rst);
#else
		FindConstantsWithSat(satconstcandidates, _satconst==1);
#endif
	  }

	assert( _sorterTree.size() > 0 );
	// Last bucket has no tare
	_currentBucketForTare = _sorterTree.size()-1;

	// add at the last position _base many variables T to each trigger except the last one.
	for (uint32_t i = 0; i < _maxSorterDepth - 1; i++)
	  {
		assert( _sorterTree[i].size() <= 1 );
		if( !_sorterTree[i].empty() )
		  {
			for (uint32_t j = 0; j < _baseMode - 1; j++)
			  {
				uint32_t tare(NewVariable());
				SetDontTouch(tare);
				_sorterTree[i][0]->AddTare(tare);
			  }
		  }
	  }

	// do the polynomial watchdog unary coding & composition
	for (uint32_t i = 0; i < _maxSorterDepth; ++i)
	  {
		// encode all the buckets to unary representation
		CreateNextSorter(i,i);

		if( _control->ReachedLimits() )
		  {	
			ExitTimeout(); 
			return ANTOM_UNKNOWN;
		  }

		if ( i != 0 )
		  {
			std::vector<uint32_t> X = _sorterTree[i-1][0]->BaseRanks(_baseMode);
			assert(X.size() > 0 || _sorterTree[i][0]->size() > 0);
			_sorterTree[i][0]->MergeTotalizer(X);

			if( _control->ReachedLimits() )
			  {	
				ExitTimeout(); 
				return ANTOM_UNKNOWN;
			  }
		  }
	  }

	if (_control->GetVerbosity() > 1)
	  {
		std::cout << "the buckets: " << std::endl;
		for (uint32_t i = 0; i < _sorterTree.size(); ++i)
		  {
			assert( _sorterTree[i].size() == 1 );
			std::cout << i << " size: " << _sorterTree[i].size();
			std::cout << " entries: " << _sorterTree[i][0]->size() << std::endl;
			averageBucketEntries += _sorterTree[i][0]->size();
			//_sorterTree[i][0].print();
		  }
		std::cout << " numberof buckets: " << _currentBucketForTare << std::endl;
	  }

	averageBucketEntries = averageBucketEntries / _sorterTree.size();

	if( _dopreprocessing == INCREMENTAL )
	  {
		// (Incrementally) preprocess formula including sorter
		if (Preprocess( INCREMENTAL ) != ANTOM_UNKNOWN)
		  { 
			if( _control->GetIncrementalMode() > 0 )
			  { 
				InvalidateSoftClauses(); 
			  }
			_control->SetSumTime(false);
			return ANTOM_UNSAT; 
		  }
		if ( _control->GetTimeOutReached() || _control->GetMemOutReached() )
		  {
			ExitTimeout();
			return ANTOM_UNKNOWN;
		  }
	  }

	SetDecisionStrategiesForMaxSAT();

	if (_control->GetVerbosity() > 0)
	  {
		getrusage(RUSAGE_SELF, &resources);
		timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
		std::cout << "c #hardclauses after.....: " << (Clauses() - _softClauses.size()) << std::endl;
		std::cout << "c #hardclauses of coding.: " << Clauses() - clausesBefore << std::endl;
		std::cout << "c encoding time..........: " << (timeC - _control->GetStartTime()) << "s" << std::endl;
		std::cout << "c #verticalbypasses......: " << _verticalbypasses << std::endl;
		std::cout << "c #buckets...............: " << _sorterTree.size() << std::endl;
		std::cout << "c #average bucket entries: " << averageBucketEntries << std::endl;

		if (_control->GetVerbosity() > 1)
		  {
			std::cout << std::endl << "optimum: " << optimum << std::endl;
		  }
	  }

	// actual tare T position of trigger
	_collectedAssumptions = externalAssumptions;
	// assign highest possible value to optimum
	optimum = ~0;

	_highestBucketMultiplicator = std::pow(_baseMode, _maxSorterDepth-1);
	_currentBucketMultiplicator = _highestBucketMultiplicator;

	_sumOfSoftWeights = 0;
	for (uint32_t var = 0; var < _softClauses.size(); ++var)
	  {
		_sumOfSoftWeights += _softClauses[var]->weight;
	  }

	// Start by proceeding last bucket for rough estimation
	uint32_t currentresult = ProceedLastBucket(optimum);

	if (currentresult == ANTOM_UNSAT )
	  { return ANTOM_UNSAT; }
	else if ( currentresult == ANTOM_UNKNOWN )
	  { 
		assert(_control->GetCPULimit() > 0.0 || _control->GetMemoryLimit() > 0);

		if (_control->GetIncrementalMode() > 0)
		  {
			InvalidateSoftClauses();
		  }

		if (_control->GetVerbosity() > 1)
		  {
			getrusage(RUSAGE_SELF, &resources);
			timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
			std::cout << "c " << (timeC - _control->GetStartTime()) << "s" << std::endl;
			std::cout << "c #solver calls..........: " << _satSolverCalls << std::endl;
		  }
		_control->SetSumTime(false);
		return ANTOM_UNKNOWN; 
	  }

	// There is only 1 bucket in total! Or did we reach maximum possible weight?
	// => we are done!
   	if ( (_currentBucketForTare == 0 ) || ( _satWeight == _sumOfSoftWeights) )
	  {
		if (_control->GetVerbosity() > 1)
		  {
			getrusage(RUSAGE_SELF, &resources);
			timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
			std::cout << "c " << (timeC - _control->GetStartTime()) << "s" << std::endl;
			std::cout << "c #solver calls..........: " << _satSolverCalls << std::endl;
		  }
		return ANTOM_SAT;
	  }

	if (_control->GetVerbosity() > 1)
	  {
		std::cout << std::endl << "c Minimize T from now on!" << std::endl;
	  }

	currentresult = AdjustTareValues(optimum);
	
	return currentresult;
  }


  uint32_t Antom::ProceedLastBucket(int32_t& optimum)
  {
	int32_t pos(0);
	uint32_t currentresult(ANTOM_UNKNOWN);
#ifndef NDEBUG
	int32_t lastpos(0);
#endif
	Sorter* lastBucket(_sorterTree.back()[0]);

	assert( _highestBucketMultiplicator > 0 );
	struct rusage resources;

	bool doloop(true);

	// Set initial value for pos
	// Could be set if we have performed some SAT-Solver calls in advance
	pos = (_satWeight / _highestBucketMultiplicator);
	// If there is no remainder, we have to decrease "pos", because it now represents the exact result and not an over-approximation
	// If "pos" is zero, no soft clause could be satisfied
	if( (_satWeight % _highestBucketMultiplicator) ==  0 )
	  {
		--pos;
	  }

	if( pos > 0 )
	  {
		optimum = _sumOfSoftWeights - _satWeight;
		// Output the current best result (optimum).
		if (_control->GetVerbosity() > 0)
		  {
			std::cout << "o " << optimum << std::endl;
		  }
#ifndef NDEBUG
		bool rst = AddUnit( (lastBucket->GetOutputs()[pos] << 1)^1); assert(rst);
#else
		AddUnit( (lastBucket->GetOutputs()[pos] << 1)^1);
#endif

		// all entries of last bucket could be satisfied -> start directly with minimizing tare
		if ( pos == (int32_t)(lastBucket->size() - 1) )
		  { doloop = false; }

		if( doloop )
		  {
			// Try to set the next position of last bucket
			_collectedAssumptions.push_back( ( lastBucket->GetOutputs()[pos+1] << 1) ^ 1);
			if (_control->GetVerbosity() > 2)
			  {
				std::cout << "c add assumption: " << helper::Lit(_collectedAssumptions.back()) << std::endl;
			  }
		  }
	  } 

	// First try to find solution interval by analyzing the bucket with the largest weight
	while (doloop)
	  {
		if( _maxInprocess )
		  {
			// Do inprocessing
			currentresult = Preprocess(INPROCESS);
		
			// Have we found the optimum?
			if (currentresult != ANTOM_UNKNOWN)
			  { break; }
		  }

		// Solve the CNF
		currentresult = Solve(_collectedAssumptions);

		// Have we found the optimum for the outer bucket!
		// Stop here and start adjusting tare
		if (currentresult == ANTOM_UNSAT)
		  {
			if (_control->GetVerbosity() > 1)
			  {
				std::cout << "c Currently UNSAT, because of last assumption: " << helper::Lit(_collectedAssumptions.back()) << std::endl;
			  }

			// No assumption? Then the whole problem is unsat
			if( _collectedAssumptions.empty() )
			  { return ANTOM_UNSAT; }

			// Add flipped last assumption
			AddUnit(_collectedAssumptions.back()^1);

			_collectedAssumptions.pop_back();
			break;
		  }
		// Reached timeout?
		else if (currentresult == ANTOM_UNKNOWN)
		  {
			return ANTOM_UNKNOWN;
		  }
		assert(currentresult == ANTOM_SAT);

		// Try to satisfy more soft clauses by chance.
		_lastModel = Model();
		_satWeight = CountSatisfiedSoftClauses(nullptr,_lastModel);
		
		// current position of last bucket
#ifndef NDEBUG
		lastpos = pos;
#endif
		pos = (_satWeight / _highestBucketMultiplicator);
		// If there is no remainder, we have to decrease "pos", because it now represents the exact result and not an over-approximation
		// If "pos" is zero, no soft clause could be satisfied
		if( (_satWeight % _highestBucketMultiplicator) ==  0 )
		  {
			--pos;
		  }

		assert( (pos <= 0) || (lastpos < pos) );
		assert(pos == ((int32_t)lastBucket->size()-1) || _lastModel[lastBucket->GetOutputs()[pos+1]] == ((lastBucket->GetOutputs()[pos+1]<<1)) );
		
		// at which position of the last bucket we are?
		// to differentiate, because of wrong solutions if only one bucket. WHY??
		_estimatedSatWeight = _highestBucketMultiplicator * (pos+1);
		_diffToSatWeight = _estimatedSatWeight - _satWeight;

		optimum = _sumOfSoftWeights - _satWeight;
		// Output the current best result (optimum).

		if (_control->GetVerbosity() > 0)
		  {
			std::cout << "o " << optimum << std::endl;

			if (_control->GetVerbosity() > 1)
			  {
				getrusage(RUSAGE_SELF, &resources);
				double timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
				std::cout << "c " << (timeC - _control->GetStartTime()) << "s" << std::endl;

				if ((_control->GetVerbosity() > 2))
				  {
					std::cout << "c satisfied weights: ( " << _satWeight << " / " << _sumOfSoftWeights << " )" << std::endl;
					std::cout << "c diff to estimated: " << _diffToSatWeight << std::endl;
					std::cout << "c maximal estimated: " << _estimatedSatWeight << std::endl;
			
					std::cout << "c Bucket Multiplicators: ( " << _currentBucketMultiplicator << " / " << _highestBucketMultiplicator << " )" << std::endl;
				  }
			  }
		  }

#ifndef NDEBUG
		CheckGates();
#endif
		assert(_estimatedSatWeight >= _satWeight);

		// Clear assumptions for next run
		_collectedAssumptions.clear();

		if( pos > 0 )
		  {
#ifndef NDEBUG
			bool rst = AddUnit( (lastBucket->GetOutputs()[pos] << 1)^1); assert(rst);
#else
			AddUnit( (lastBucket->GetOutputs()[pos] << 1)^1);
#endif
		  } 

		// all entries of last bucket could be satisfied -> start with minimizing tare
		if ( pos == (int32_t)(lastBucket->size() - 1) )
		  { break; }

		// Try to set the next position of last bucket
		_collectedAssumptions.push_back( ( lastBucket->GetOutputs()[pos+1] << 1) ^ 1);
		if (_control->GetVerbosity() > 2)
		  {
			std::cout << "c add assumption: " << helper::Lit(_collectedAssumptions.back()) << std::endl;
		  }
	  }

	_estimatedSatWeight = _highestBucketMultiplicator * (pos+1);
	_diffToSatWeight = _estimatedSatWeight - _satWeight;

	return ANTOM_SAT;
  }

  uint32_t Antom::AdjustTareValues(int32_t& optimum)
  {
	int32_t currentoptimum(optimum);
	uint32_t currentresult(ANTOM_UNKNOWN);

	struct rusage resources;
  
	// Now adjust the tare values
	while( true )
	  {
		SetTareBounds();

		if( _maxInprocess )
		  {
			// Do inprocessing
			currentresult = Preprocess(INPROCESS);
		
			// Have we found the optimum?
			if (currentresult != ANTOM_UNKNOWN)
			  { break; }
		  }

		// Solve the CNF
		currentresult = Solve(_collectedAssumptions);

		// Have we found the optimum for the outer bucket!
		if (currentresult == ANTOM_UNSAT)
		  {
			// No Tares left to adjust 
			// -> we are done
			if (_currentBucketForTare == 0 )
			  {
				if (_control->GetVerbosity() > 1)
				  {
					getrusage(RUSAGE_SELF, &resources);
					double timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
					std::cout << "c " << (timeC - _control->GetStartTime()) << "s" << std::endl;
					std::cout << "c #solver calls..........: " << _satSolverCalls << std::endl;
				  }
				return ANTOM_SAT;
			  }
			assert( _collectedAssumptions.size() == 1);
			uint32_t lastassumption = _collectedAssumptions.back();
			_collectedAssumptions.pop_back();
			if (_control->GetVerbosity() > 1)
			  {
				std::cout << "c Currently UNSAT, because of last tare bound: " << helper::Lit(lastassumption) << std::endl;
			  }

			// unsat with last assumption?
			// Then we know we have to flip the assumption
			AddUnit(lastassumption^1);
			// Since instance was unsat, we have to decrease our estimation
			_estimatedSatWeight -= std::pow(_baseMode,_currentBucketForTare);

			Sorter* currentBucket = _sorterTree[_currentBucketForTare][0];
			// We can now set the remaining tares of the current bucket...

			uint32_t pos = currentBucket->GetTarePosition();
			for( ; pos < _sorterTree[_currentBucketForTare][0]->NumberOfTares(); ++pos )
			  {
				uint32_t nextt( currentBucket->GetCurrentTare() );
				AddUnit( (nextt<<1)^1 );
				// Since instance was unsat, we have to decrease our estimation
				_estimatedSatWeight -= std::pow(_baseMode,_currentBucketForTare);
			  }
			currentBucket->SetTarePosition(pos);
			
			// Adjust next tare bound
			continue;
		  }
		// Reached timeout?
		else if (currentresult == ANTOM_UNKNOWN)
		  {
			assert(_control->GetCPULimit() > 0.0 || _control->GetMemoryLimit() > 0 );

			if (_control->GetIncrementalMode() > 0)
			  {
				InvalidateSoftClauses();
			  }

			if (_control->GetVerbosity() > 1)
			  {
				getrusage(RUSAGE_SELF, &resources);
				double timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
				std::cout << "c " << (timeC - _control->GetStartTime()) << "s" << std::endl;
				std::cout << "c #solver calls..........: " << _satSolverCalls << std::endl;
			  }
			_control->SetSumTime(false);
			return ANTOM_UNKNOWN;
		  }
  
		
		assert(currentresult == ANTOM_SAT);

		while (!_collectedAssumptions.empty())
		  {
			AddUnit(_collectedAssumptions.back());
			_collectedAssumptions.pop_back();
		  }

		// Try to satisfy more soft clauses by chance.
		_lastModel = Model();
		_satWeight = CountSatisfiedSoftClauses(nullptr,_lastModel);

		currentoptimum = _sumOfSoftWeights - _satWeight;
		if (currentoptimum < optimum)
		  {
			optimum = currentoptimum;
			if (_control->GetVerbosity() > 0)
			  {
				std::cout << "o " << optimum << std::endl;
				if (_control->GetVerbosity() > 1)
				  {
					getrusage(RUSAGE_SELF, &resources);
					double timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
					std::cout << "c " << (timeC - _control->GetStartTime()) << "s" << std::endl;
				  }
			  }
		  }

#ifndef NDEBUG
		CheckGates();
#endif

		assert(_satWeight <= _estimatedSatWeight);
		_diffToSatWeight = _estimatedSatWeight - _satWeight;

		if (_control->GetVerbosity() > 2)
		  {
			std::cout << "c satisfied weights: ( " << _satWeight << " / " << _sumOfSoftWeights << " )" << std::endl;
			std::cout << "c diff to estimated: ( " << _diffToSatWeight << " / " << _estimatedSatWeight << ")" << std::endl;
			
			std::cout << "c Bucket Multiplicators: ( " << _currentBucketMultiplicator << " / " << _highestBucketMultiplicator << " )" << std::endl;

			if (_control->GetVerbosity() > 3)
			  {
				DumpBucketModel(_lastModel);
			  }
		  }

		const Sorter* currentBucket = _sorterTree[_currentBucketForTare][0];
		// Problem SAT and we reached the last bucket and last tare position:
		// -> We are done
		if ( (_currentBucketForTare == 0) && ( currentBucket->GetTarePosition() == (_baseMode-1) ) )
		  {
			if (_control->GetVerbosity() > 1)
			  {
				getrusage(RUSAGE_SELF, &resources);
				double timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
				std::cout << "c " << (timeC - _control->GetStartTime()) << "s" << std::endl;
				std::cout << "c #solver calls..........: " << _satSolverCalls << std::endl;
			  }
			return ANTOM_SAT;
		  }

		if (_satWeight == _sumOfSoftWeights)
		  {
			if (_control->GetVerbosity() > 1)
			  {
				getrusage(RUSAGE_SELF, &resources);
				double timeC = (double) resources.ru_utime.tv_sec + 1.e-6 * (double) resources.ru_utime.tv_usec;
				std::cout << "c " << (timeC - _control->GetStartTime()) << "s" << std::endl;
				std::cout << "c #solver calls..........: " << _satSolverCalls << std::endl;
			  }
			return currentresult;
		  }
	  }
	return currentresult;
  }

  void Antom::SetTareBounds(void)
  {
	// Reset tare position if we have to visit the next bucket
	if( _sorterTree[_currentBucketForTare][0]->GetTarePosition() == (_baseMode-1) )
	  {
		--_currentBucketForTare;
	  }

	// Get current difference to estimated weight
	_diffToSatWeight = _estimatedSatWeight - _satWeight;

	if (_control->GetVerbosity() > 2)
	  {
		// output the variables of the Tare T
		std::cout << std::endl << "c Model of Tares: (n-1...0): ";
		for (int whichBucket = (_sorterTree.size() - 2); whichBucket >= 0; --whichBucket)
		  {
			std::cout << "(";
			for (int whichTare = _baseMode - 2; whichTare >= 0; --whichTare)
			  {
				std::cout << helper::Lit(_lastModel[_sorterTree[whichBucket][0]->GetTares()[whichTare]]);
				if (whichTare != 0)
				  {	std::cout << ", "; }
			  }
			if (whichBucket != 0)
			  { std::cout << "), "; }
			else
			  { std::cout << ")"; }
		  }
		std::cout << std::endl;
	  }

	// minimize T logarithmically
	int32_t currentbucket = _currentBucketForTare;
	for (;currentbucket >= 0; --currentbucket )
	  {
		_currentBucketMultiplicator = pow(_baseMode, currentbucket);

		Sorter* currentBucket = _sorterTree[currentbucket][0];

		// Start with the current tare 
		uint32_t pos = currentBucket->GetTarePosition();
		for( ; pos < _sorterTree[currentbucket][0]->NumberOfTares(); ++pos )
		  {
			uint32_t currentTare( currentBucket->GetCurrentTare() );

			if (_control->GetVerbosity() > 1)
			  {
				std::cout << "c currentTare: " << currentTare << " no. " << (currentBucket->GetTarePosition()+1) << " of " << currentBucket->NumberOfTares() << std::endl;
			  
				if ((_control->GetVerbosity() > 2))
				  {
					bool comp(_diffToSatWeight >= _currentBucketMultiplicator);
					std::cout << "c diff to estimated <=> currentBucketMultiplicator:   " << _diffToSatWeight << (comp?" >= ": " < ") << _currentBucketMultiplicator << std::endl;
				  }
			  }
			
			// Our actual result implies that the currentTare can be set to true
			if ( _diffToSatWeight < _currentBucketMultiplicator )
			  {
#ifndef NDEBUG
				bool rst = AddUnit(currentTare << 1); assert(rst);
#else
				AddUnit(currentTare << 1);
#endif
			
				if (_control->GetVerbosity() > 2)
				  { 
					std::cout << "c DiffToSatWeight < CurrentBucketMult; set: " << currentTare << std::endl;
				  }	
				
				// if this assertion isn't fulfilled the whole formula cannot be fulfilled anymore
				assert(_diffToSatWeight <= _baseMode * (_currentBucketMultiplicator * (_baseMode - 1)));
			  }
			// If new result is already larger as maximal possible weight, we do not need to try,
			// -> the corresponding tare have to set to FALSE
			else if( (_estimatedSatWeight-_currentBucketMultiplicator) > _sumOfSoftWeights )
			  {
#ifndef NDEBUG
				bool rst = AddUnit((currentTare << 1)^1); assert(rst);
#else
				AddUnit((currentTare << 1)^1);
#endif

				if (_control->GetVerbosity() > 2)
				  { 
					std::cout << "c estimation larger than largest possible result" << std::endl; 
					std::cout << "c set: " << helper::Lit(currentTare^1) << std::endl;
				  }	

				// adjust estimated weight
				_estimatedSatWeight -= pow(_baseMode,currentbucket);
				_diffToSatWeight = _estimatedSatWeight - _satWeight;
			  } 
			// At this point we have have found a bucket for which we try to set the tare the TRUE
			else
			  {
				_collectedAssumptions.push_back(currentTare << 1);
				// increase tare position
				currentBucket->IncreaseTarePosition();
				if (_control->GetVerbosity() > 2)
				  { 
					std::cout << "c DiffToSatWeight => CurrentBucketMult; Tryout: " << helper::Lit(_collectedAssumptions.back()) << std::endl;
				  }
				_currentBucketForTare = currentbucket;
				return;
			  }
			currentBucket->IncreaseTarePosition();
		  }
			
		if( currentbucket == 0 )
		  { break; }
	  }
	_currentBucketForTare = currentbucket;
  }

  void Antom::SetEncode01Mode(bool val)
  {
	_encode01Mode = val;
  }

  void Antom::SetDecStratMode(uint32_t val)
  {
	assert( val < 3 );
	_decStratMode = val;
  }

  void Antom::SetBaseMode(uint32_t val)
  {
	assert( val > 0 );
	_baseMode = val;
  }
}
