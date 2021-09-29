//
// This file is part of the ijtihad QBF solver
// Copyright (c) 2016, Graz University of Technology
// Authored by Vedad Hadzic
//

#include "MySolver.hh"
#include "QuantifiedFormula.hh"
#include <set>
#include "debug.hh"
#include <assert.h>
#include <algorithm>
#include "auxiliary.hh"
#include <queue>
#include <sstream>
#include "ipasir.hh"
#include <random>

#define INDEX_PHI(v) ((v >> 1) + 1)
#define INDEX_KSI(v) ((v >> 1))

// #define layerp_index(v) (v >> 1)
// #define layerIndexKSI(v) ((v >> 1) - 1)

#define LAYERP_INVERSE(i) ((i << 1) + 1)
#define LAYERK_INVERSE(i) ((i + 1) << 1)

using std::endl;

#ifndef VERSION
#define VERSION "v970"
#endif

MySolver::MySolver(Prefix& pref, const CNF& cnf, const SolverOptions* options) :
    max_id_(0),
    current_top_ksi_(nullptr),
    sat_calls_(0),
    num_vars_phi_(0),
    num_vars_ksi_(0),
    num_clauses_phi_(0),
    num_clauses_ksi_(0),
    current_top_phi_(nullptr),
    layer_size_phi_(0),
    layer_size_ksi_(0),
    time_phi_(0.0),
    time_ksi_(0.0),
    time_(0.0)
{
  printVersion();
  debugn("Creating MySolver.");
  
  if (options == NULL)
    options_ = SolverOptions();
  else
    options_ = *options;
  
  if(options_.logging_phi)
  {
    try
    {
      phi_log_.open(options_.phi_log, std::ofstream::out);
      tmp_phi_clauses_.open(options_.tmp_phi_log, std::ofstream::out);
      phi_log_ << "c This file was generated by Ijtihad.\n";
    } catch (std::exception& e)
    {
      cout << "c " << e.what();
      options_.logging_phi = false;
    }
  }
  
  if(options_.logging_ksi)
  {
    try
    {
      ksi_log_.open(options_.ksi_log, std::ofstream::out);
      tmp_ksi_clauses_.open(options_.tmp_ksi_log, std::ofstream::out);
      ksi_log_ << "c This file was generated by Ijtihad.\n";
    } catch (std::exception& e)
    {
      cout << "c " << e.what();
      options_.logging_ksi = false;
    }
  }
  
  
  sat_solver_phi_ = ipasir_init();
  sat_solver_ksi_ = ipasir_init();
  
  debugn("Solver PHI is : " << sat_solver_phi_ << endl << "Solver KSI is: " << sat_solver_ksi_);
  
  // TODO: think about refactoring this
  
  debugn("There are " << pref.size() << " quantifiers.");
  
  if (pref.size() == 0)
    pref.push_back(Quantification(EXISTENTIAL, vector<Var>()));
  
  if (pref.size() == 1 && pref.front().first == EXISTENTIAL)
    pref.push_back(Quantification(UNIVERSAL, vector<Var>()));
  
  if (pref.front().first == UNIVERSAL)
    pref.insert(pref.begin(), Quantification(EXISTENTIAL, vector<Var>()));
  
  if (pref.back().first == UNIVERSAL)
    pref.push_back(Quantification(EXISTENTIAL, vector<Var>()));
  
  debugn("After adjustment there are " << pref.size() << " quantifiers.");
  
  formula_ = new QuantifiedFormula();
  
  renameVariables(pref, cnf);
  
  if (formula_->getNumQuants() % 2 != 1 || formula_->getQuant(0).first != EXISTENTIAL)
  {
    debugn("Error: prefix size is " << formula_->getNumQuants() << ", first quantifier is " \
      << ((formula_->getQuant(0).first == EXISTENTIAL) ? "EXISTENTIAL" : "UNIVERSAL"));
    return;
  }
  
  for (unsigned i = 1; i <= independent_phi_.size(); i++)
  {
    newVarPHI();
  }
  
  empty_layer_phi_.resize(layer_size_phi_, 0);
  empty_layer_ksi_.resize(layer_size_ksi_, 0);
  
  for(unsigned i = 1; i <= formula_->getQuant(1).second.size(); i++)
    top_ksi_base_.push_back(i);
  
  current_top_ksi_.rebase(&top_ksi_base_);
  current_top_phi_.rebase(&independent_phi_);
  
  for(unsigned i = 1; i <= layer_size_phi_; i++)
    tuple_base_phi_.push_back(i);
  
  for(unsigned i = 1; i <= layer_size_ksi_; i++)
    tuple_base_ksi_.push_back(i);
}

