
/********************************************************************************************
sorter.cpp -- Copyright (c) 2016, Sven Reimer

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

// Include antom related headers.
#include "antom.h"
#include "control.h"

#include "sorter.h"

namespace antom
{
  // Constructor
  Sorter::Sorter(uint32_t size, Antom* antom) :
	//_solver(solverProxy),
	_antom(antom),
	_control(antom->_control),
	_outputs(),
	_softClauses(),
	_tare(),
	_minContra(size),
	_depth(0),
	_minUnsatisfied(0),
	_minSatisfied(0),
	_sumOfWeights(0),
	_proceeded(false),
	_proceedNext(false),
	_tarePosition(0),
	_sorterType(antom->_networkType)
  {
	assert(antom != nullptr);
	//assert(solverProxy != nullptr);
  }

  Sorter::~Sorter(void)
  {}

  uint32_t Sorter::GetCurrentTare(void) const
  {
	return _tare[_tarePosition];
  }

  uint32_t Sorter::CurrentGlobalAssumption(void) const
  { return _antom->CurrentGlobalAssumption(); }

  void Sorter::AddBypassClause(uint32_t lit1, uint32_t lit2)
  {
	//assert(_solver != nullptr);
	assert(_antom != nullptr);
	std::vector< uint32_t > newclause(2);
	newclause[0] = lit1;
	newclause[1] = lit2;

	if( _control->GetIncrementalMode() > 0 )
	  {
		// Add tglobal assumption
		newclause.push_back( CurrentGlobalAssumption() );
	  }

#ifndef NDEBUG
	bool rst = _antom->AddClause(newclause); assert(rst);			
#else
	_antom->AddClause(newclause);
#endif
  }


  // Checks whether soft clauses are satisfied "by chance" by the model "model", i.e. the trigger
  // literal in a soft clause is set to TRUE, but the clause is satisfied without the trigger literal
  // Returns the number of overall satisfied soft clauses
  // Merge this sorter with "sorter"
  void Sorter::MergeWithSorter(Sorter& sorter)
  {
	assert( _antom->_triggervar != 0 );

	if( _sorterType != TOTALIZER )
	  {
		// Fill the two parts in order to get two balanced part
		while( size() < sorter.size() )
		  {
			uint32_t dummyvar = _antom->_triggervar;
			_outputs.push_back( dummyvar );
		  }
		while( sorter.size() < size() )
		  {
			uint32_t dummyvar = _antom->_triggervar;
			sorter._outputs.push_back( dummyvar );
		  }
				
		assert( size() == sorter.size() );
	  }

	uint32_t thissortersize = size();

	std::vector< uint32_t > tmpoutputs(_outputs);

	// Merge current triggers
	_outputs.insert( _outputs.end(), sorter._outputs.begin(), sorter._outputs.end());


	// Reverse "left half": bitonic sorter needs one half in decreasing and the other in increasing order
	if( _sorterType == BITONIC )
	  {
		uint32_t middle = size()>>1;
		uint32_t start = 0;
		while ((start != middle) && (start != --middle)) 
		  { std::swap(_outputs[start++], _outputs[middle]); }
	  }

	// Merge original trigger indices
	_softClauses.insert( _softClauses.end(), sorter._softClauses.begin(), sorter._softClauses.end() );

	// "Re-don't touch" current trigger vars
	if( _depth > 0 )
	  {
		for( uint32_t i = 0; i != size(); ++i )
		  {
			_antom->SetDontTouch(_outputs[i], false);
		  }
	  }

	_minUnsatisfied = _minUnsatisfied + sorter._minUnsatisfied;
	_minSatisfied = _minSatisfied + sorter._minSatisfied;

	_sumOfWeights += sorter._sumOfWeights;
	_proceeded = false;

	//clear model
	_antom->TrivialAssignment();

	FindCSC();

	// Merge two partial sets
	if ( _sorterType == BITONIC )
	  {
		//uint32_t tmpvar = _antom->Variables();
		BitonicMerge( 0, size(), false );
		//		std::cout << "introduce " << (_antom->Variables()-tmpvar) << " new variables, merging " << size() << std::endl;
	  }
	else if ( _sorterType == ODDEVEN )
	  {
		uint32_t n = helper::SmallestPowerOfTwoLargerThan(size());
		for( uint32_t i = size(); i < n; ++i )
		  {
			uint32_t dummyvar = _antom->_triggervar;
			_outputs.push_back( dummyvar );
		  }

		OddEvenMerge( 0, size(), 1 );
	  }
	else if ( _sorterType == TOTALIZER )
	  {
		EncodingUV(0, thissortersize, size());
	  }
	else
	  {
		//TODO
		assert(false);
	  }

	// extend model size if necessary
	assert( _antom->_bestModel.size() <= _antom->_lastModel.size());
	uint32_t max = _antom->Variables()+1;
	if( _antom->_lastModel.size() > max )
	  {
		max = _antom->_lastModel.size();
	  }
	if (max > _antom->_bestModel.size())
	  {
		_antom->_bestModel.resize(max,0);
		_antom->_lastModel.resize(max,0);
	  }

	for( uint32_t i = 0; i != size(); ++i )
	  {
		_antom->SetDontTouch(_outputs[i]);
	  }

	SetSorterCSC();

	SetHorizontalBypasses( tmpoutputs, sorter._outputs );

	// increase depth
	++_depth;

	SetVerticalBypasses();

	if ( _control->GetVerbosity() > 1)
	  {
		std::cout << "c comparator: " << _antom->_comparator << " skipped: " << _antom->_skipped << std::endl;
		std::cout << "c horizontal bypasses: " << _antom->_horizontalbypasses << " vertical bypasses: " << _antom->_verticalbypasses << std::endl;
	  }
  }

  // Search in soft clauses for sorter bypasses by "deduceAssumption"
  void Sorter::FindCSC(void)
  {
	//assert(_solver != nullptr);
	assert(_antom != nullptr);
		
	if( _antom->_setCSC == 0 )
	  { return; }

	if( _control->GetVerbosity() > 1 )
	  { std::cout << "c " << __func__ << " " << size() << std::endl; }

	_minContra = size();
	uint32_t trivialcontra(0);
	uint32_t unsats(0);
	uint32_t bypasses(0);

	std::vector< SoftClause* > candidates;
	size_t softclausessize = _softClauses.size();
	for (size_t i = 0; i != softclausessize; ++i)
	  {
		uint32_t relaxlit(_softClauses[i]->relaxationLit);
		if (_antom->Model()[relaxlit>>1] == 0 )
		  {
			candidates.push_back(_softClauses[i]);
		  }
		else if (_antom->Model()[relaxlit>>1] == (relaxlit))
		  {
			++trivialcontra;
		  }
	  }

	//for (size_t i = 0; i != softclausessize; ++i)
	for (size_t i = 0; i != candidates.size(); ++i)
	  {
		std::vector< uint32_t > assumptions;
		uint32_t lit( candidates[i]->relaxationLit );

		candidates[i]->contra = trivialcontra;

		assumptions.push_back( lit^1 );

		// Deduce activation of current soft clause by setting it to FALSE
		bool rst = _antom->DeduceAssumptions(assumptions);

		//std::cout << helper::Lit(lit^1) << " rst: " << rst << std::endl;

		// Instance is not satisfied if current soft clause is activated
		if( !rst )
		  {
			rst = _antom->AddUnit( lit ); 
			assert( rst );
			++unsats;
		  }
		else
		  {
			const std::vector< uint32_t >& solvermodel(_antom->Model());

			// Check model
			for( uint32_t j = 0; j != candidates.size(); ++j )
			  {
				if (i == j)
				  {
					continue;
				  }
				//uint32_t checklit( _softClauses[j]->relaxationLit );
				uint32_t checklit( candidates[j]->relaxationLit );

				// Contradiction... j'th soft clause is always TRUE if i'th soft clause is FALSE
				if( solvermodel[checklit>>1] == checklit )
				  {
					++candidates[i]->contra;
					++bypasses;

					AddBypassClause(lit, checklit);
				  }
				// The j'th soft clause is always FALSE if i'th soft clause is FALSE
				else if( solvermodel[checklit>>1] == (checklit^1) )
				  {
					++bypasses;

					AddBypassClause(lit, checklit^1);
				  }
			  }

			if( _minContra > candidates[i]->contra )
			  { 
				_minContra = candidates[i]->contra;
			  }
		  }
	  }

	if( _control->GetVerbosity() > 1 )
	  { 
		std::cout << "c find overall " << bypasses << " bypasses and " << unsats << " unsats" << std::endl; 
	  }

	// Reset model from deduce assumptions
	_antom->TrivialAssignment();
  }

  // Set clause bypasses in sorter, given by found trivial cases in "findSorterBypasses()"
  // Assumes that the triggervariables in "m_inputsParts" are up to date
  void Sorter::SetSorterCSC(void)
  {
	//assert(_solver != nullptr);
	assert(_antom != nullptr);
	
	// Currently does not work with weighted maxsat
	if( (_antom->_setCSC == 0 ) || (_control->GetApplication() == WEIGHTEDMAXSAT ) )
	  { return; }

	// No of soft clauses in this part
	unsigned int noofsoftclauses(size());
	unsigned int softclausessize(_softClauses.size());

	unsigned int diff = noofsoftclauses - softclausessize;

	if ( _minContra < noofsoftclauses && _minContra > 0 )
	  {
		uint32_t var( _outputs[noofsoftclauses - _minContra - diff] );
#ifndef NDEBUG 
		bool rst = _antom->AddUnit( var<<1 ); assert( rst );
#else
		_antom->AddUnit( var<<1 );
#endif
	  }

	for (uint32_t i = 0; i != softclausessize; ++i)
	  {
		uint32_t softClauseContra = _softClauses[i]->contra;
		if (softClauseContra > _minContra )
		  {
			uint32_t var( _outputs[noofsoftclauses - softClauseContra - diff] );
			AddBypassClause(_softClauses[i]->relaxationLit, var<<1);
		  }
	  }
  }

  void Sorter::SetVerticalBypasses(void)
  {
	if ( _antom->_bypassGrid != 2 && _antom->_bypassGrid != 3 )
	  { return; }

	// Add additional contraints to the sorter network interface
	for (uint32_t i = 1 ; i < _outputs.size(); ++i)
	  {
		if( ( _outputs[i] != _outputs[i-1] )  )
		  {
			// temp[i-1] == 1 -> temp[i]==1
			// temp[i] == 0 -> temp[i-1]==0
			AddBypassClause( (_outputs[i-1]<<1)^1, _outputs[i]<<1 );
			++_antom->_verticalbypasses;
		  }
	  }
  }

  void Sorter::SetHorizontalBypasses(const std::vector< uint32_t >& outputA, const std::vector< uint32_t >& outputB)
  {
	if( _antom->_bypassGrid != 1 && _antom->_bypassGrid != 3 )
	  { return; }

	// Totalizer networks have implicit horizontal bypasses
	if( _sorterType == TOTALIZER )
	  { return; }

	size_t sizeA = outputA.size();
#ifndef NDEBUG
	size_t sizeB = outputB.size();
	assert( sizeA == sizeB );
#endif

	for( size_t i = 0; i != sizeA; ++i )
	  {
		// Set every 4 positions
		if ( ( i != 0 ) && ( (i&3) != 3 ) )
		  { continue; }

		// skip trivial parts
		if( _antom->Model()[outputA[i]] == 0 )
		  { 
			if( _antom->Model()[_outputs[i+sizeA]] == 0 )
			  {
				// "Forward bypass"
				// A[i] == 1 => trigger[i+size] == 1
				AddBypassClause( (outputA[i]<<1)^1, _outputs[i+sizeA]<<1 );
				++_antom->_horizontalbypasses;
			  }

			if( _antom->Model()[_outputs[i]] == 0 )
			  {
				// "Backward bypass"
				// A[i] == 0 => trigger[i] == 0
				AddBypassClause(outputA[i]<<1, (_outputs[i]<<1)^1);
				++_antom->_horizontalbypasses;
			  }
		  }

		if( _antom->Model()[outputB[i]] == 0 )
		  { 
			if( _antom->Model()[_outputs[i+sizeA]] == 0 )
			  {
				// "Forward bypass"
				// B[i] == 1 => trigger[i+size] == 1
				AddBypassClause( (outputB[i]<<1)^1, _outputs[i+sizeA]<<1 );
				++_antom->_horizontalbypasses;
			  }
			if( _antom->Model()[_outputs[i]] == 0 )
			  {
				// "Backward bypass"
				// B[i] == 0 => trigger[i] == 0
				AddBypassClause( outputB[i]<<1, (_outputs[i]<<1)^1 );
				++_antom->_horizontalbypasses;
			  }
		  }
	  }
  }

  // Pushes new part in front of "inputsParts"
  void Sorter::EncodeSorter(uint32_t weight)
  {
	if( size() <= 1 )
	  { return; }

	FindCSC();

	/*
	std::cout << "inputs: " << std::endl;
	for(uint32_t i = 0; i != size(); ++i )
	  {
		std::cout << outputs[i] << " ";
	  }
	std::cout << std::endl;
	*/
	assert( ( _antom->_triggervar != 0 ) || ( _control->GetIncrementalMode() == 0 ) );

	// Set last index of original problem
	if( _antom->_lastIndex == 0 )
	  {
		_antom->_lastIndex = _antom->Variables();
	  }

	// clear model
	_antom->TrivialAssignment();

	// Creates and adds sorter network clauses
	if ( _sorterType == BITONIC )
	  {
		//uint32_t tmpvar = _antom->Variables();
		BitonicSort( 0, size(), false );
		//		std::cout << "introduced " << (_antom->Variables()-tmpvar) << " new variables for size: " << size();
	  }
	else if ( _sorterType == ODDEVEN )
	  {
		uint32_t n = helper::SmallestPowerOfTwoLargerThan(size());
		for( uint32_t i = size(); i < n; ++i )
		  {
			uint32_t dummyvar = _antom->_triggervar;
			_outputs.push_back( dummyvar );
		  }

		OddEvenSort( 0, size() );
	  }
	else if ( _sorterType == TOTALIZER )
	  {
		TotalizerPhi(0, size());
	  }
	else
	  {
		// TODO
		assert(false);
	  }

	for(uint32_t i = 0; i != size(); ++i )
	  {
		_antom->SetDontTouch(_outputs[i]);
	  }

	/*
	std::cout << "outputs: " << std::endl;
	for(uint32_t i = 0; i != size(); ++i )
	  {
		std::cout << outputs[i] << " ";
	  }
	std::cout << std::endl;
	*/

	SetVerticalBypasses();
	_depth = weight;
	SetSorterCSC();
  }  

  // Helper functions to generate the "Bitonic Sorting Network" used for (partial) MaxSAT solving.
  // See "http://www.iti.fh-flensburg.de/lang/algorithmen/sortieren/bitonic/oddn.htm" for more details. 
  void Sorter::BitonicSort(uint32_t lo, uint32_t n, bool dir)
  {
	assert( _control->GetIncrementalMode() == 0 || CurrentGlobalAssumption() != 0 );

    if (n > 1)
      {
		uint32_t m(n >> 1);
		BitonicSort(lo, m, !dir);
		BitonicSort(lo + m, n - m, dir);
		BitonicMerge(lo, n, dir);
      }
  }

  void Sorter::BitonicMerge(uint32_t lo, uint32_t n, bool dir)
  {
    if (n > 1)
      {
		uint32_t m(helper::GreatestPowerOfTwoLessThan(n));
		for (uint32_t i = lo; i < (lo + n - m); ++i)
		  { BitonicCompare(i, i + m, dir); }
		BitonicMerge(lo, m, dir);
		BitonicMerge(lo + m, n - m, dir);
      }
  }

  void Sorter::BitonicCompare(uint32_t i, uint32_t j, bool dir)
  {
	//assert(_solver != nullptr);
	assert(_antom != nullptr);
	// Don't do anything if current trigger is trivally true/false
	// If the "left" trigger is true or the "right" trigger is false -> swap (shift true to "left")
	if( _antom->_doskipping && ( _antom->Model()[_outputs[i]] != 0 || _antom->Model()[_outputs[j]] != 0 ) )
	 { 
		++_antom->_skipped;

		if( dir && ( _antom->Model()[_outputs[j]] == (_outputs[j]<<1) ||
					 _antom->Model()[_outputs[i]] == ( (_outputs[i]<<1)^1 ) ) )
		  { 
			std::swap( _outputs[i], _outputs[j] ); 
		  }
		else if ( !dir && ( _antom->Model()[_outputs[i]] == (_outputs[i]<<1) ||
							_antom->Model()[_outputs[j]] == ( (_outputs[j]<<1)^1 ) ) )
		  { 
			std::swap( _outputs[i], _outputs[j] ); 
		  }

		// If the "left" trigger is false or the "right" trigger is true -> do nothing (shift false to "right")
		return;
	  }

	++_antom->_comparator;
   
    // Encode "v1 = trigger[i] OR trigger[j]".
	uint32_t v1( _antom->NewVariable() );
	_antom->EncodeOR(v1, _outputs[i], _outputs[j]);

    // Encode "v2 = trigger[i] AND trigger[j]".
	uint32_t v2( _antom->NewVariable() );
	_antom->EncodeAND(v2, _outputs[i], _outputs[j]);
      
    // Finally, update "trigger".
    if (dir)
      { _outputs[i] = v1; _outputs[j] = v2; } 
    else
      { _outputs[i] = v2; _outputs[j] = v1; } 
  }


    /** sorts a piece of length n of the array
     *  starting at position lo
     */
  void Sorter::OddEvenSort(uint32_t lo, uint32_t n)
  {
	if (n>1)
	  {
		//uint32_t m = n>>1;
		uint32_t m(helper::GreatestPowerOfTwoLessThan(n));
		OddEvenSort( lo, m);
		OddEvenSort( lo+m, m);
		OddEvenMerge( lo, n, 1);
	  }
    }
  
    /** lo is the starting position and
     *  n is the length of the piece to be merged,
     *  r is the distance of the elements to be compared
     */
	void Sorter::OddEvenMerge(uint32_t lo, uint32_t n, uint32_t r)
    {
	  uint32_t m = r<<1;

	  if ( m < n )
        {
		  OddEvenMerge(lo, n, m);      // even subsequence
		  OddEvenMerge(lo+r, n, m);    // odd subsequence

		  for (int i=lo+r; i+r<lo+n; i+=m)
			{ OddEvenCompare(i, i+r); }
        }
	  else
		{
		  OddEvenCompare(lo, lo+r);
		}
    }

  void Sorter::OddEvenCompare(uint32_t i, uint32_t j)
  {
	//assert(_solver != nullptr);
	assert(_antom != nullptr);
	// Don't do anything if current trigger is trivally true/false
	// If the "left" trigger is true or the "right" trigger is false -> swap (shift true to "left")
	if( _antom->_doskipping && ( _antom->Model()[_outputs[i]] != 0 || _antom->Model()[_outputs[j]] != 0 ) )
	 { 
		++_antom->_skipped;

		if( ( _antom->Model()[_outputs[i]] == (_outputs[i]<<1) ||
			  _antom->Model()[_outputs[j]] == ( (_outputs[j]<<1)^1 ) ) )
		  { 
			std::swap( _outputs[i], _outputs[j] ); 
		  }

		// If the "left" trigger is false or the "right" trigger is true -> do nothing (shift false to "right")
		return;
	  }

	++_antom->_comparator;

	// Get two fresh variables not used by solver so far.

    // Encode "v1 = trigger[i] OR trigger[j]".
	uint32_t v1( _antom->NewVariable()); 
	_antom->EncodeOR(v1, _outputs[i], _outputs[j]);

    // Encode "v2 = trigger[i] AND trigger[j]".
	uint32_t v2( _antom->NewVariable()); 
	_antom->EncodeAND(v2, _outputs[i], _outputs[j]);
      
    // Finally, update inputs
	_outputs[i] = v2; _outputs[j] = v1;
  }


  void Sorter::MergeTotalizer(std::vector< uint32_t >& X)
  {
	/*
	std::cout << "inputs: " << std::endl;
	for(uint32_t i = 0; i != size(); ++i )
	  {
		std::cout << outputs[i] << " ";
	  }
	std::cout << std::endl << "and" << std::endl;
	for(uint32_t i = 0; i != X.size(); ++i )
	  {
		std::cout << X[i] << " ";
	  }
	std::cout << std::endl;
	*/

	uint32_t outputsize = _outputs.size();

	for(uint32_t i = 0; i != size(); ++i )
	  {
		_antom->SetDontTouch(_outputs[i], false);
	  }	
	// The tares should be still don't touch
	for( uint32_t i = 0; i != _tare.size(); ++i )
	  {
		_antom->SetDontTouch(_tare[i], true);
	  }

	_outputs.insert( _outputs.end(), X.begin(), X.end());
	uint32_t newoutputsize = _outputs.size();

	// Skip, if X is empty
	if ( outputsize != newoutputsize )
	  {
		EncodingUV(0, outputsize, newoutputsize);
		SetVerticalBypasses();
	
		for(uint32_t i = 0; i != size(); ++i )
		  {
			_antom->SetDontTouch(_outputs[i]);
		  }	
	  }

	/*
	std::cout << "outputs: " << std::endl;
	for(uint32_t i = 0; i != size(); ++i )
	  {
		std::cout << outputs[i] << " ";
	  }
	std::cout << std::endl;
	*/


  }

  void Sorter::TotalizerPhi(uint32_t lo, uint32_t hi)
  {
	if( (hi-lo) > 1)
	  {
		assert(lo < hi);
		uint32_t m = ((hi-lo)>>1);
		// Calculate recursively the unary representation.
		TotalizerPhi(lo, lo+m);
		TotalizerPhi(lo+m, hi);
		EncodingUV(lo, lo+m, hi);
	  }
  }

  void Sorter::EncodingUV(uint32_t beginA, uint32_t endA, uint32_t endB)
  {
	//assert(_solver != nullptr);
	assert(_antom != nullptr);
	//std::cout << __func__ << " " << beginA << " " << endA << " " << endB << std::endl;
	// Variables----------------------------------------------------
	std::vector<uint32_t> clause;
	// if direction is true 0's at the beginning, 1's at the end
	// if direction is false the other way around.
	bool direction(true);
	assert( beginA <= endA );
	assert( endA < endB );
	assert( endB <= _outputs.size() );
	// -------------------------------------------------------------
	std::vector< uint32_t> W;
	for (uint32_t a = beginA; a < endB; ++a)
	  { W.push_back(_antom->NewVariable()); }

	uint32_t beginB = endA;

	for (uint32_t a = beginA; a <= endA; a++)
	  {
		uint32_t an = a-beginA;
		for (uint32_t b = beginB; b <=endB; b++)
		  {
			uint32_t bn = b-beginB;
			// To assure to get 0's at the beginning.
			if (_antom->_encode01Mode && ( ( a > beginA ) || ( b > beginB ) ) )
			  {

				if (a > beginA && b > beginB)
				  {
					clause.push_back((_outputs[a - 1] << 1) ^ !direction);
					clause.push_back((_outputs[b - 1] << 1) ^ !direction);
					clause.push_back((W[an + bn - 1] << 1) ^ direction);
				  }
				else if (a == beginA && b > beginB)
				  {
					clause.push_back((_outputs[b - 1] << 1) ^ !direction);
					clause.push_back((W[bn - 1] << 1) ^ direction);
				  }
				else if (b == beginB && a > beginA)
				  {
					clause.push_back((_outputs[a - 1] << 1) ^ !direction);
					clause.push_back((W[an - 1] << 1) ^ direction);
				  }
				++_antom->_comparator;
				_antom->AddClause(clause);
				clause.clear();
			  }

			if (a == endA && b == endB)
			  { continue; }

			//To assure to get 0's at the end.
			if (a < endA && b < endB)
			  {
				clause.push_back((_outputs[a] << 1) ^ direction);
				clause.push_back((_outputs[b] << 1) ^ direction);
				clause.push_back((W[an + bn] << 1) ^ !direction);
#ifndef NDEBUG
				_antom->_debugNetwork.push_back( Gate(W[an+bn],_outputs[a],_outputs[b], HALFANDGATE) );
#endif
			  }
			else if (a == endA && b < endB)
			  {
				clause.push_back((_outputs[b] << 1) ^ direction);
				clause.push_back((W[an + bn] << 1) ^ !direction);
#ifndef NDEBUG
				_antom->_debugNetwork.push_back( Gate(W[an+bn],_outputs[b], HALFORGATE) );
#endif
			  }
			else if (b == endB && a < endA)
			  {
				clause.push_back((_outputs[a] << 1) ^ direction);
				clause.push_back((W[an + bn] << 1) ^ !direction);
#ifndef NDEBUG
				_antom->_debugNetwork.push_back( Gate(W[an+bn],_outputs[a], HALFORGATE) );
#endif
			  }

			++_antom->_comparator;
			_antom->AddClause(clause);
			clause.clear();
		  }
	  }

	for (uint32_t a = beginA; a < endB; ++a)
	  {
		assert( (a-beginA) < W.size() );
		_outputs[a] = W[a-beginA];
	  }

  }

  // ____________________________________________________________________________________________________________________

  std::vector<uint32_t> Sorter::BaseRanks( uint32_t baseMode ) const
  {
	std::vector< uint32_t > baseRankVector;
	for (uint32_t i = 0; i < _outputs.size(); i++)
	  {
		if (i % baseMode == baseMode - 1)
		  {
			baseRankVector.push_back(_outputs[i]);
		  }
		// Since we merge the outputs of this sorter next, we can deactive the don't touch status
		_antom->SetDontTouch(_outputs[i], false);
	  }
	// The tares should be still don't touch
	for( uint32_t i = 0; i != _tare.size(); ++i )
	  {
		_antom->SetDontTouch(_tare[i], true);
	  }
	return baseRankVector;
  }
}