MySolver::~MySolver()
{
  if(options_.logging_phi)
    phi_log_.close();
  if(options_.logging_ksi)
    ksi_log_.close();
  
  delete formula_;
  ipasir_release(sat_solver_phi_);
  ipasir_release(sat_solver_ksi_);
  for(vector<Var>* b : layer_caluese_bases_)
    delete b;
}

int MySolver::initPHI()
{
  if (layer_size_ksi_ == 0)
  {
    void *triv_solver = ipasir_init();
    for (unsigned clause_num = 0; clause_num < formula_->getNumClauses(); clause_num++)
    {
      addClause(triv_solver, formula_->getClause(clause_num));
    }
    return ipasir_solve(triv_solver);
    ipasir_release(triv_solver);
  }

  LitTuple addition(&tuple_base_ksi_);
  vector<bool> addt;

  if(options_.warmup)
  {
    // instantiate all positive
    addt.assign(layer_size_ksi_, false);
    addition.assign(addt);
    counterexample_cache_.insert(addition);
    cex_to_add_.push_back(addition);

    // instantiate all negative
    addt.assign(layer_size_ksi_, true);
    addition.assign(addt);
    counterexample_cache_.insert(addition);
    cex_to_add_.push_back(addition);

    // instantiate only one quantifier negative
    unsigned q_index = 0;
    for (unsigned quant_index = 0; quant_index < formula_->getNumQuants(); quant_index++)
    {
      const Quantification& q = formula_->getQuant(quant_index);
      if(q.first == UNIVERSAL)
      {
        addt.assign(layer_size_ksi_, false);
        for(unsigned v_index = 0; v_index < q.second.size(); v_index++)
        {
          addt[q_index] = true;
          q_index += 1;
        }
        addition.assign(addt);
        if(counterexample_cache_.insert(addition).second)
          cex_to_add_.push_back(addition);
      }
    }

    // instantiate only one qunatifier positive
    q_index = 0;
    for (unsigned quant_index = 0; quant_index < formula_->getNumQuants(); quant_index++)
    {
      const Quantification& q = formula_->getQuant(quant_index);
      if(q.first == UNIVERSAL)
      {
        addt.assign(layer_size_ksi_, true);
        for(unsigned v_index = 0; v_index < q.second.size(); v_index++)
        {
          addt[q_index] = false;
          q_index += 1;
        }
        addition.assign(addt);
        if(counterexample_cache_.insert(addition).second)
          cex_to_add_.push_back(addition);
      }
    }
  }

  // instantiate with false for all universals
  addt.clear();
  for (unsigned quant_index = 0; quant_index < formula_->getNumQuants(); quant_index++)
  {
    const Quantification& q = formula_->getQuant(quant_index);
    if(q.first == UNIVERSAL)
      for(long unsigned i = 0; i < q.second.size(); i++)
        addt.push_back(true);
  }

  addition.assign(addt);
  if(counterexample_cache_.insert(addition).second)
    cex_to_add_.push_back(addition);

  std::mt19937 gen(0x243F6A88);
  std::uniform_int_distribution<> r(0,1);

  for(int i = 0; i < options_.warmup_randoms; i++)
  {
    addt.clear();
    for (unsigned quant_index = 0; quant_index < formula_->getNumQuants(); quant_index++)
    {
      const Quantification& q = formula_->getQuant(quant_index);
      if(q.first == UNIVERSAL)
      {
        for(unsigned v_i = 0; v_i < q.second.size(); v_i++)
          addt.push_back(bool(r(gen)));
      }
    }

    addition.assign(addt);
    debugn("adding random cex with hash:" << addition.getHash());
    if(counterexample_cache_.insert(addition).second)
      cex_to_add_.push_back(addition);
  }

  for(auto& c : counterexample_cache_)
  {
    for(long unsigned int i = 0; i < c.size(); i++)
    {
      cout << c[i] << " ";
    }
    cout << endl;
  }

  unsigned long cpc = options_.cex_per_call;
  options_.cex_per_call = (unsigned long)-1;

  extendPHI();
  options_.cex_per_call = cpc;

  return 0;
}

bool MySolver::solve()
{
  double time0 = read_cpu_time();
  
  int trivial = initPHI();
  if(trivial != 0)
  {
    return (trivial == 10);
  }
  
  cout << "c Running Ijtihad loop" << endl;
  
  vector<Lit> learned_clause;
  vector<Lit> empty_ksi_clause_part;
  
  while (true)
  {
    double time1 = read_cpu_time();
    
    bool phi_sat = (ipasir_solve(sat_solver_phi_) == 10);
    time_phi_ += read_cpu_time() - time1;
  
    if (phi_sat)
    {
      debugn("PHI is SAT");
      
      extendKSI();
      
      double time2 = read_cpu_time();
      bool ksi_sat = (ipasir_solve(sat_solver_ksi_) == 10);
      time_ksi_ += read_cpu_time() - time2;
      
      if (ksi_sat)
      {
        debugn("KSI is SAT");
        extendPHI();
      }
      else
      {
        debugn("KSI is UNSAT");
        time_ = read_cpu_time() - time0;
        if (options_.logging_ksi) generateDimacsKSI();
        return true;
      }
    }
    else
    {
      debugn("PHI is UNSAT");
      time_ = read_cpu_time() - time0;
      if (options_.logging_phi) generateDimacsPHI();
      return false;
    }
    sat_calls_++;
  }
}

void MySolver::generateDimacsPHI()
{
  if(!independent_phi_.empty())
  {
    phi_log_ << "c x ";
    for (unsigned i = 1; i <= independent_phi_.size(); i++)
      phi_log_ << i << " ";
  
    phi_log_ << "0 ";
    
    for (const Var& v : independent_phi_)
      phi_log_ << renaming_scheme_[v] << " ";
    
    phi_log_ << "0 0" << std::endl;
  }
  
  for (auto& p : vars_cache_phi_)
  {
    phi_log_ << "c x ";
    
    for(const Var& v : p.second)
      phi_log_ << v << " ";
  
    phi_log_ << "0 ";
    
    {
      unsigned i = 0;
      unsigned j = 0;
      while (j < p.first.size())
      {
        const Quantification& q = formula_->getQuant(i);
        if(q.first == UNIVERSAL)
          j += q.second.size();
        i++;
        assert(j <= p.first.size());
      }
      assert(j == p.first.size());
      assert(i < formula_->getNumQuants());
      const Quantification& q = formula_->getQuant(i);
      assert(q.first == EXISTENTIAL);
      for(const Var& v : q.second)
        phi_log_ << renaming_scheme_[v] << " ";
    }
    
    phi_log_ << "0 ";
    
    {
      Lit l = 0;
      for(unsigned lidx = 0; lidx < p.first.size(); lidx++)
      {
        l = p.first[lidx];
        phi_log_ << mkLit(renaming_scheme_[var(l)],sign(l)) << " ";
      }
      phi_log_ << "0" << std::endl;
    }
  }
  
  phi_log_ << "c o ";
  for(unsigned origin : origins_phi_)
    phi_log_ << origin << " ";
  phi_log_ << "0" << std::endl;
  
  phi_log_ << "p cnf " << num_vars_phi_ << " " << num_clauses_phi_ << std::endl;
  
  tmp_phi_clauses_.close();
  std::ifstream clause_file(options_.tmp_phi_log, std::ios_base::binary);
  assert(clause_file.good());
  char* buff = new char[1024 * 1024];
  while(clause_file.good())
  {
    clause_file.read(buff, 1024 * 1024);
    phi_log_.write(buff, clause_file.gcount());
  }
  delete[] buff;
  clause_file.close();
  phi_log_.close();
  if (options_.logging_ksi)
  {
    tmp_ksi_clauses_.close();
    ksi_log_.close();
  }
}

void MySolver::generateDimacsKSI()
{
  tmp_ksi_clauses_.close();
  ksi_log_.close();
  if (options_.logging_phi)
  {
    tmp_phi_clauses_.close();
    phi_log_.close();
  }
}


void MySolver::printStats()
{
  cout << "c sat_solver_phi calld " << sat_calls_
       << " times, running for " << time_phi_ << " seconds." << endl;
  cout << "c number of layers in PHI is " << layers_phi_.size() << endl;
  cout << "c sat_solver_ksi was running for " << time_ksi_ << " seconds." << endl;
  cout << "c number of layers in KSI is " << layers_ksi_.size() << endl;
  cout << "c mysolver was running for " << time_ << " seconds." << endl;
}

void MySolver::computeExtension(FormulaType type)
{
  debugn("computeExtension " << ((type == PHI) ? "PHI" : "KSI") << ": begin");
  unsigned long per_call = (type == PHI) ? options_.cex_per_call : options_.wit_per_call;
  vector<vector<Var>>& layers = (type == PHI) ? layers_ksi_ : layers_phi_;
  unsigned long layer_size = (type == PHI) ? layer_size_ksi_ : layer_size_phi_;
  void* sat_solver = (type == PHI) ? sat_solver_ksi_ : sat_solver_phi_;
  vector<LitTuple>& to_add_container = (type == PHI) ? cex_to_add_ : wit_to_add_;
  LitTupleSet& cache = (type == PHI) ? counterexample_cache_ : witness_cache_;
  vector<Var>* base = (type == PHI) ? &tuple_base_ksi_ : &tuple_base_phi_;
  LitTuple2Uint candidates;
  
  vector<bool> addt;
  addt.reserve(layer_size);
  
  LitTuple addition(base);
  unsigned long added_layers = 0;

  // prepare extension
  for(long unsigned int layer_number = 0; added_layers < per_call && layer_number != layers.size(); layer_number += 1)
  {
    vector<Var>& layer = layers[layer_number];
    debugn("Layer number is " << layer_number);
    addt.clear();
    for (unsigned i = 0; i < layer_size; i++)
    {
      addt.push_back(sign((Lit) ipasir_val(sat_solver, layer[i])));
    }
    
    addition.assign(addt);
    
    if (cache.find(addition) != cache.end())
    {
      debugn("cache worked...");
      continue;
    }

    cache.insert(addition);

    added_layers++;
    to_add_container.push_back(addition);
  }

  debugn("computeExtension " << ((type == PHI) ? "PHI" : "KSI") << ": end");
}

void MySolver::extendKSI()
{
  debugn("extendKSI: begin");
  
  vector<bool> ext;
  ext.reserve(layer_size_phi_);
  
  LitTuple ext_key(nullptr);
  
  LitTuple layer_clause(nullptr);
  
  bool trim_branch = (options_.trimming_ksi == SolverOptions::TrimmingMode::BRANCH_TRIMMING);
  if (trim_branch && independent_phi_.size() != 0 && layers_phi_.size() != 0)
  {
    vector<bool> signs;
    // ksi indices for top will have to start from 1
    LitTuple assignment(&independent_phi_);
    
    for (const Var& v : independent_phi_)
      signs.push_back(sign(ipasir_val(sat_solver_phi_, v)));
    
    assignment.assign(signs);
    
    if (current_top_phi_.isAssigned())
      trim_branch = !(assignment == current_top_phi_);
    
    if (trim_branch || !current_top_phi_.isAssigned())
      current_top_phi_.assign(signs);
  }
  
  unsigned long old_wpc = 0;
  
  if (trim_branch)
    old_wpc = trimKSI();
  
  computeExtension(KSI);
  
  // restore wpc
  if(trim_branch)
    options_.cex_per_call = old_wpc;
  
  for (auto& wit_it : wit_to_add_)
  {
    bool cache_possible = true;
    bool skip = false;
    
    ext.clear();
    
    layers_ksi_.push_back(empty_layer_ksi_);
    
    vector<Var>& layer_k = layers_ksi_.back();

    for (unsigned quant_index = 0; quant_index < formula_->getNumQuants(); quant_index++)
    {
      const Quantification& quant = formula_->getQuant(quant_index);
      const Quantification& quant_inc = formula_->getCumulativeQuant(quant_index);

      if (quant.first == EXISTENTIAL)
      {
        skip = false;
        for (const Var& v : quant.second)
          ext.push_back(sign(wit_it[layerIndexPHI(v)]));
        ext_key.rebase(&(quant_inc.second));
        if (cache_possible)
        {
          ext_key.assign(ext);
          const auto cache_iter = vars_cache_ksi_.find(ext_key);
          if (cache_iter != vars_cache_ksi_.end())
          {
            debugn("We have a variable cache hit: ");

            debug
            ({
               cout << ext_key.getHash();
               cout << "key : < ";
               for (const bool& l : ext)
               { cout << l << " "; }
               cout << "> data: <";
               for (const Var& v : cache_iter->second)
               { cout << v << " "; }
               cout << ">\n";
             });

            vector<Var>::iterator subst_iter = cache_iter->second.begin();
            for (const Var& v : formula_->getQuant(quant_index + 1).second)
            {
              layer_k[layerIndexKSI(v)] = *subst_iter;
              subst_iter++;
            }
            skip = true;
          }
          else
            cache_possible = false;
        }
      }
      else if (!skip) // && quant->first == UNIVERSIAL
      {
        vector<Var> subst;
        for (const Var& v : quant.second)
        {
          subst.push_back(newVarKSI());
          layer_k[layerIndexKSI(v)] = subst.back();
        }

        ext_key.assign(ext);
        // add to variable cache
        debug
        ({
           cout << ext_key.getHash();
           cout << " caching key: < ";
           for (const bool& l : ext)
           { cout << l << " "; }
           cout << "> data: <";
           for (const Var& v : subst)
           { cout << v << " "; }
           cout << ">\n";
         });
        vars_cache_ksi_[ext_key] = subst;
      }
    }
    
    debugn("Layer:\n");
    vector<Lit> global_nand;
    vector<Lit> literal_clause;
    literal_clause.reserve(2);
    vector<bool> layer_cl_sign;
    
    for (unsigned clause_num = 0; clause_num < formula_->getNumClauses(); clause_num++)
    {
      bool satisfied = false;
      for (const Lit& literal : formula_->getClausePHI(clause_num))
      {
        if (sign(literal) == sign(wit_it[layerIndexPHI(var(literal))]))
        {
          satisfied = true;
          break;
        }
      }
      
      if (satisfied) continue;
  
      vector<Var>* layer_cl_var = new vector<Var>();
      layer_cl_sign.clear();
  
      if(options_.tseitin_optimisation && formula_->getClauseKSI(clause_num).size() == 1)
      {
        Lit literal = formula_->getClauseKSI(clause_num)[0];
        global_nand.push_back(mkLit(layer_k[layerIndexKSI(var(literal))],!sign(literal)));
      }
      else
      {
        for (const Lit& literal : formula_->getClauseKSI(clause_num))
        {
          layer_cl_var->push_back(layer_k[layerIndexKSI(var(literal))]);
          layer_cl_sign.push_back(sign(literal));
        }
        
        // layer_caluese_bases_.push_back(layer_cl_var);
        
        layer_clause.rebase(layer_cl_var);
        layer_clause.assign(layer_cl_sign);
        
        const auto cache_iter = tseitin_cache_.find(layer_clause);
        if (cache_iter != tseitin_cache_.end())
        {
          // debugn("cache worked...");
          delete layer_cl_var;
          global_nand.push_back(cache_iter->second);
          continue;
        }
    
        layer_caluese_bases_.push_back(layer_cl_var);
        
        Lit x_i = mkLit(newVarKSI(), true);
        
        global_nand.push_back(x_i);
        tseitin_cache_[layer_clause] = x_i;
        
        literal_clause.clear();
        literal_clause.push_back(negateLit(x_i));
        literal_clause.push_back(mkLit(0));
        
        for (unsigned lit_index = 0; lit_index < layer_clause.size(); lit_index++)
        {
          literal_clause[1] = negateLit(layer_clause[lit_index]);

          addClause(sat_solver_ksi_, literal_clause);
          debugn("extendKSI: " << literal_clause);

        }
      }
    }
    addClause(sat_solver_ksi_, global_nand);
    debugn("extendKSI: " << global_nand);
  }
  
  wit_to_add_.clear();
  
  debugn("extendKSI: end");
}

unsigned long MySolver::trimPHI()
{
  debugn("triming formula PHI");
  ipasir_release(sat_solver_phi_);
  sat_solver_phi_ = ipasir_init();
  
  counterexample_cache_.clear();
  
  vars_cache_phi_.clear();
  num_vars_phi_ = 0;
  num_clauses_phi_ = 0;
  origins_phi_.clear();
  tmp_phi_clauses_.close();
  tmp_phi_clauses_.open(options_.tmp_phi_log, std::ofstream::out);
  
  pure_phi_.clear();
  
  for (unsigned i = 1; i <= independent_phi_.size(); i++)
  {
    newVarPHI();
  }
  layers_phi_.clear();
  unsigned long old_cpc = options_.cex_per_call;
  options_.cex_per_call = (unsigned long)-1;
  return old_cpc;
}

unsigned long MySolver::trimKSI()
{
  debugn("triming formula KSI");
  ipasir_release(sat_solver_ksi_);
  sat_solver_ksi_ = ipasir_init();
  
  witness_cache_.clear();
  
  vars_cache_ksi_.clear();
  tseitin_cache_.clear();
  num_vars_ksi_ = 0;
  num_clauses_ksi_ = 0;
  origins_ksi_.clear();
  tmp_ksi_clauses_.close();
  tmp_ksi_clauses_.open(options_.tmp_ksi_log, std::ofstream::out);
  
  pure_ksi_.clear();
  layers_ksi_.clear();
  unsigned long old_wpc = options_.wit_per_call;
  options_.wit_per_call = (unsigned long)-1;
  return old_wpc;
}

void MySolver::extendPHI()
{
  debugn("extendPHI: begin");
  
  vector<bool> uni;
  uni.reserve(layer_size_ksi_);
  
  vector<vector<Lit>*> pending_clauses;
  
  LitTuple uni_key(nullptr);
  
  cout << "c Number of calls is: " << sat_calls_ << endl
       << "c Layers in PHI: " << layers_phi_.size() << endl
       << "c Layers in KSI: " << layers_ksi_.size() << endl;
  
  bool trim_periodic = options_.trimming_phi == SolverOptions::TrimmingMode::PERIODIC_TRIMMING &&
      (sat_calls_ % options_.trimming_interval == 1);
  bool trim_dynamic = (options_.trimming_phi == SolverOptions::TrimmingMode::DYNAMIC_TRIMMING && (
      (read_mem_usage() > options_.memory_limit) || (sat_calls_ % options_.trimming_interval == 1) ||
      (sat_calls_ > 0 && layers_phi_.size() > 1.2 * layers_ksi_.size())));
  
  bool trim_branch = (options_.trimming_phi == SolverOptions::TrimmingMode::BRANCH_TRIMMING);
  if( trim_branch && independent_phi_.size() == 0 && formula_->getNumQuants() != 3 && layers_ksi_.size() != 0)
  {
    vector<bool> signs;
    // ksi indices for top will have to start from 1
    LitTuple assignment(&top_ksi_base_);
  
    for (const Var& v : top_ksi_base_)
      signs.push_back(sign(ipasir_val(sat_solver_ksi_, v)));
  
    assignment.assign(signs);
  
    if (current_top_ksi_.isAssigned())
      trim_branch = !(assignment == current_top_ksi_);
    
    if(trim_branch || !current_top_ksi_.isAssigned())
      current_top_ksi_.assign(signs);
    
    trim_branch &= sat_calls_ % 3;
  }
  
  unsigned long old_cpc = 0;
  
  if(trim_dynamic)
    cout << "<dynamic trimming> memory usage: " << read_mem_usage() << endl;
  
  if(trim_branch)
    cout << "Assignment to KSI changed, trimming PHI" << endl;
  
  if(trim_periodic || trim_dynamic || trim_branch)
  {
    // save cpc
    old_cpc = trimPHI();
  }
  
  computeExtension(PHI);
  
  // restore cpc
  if(trim_periodic || trim_dynamic || trim_branch)
  {
    options_.cex_per_call = old_cpc;
  }
  
  for (auto& cex_it : cex_to_add_)
  {
    // counterexample_cache_.insert(cex_it.second);
    
    bool cache_possible = true;
    bool skip = false;
    
    uni.clear();
    
    layers_phi_.push_back(empty_layer_phi_);
    
    vector<Var>& layer_p = layers_phi_.back();
    
    for (unsigned int i = 0; i < independent_phi_.size(); i++)
    {
      layer_p[layerIndexPHI(independent_phi_[i])] = i + 1;
      if(options_.bumping)
        ipasir_bump(sat_solver_phi_, i+1);
    }
    
    int deepest_match = 0;

    // add a new layer of existentials
    // if a value assignment for universals that are upstream
    // from some existentials was already encountered once, then
    // reuse the same existential substitutions, to boost performance
    // otherwise, create new variables as substitutions and add them
    // to both layer and cache
    for (unsigned quant_index = 1; quant_index < formula_->getNumQuants(); quant_index++)
    {
      const Quantification& quant = formula_->getQuant(quant_index);
      const Quantification& quant_inc = formula_->getCumulativeQuant(quant_index);

      if (quant.first == UNIVERSAL)
      {
        skip = false;
        for (const Var& v : quant.second)
          uni.push_back(sign(cex_it[layerIndexKSI(v)]));
        uni_key.rebase(&(quant_inc.second));
        if (cache_possible)
        {
          uni_key.assign(uni);
          const auto cache_iter = vars_cache_phi_.find(uni_key);
          if (cache_iter != vars_cache_phi_.end())
          {
            deepest_match = quant_index + 1;
            debugn("We have a variable cache hit: " << quant_index + 1);
            debug
            ({
               cout << uni_key.getHash();
               cout << " key : < ";
               for (const bool& l : uni)
               { cout << l << " "; }
               cout << ">data: <";
               for (const Var& v : cache_iter->second)
               { cout << v << " "; }
               cout << ">\n";
             });

            vector<Var>::iterator subst_iter = cache_iter->second.begin();
            for (const Var& v : formula_->getQuant(quant_index + 1).second)
            {
              layer_p[layerIndexPHI(v)] = *subst_iter;
              subst_iter++;
            }
            skip = true;
          }
          else
            cache_possible = false;
        }
      }
      else if (!skip) // && quant.first == EXISTENTIAL
      {
        vector<Var> subst;
        for (const Var& v : quant.second)
        {
          subst.push_back(newVarPHI());
          layer_p[layerIndexPHI(v)] = subst.back();
        }

        uni_key.assign(uni);

        debug
        ({
           cout << uni_key.getHash();
           cout << " caching key: < ";
           for (const bool& l : uni)
           { cout << l << " "; }
           cout << "> data: <";
           for (const Var& v : subst)
           { cout << v << " "; }
           cout << ">\n";
         });

        // add to variable cache
        vars_cache_phi_[uni_key] = subst;
      }
    }
    
    vector<Lit> cl;
    // LitTuple clause_key;
    cl.reserve(layer_size_phi_);
    for (unsigned clause_num = 0; clause_num < formula_->getNumClauses(); clause_num++)
    {
      bool satisfied = false;
      for (const Lit& literal : formula_->getClauseKSI(clause_num))
      {
        if (sign(literal) == sign(cex_it[layerIndexKSI(var(literal))]))
        {
          satisfied = true;
          break;
        }
      }
      
      if (satisfied) continue;
      
      if (formula_->getClauseKSI(clause_num).empty() && !formula_->getClausePHI(clause_num).empty() &&
          layers_phi_.size() > 1 && formula_->getDepthPHI(clause_num) <= deepest_match)
      {
        debugn("skipped adding duplicate clause in PHI");
        continue;
      }
      
      cl.clear();
      for (const Lit& literal : formula_->getClausePHI(clause_num))
      {
          cl.push_back(mkLit(layer_p[layerIndexPHI(var(literal))], sign(literal)));
      }
      
      if (satisfied) continue;

      origins_phi_.push_back(clause_num + 1);
      addClause(sat_solver_phi_, cl);
      debugn("extendPHI: " << cl);

    }
  }
  
  cex_to_add_.clear();
  debugn("extendPHI: end");
}

void MySolver::addClause(void* solver, const vector<Lit>& clause)
{
  if(options_.logging_phi && solver == sat_solver_phi_)
  {
    for (const Lit& l : clause)
    {
      tmp_phi_clauses_ << l << " ";
    }
    tmp_phi_clauses_ << "0\n" << std::flush;
    num_clauses_phi_ += 1;
  }
  else if(options_.logging_ksi && solver == sat_solver_ksi_)
  {
    for (const Lit& l : clause)
    {
      tmp_ksi_clauses_ << l << " ";
    }
    tmp_ksi_clauses_ << "0\n" << std::flush;
    num_clauses_ksi_ += 1;
  }
  
  for (const Lit& l : clause)
  {
    ipasir_add(solver, l);
  }
  ipasir_add(solver, 0);
}

void MySolver::printVersion()
{
  cout << "c MySolver" << VERSION << std::endl;
}

void MySolver::renameVariables(Prefix& pref, const CNF& cnf)
{
  // Rename the variables to eliminate the need for lookup tables.
  // The renaming scheme will go through the variables and rename
  // existentials to uneven and universals to even ids.
  
  // find size of renaming scheme lookup
  
  unsigned renaming_size[2];
  renaming_size[0] = 0;
  renaming_size[1] = 0;
  
  max_id_ = 0;
  for (const Quantification& q : pref)
  {
    unsigned* x = (q.first == EXISTENTIAL) ? &(renaming_size[0]) : &(renaming_size[1]);
    *x += q.second.size();
    for (const Var& v : q.second)
    {
      if (v > max_id_)
        max_id_ = v;
    }
  }
  
  renaming_scheme_.resize(2 * std::max(renaming_size[0], renaming_size[1]) + 1);
  
  formula_->setVariableDepthSize(2 * std::max(renaming_size[0], renaming_size[1]) + 1);
  names_phi_.resize((unsigned long) max_id_ + 1, (Var) -1);
  names_ksi_.resize((unsigned long) max_id_ + 1, (Var) -1);
  
  layer_size_phi_ = renaming_size[0];
  layer_size_ksi_ = renaming_size[1];
  
  // go through the variables as they appear in the prefix and assign new name
  // names
  Var e = 1;
  Var a = 2;
  // alternation
  int e_l = 0;
  int a_l = 1;
  
  Quantification quant_inc_e;
  Quantification quant_inc_a;
  
  quant_inc_e.first = EXISTENTIAL;
  quant_inc_a.first = UNIVERSAL;
  
  for (const Quantification& q : pref)
  {
    Quantification quant;
    quant.first = q.first;
    
    Quantification& quant_inc = (q.first == EXISTENTIAL) ? quant_inc_e : quant_inc_a;
    
    Var& x = (q.first == EXISTENTIAL) ? e : a;
    int& x_l = (q.first == EXISTENTIAL) ? e_l : a_l;
    vector<Var>& names = (q.first == EXISTENTIAL) ? names_phi_ : names_ksi_;
    for (const Var& v : q.second)
    {
      renaming_scheme_[x] = v;
      formula_->setVariableDepth(x, x_l);
      names[v] = x;
      quant.second.push_back(x);
      quant_inc.second.push_back(x);
      x += 2;
    }
    x_l+=2;
    
    formula_->addQuant(quant);
    formula_->addCumulativeQuant(quant_inc);
  }
  
  // rename the literals in the clauses
  vector<Lit> clause_part_phi;
  vector<Lit> clause_part_ksi;
  vector<Lit> clause_whole;
  
  for (const vector<Lit>& clause : cnf)
  {
    clause_part_phi.clear();
    clause_part_ksi.clear();
    clause_whole.clear();
    
    for (const Lit& literal : clause)
    {
      Var v = var(literal);
      
      if (names_phi_[v] != (Var) -1)
      {
        clause_part_phi.push_back(mkLit(names_phi_[v], sign(literal)));
      }
      else if (names_ksi_[v] != (Var) -1)
      {
        clause_part_ksi.push_back(mkLit(names_ksi_[v], sign(literal)));
      }
    }
    
    formula_->addClause(clause_part_phi, clause_part_ksi, clause_whole);
  }
  
  independent_phi_ = formula_->getQuant(0).second;
}