/*
 * This file is part of HQSpre.
 *
 * Copyright 2016/17 Ralf Wimmer, Sven Reimer, Paolo Marin, Bernd Becker
 * Albert-Ludwigs-Universitaet Freiburg, Freiburg im Breisgau, Germany
 *
 * HQSpre is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * HQSpre is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with HQSpre. If not, see <http://www.gnu.org/licenses/>.
 */

// $Id: main.cpp 1696 2017-09-06 09:25:40Z wimmer $

#include <cstdlib>
#include <csignal>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <set>
#include <iterator>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/program_options.hpp>
#include "../libs/easylogging/easylogging++.hpp"
#include "aux.hpp"
#include "formula.hpp"

INITIALIZE_EASYLOGGINGPP

using namespace hqspre;

const static std::string sat = "p cnf 0 0\n";
const static std::string unsat = "p cnf 0 1\n0\n";


// We need the processed formula as a global variable.
// Otherwise we cannot access it using signal handlers.
Formula dqbf;


void signalHandler(int /* signum*/ )
{
    dqbf.setInterrupt(true);

    // Remove the signal handler.
    std::signal(SIGINT, nullptr);
    std::signal(SIGUSR1, nullptr);
}


int main(int argc, char** argv)
{
    // Configure logging
    el::Configurations defaultConf;
    defaultConf.setToDefault();
    defaultConf.setGlobally(el::ConfigurationType::Enabled, "true");
    defaultConf.setGlobally(el::ConfigurationType::Format, "[%level] %msg");
    defaultConf.set(el::Level::Verbose, el::ConfigurationType::Format, "[%level-%vlevel] %msg");
    defaultConf.setGlobally(el::ConfigurationType::ToFile, "false");
    defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "true");
#ifdef ELPP_DISABLE_VERBOSE_LOGS
    defaultConf.set(el::Level::Verbose, el::ConfigurationType::ToStandardOutput, "false");
#endif


    // Get default values for preprocessor options
    Settings setting = dqbf.getSettings();
    std::string in_name("");
    std::string log_name("");
    std::string out_name("");

    // Preprocessing options
    bool compress_output = true;
#ifdef linux
    unsigned int timeout = 0;
#endif
    bool use_limits = true;

    // Declaration of parameters:
    boost::program_options::options_description public_options("Options");
    public_options.add_options()
      ("help,h", "Show available options")
      ("dqbf",      boost::program_options::value<bool>(&setting.force_dqbf),      "Treat the formula always as a DQBF")
      ("verbosity,v",   boost::program_options::value<el::base::type::VerboseLevel>(&setting.verbosity)->default_value(setting.verbosity),"Set verbosity of the preprocessor")
#ifdef linux
      ("timeout",     boost::program_options::value<unsigned int>(&timeout)->default_value(timeout),  "Set a time limit (0 = none)")
#endif
      ("outfile,o", boost::program_options::value<std::string>(&out_name), "Write the result to the given file")
      ("logfile", boost::program_options::value<std::string>(&log_name), "Write the output to a log file")
      ("compress", boost::program_options::value<bool>(&compress_output)->default_value(true), "Rename variables in the output file")
      ("pipe", "Suppress status messages and print the formula to the standard output")
    ;

    boost::program_options::options_description pre_options("Preprocessor options");
    pre_options.add_options()
      ("max_loops",   boost::program_options::value<std::size_t>(&setting.max_loops)->default_value(setting.max_loops), "Set maximal number of preprocessor loops")
      ("univ_reduct", boost::program_options::value<bool>(&setting.univ_reduction)->default_value(setting.univ_reduction),   "Use universal reduction")
      ("bce",         boost::program_options::value<unsigned int>(&setting.bce)->default_value(setting.bce),      "Use blocked clause elimination, 0: no, 1: old, 2: new")
      ("hidden",      boost::program_options::value<unsigned int>(&setting.hidden)->default_value(setting.hidden),   "Add hidden literals before BCE, 0: no, 1: incomplete, 2: complete")
      ("covered",     boost::program_options::value<bool>(&setting.covered)->default_value(setting.covered),      "Add covered literals before BCE")
      ("ble",         boost::program_options::value<bool>(&setting.ble)->default_value(setting.ble),   "Use blocked literal elimination for universal literals")
      ("bla",         boost::program_options::value<bool>(&setting.bla)->default_value(setting.bla),   "Use blocked literal addition for universal literals")
      ("bia",         boost::program_options::value<bool>(&setting.bia)->default_value(setting.bia),  "Use blocked implication addition")
      ("max_clause_size", boost::program_options::value<std::size_t>(&setting.max_clause_size)->default_value(setting.max_clause_size), "Maximal size of clause using hidden and covered literals")
      ("hse",         boost::program_options::value<bool>(&setting.hse)->default_value(setting.hse),           "Use hidden subsumption elimination")
      ("hec",         boost::program_options::value<bool>(&setting.hec)->default_value(setting.hec),           "Find hidden equivalences and contradictions")
      ("ic",          boost::program_options::value<bool>(&setting.impl_chains)->default_value(setting.impl_chains),   "Eliminate implication chains")
      ("contra",      boost::program_options::value<bool>(&setting.contradictions)->default_value(setting.contradictions),        "Find contradictions")
      ("substitute",  boost::program_options::value<bool>(&setting.substitution)->default_value(setting.substitution),    "Use gate substitution")
      ("equiv_gates", boost::program_options::value<bool>(&setting.equiv_gates)->default_value(setting.equiv_gates), "Detect structurally equivalent gates")
      ("sem_gates",   boost::program_options::value<bool>(&setting.semantic_gates)->default_value(setting.semantic_gates), "Use SAT-based semantic gate detection")
      ("extend_gates",boost::program_options::value<bool>(&setting.extend_gates)->default_value(setting.extend_gates), "Add blocked or transitive implications to find more AND gates")
      ("max_substitution_cost",  boost::program_options::value<int>(&setting.max_substitution_cost)->default_value(setting.max_substitution_cost), "Maximal substitution costs")
      ("max_substitution_loops", boost::program_options::value<std::size_t>(&setting.max_substitution_loops)->default_value(setting.max_substitution_loops), "Maximal substitution loops")
      ("rewrite",     boost::program_options::value<bool>(&setting.rewrite)->default_value(setting.rewrite),      "Use gate rewriting")
      ("self_sub",    boost::program_options::value<bool>(&setting.self_subsumption)->default_value(setting.self_subsumption),      "Eliminate self-subsuming literals")
      ("subsumption", boost::program_options::value<bool>(&setting.subsumption)->default_value(setting.subsumption),   "Eliminate subsumed clauses")
      ("resolution",  boost::program_options::value<bool>(&setting.resolution)->default_value(setting.resolution),    "Eliminate variables by resolution")
      ("max_resolution_cost", boost::program_options::value<int>(&setting.max_resolution_cost)->default_value(setting.max_resolution_cost), "Maximal resolution costs")
      ("sat_const",   boost::program_options::value<bool>(&setting.sat_const)->default_value(setting.sat_const),     "Detect constants with SAT-based techniques")
      ("sat_impl",    boost::program_options::value<bool>(&setting.sat_impl)->default_value(setting.sat_impl),      "Detect implications with SAT-based techniques")
      ("pure_sat_timeout", boost::program_options::value<unsigned int>(&setting.pure_sat_timeout)->default_value(20), "Timeout for solving pure SAT instances")
      ("univ_exp",    boost::program_options::value<unsigned int>(&setting.univ_expand)->default_value(setting.univ_expand),   "Apply universal expansion")
      ("incomplete",  boost::program_options::value<bool>(&setting.sat_incomplete)->default_value(setting.sat_incomplete), "Apply incomplete decision procedures")
      ("random_patterns", boost::program_options::value<std::size_t>(&setting.num_random_patterns)->default_value(setting.num_random_patterns), "Number of random patterns for incomplete UNSAT checks")
      ("cons_check",  boost::program_options::value<bool>(&setting.do_consistency_check)->default_value(setting.do_consistency_check),    "Enable/disable consistency checks in preprocessor")
      ("use_limits",  boost::program_options::value<bool>(&use_limits)->default_value(use_limits),    "Enable/disable process limits")
      ("preserve_gates",  boost::program_options::value<bool>(&setting.preserve_gates)->default_value(setting.preserve_gates),    "Enable/disable gates protection")
    ;

    boost::program_options::options_description hidden_options("Options");
    hidden_options.add_options()
      ("infile,i", boost::program_options::value<std::string>(&in_name), "Input file name")
    ;

    boost::program_options::options_description all_options("All options");
    all_options.add(public_options).add(hidden_options).add(pre_options);

    boost::program_options::positional_options_description p;
    p.add("infile", -1);

    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(all_options).positional(p).run(), vm);
    boost::program_options::notify(vm);

    if (vm.count("pipe")) {
        setting.verbosity = 0;
    }

    if (setting.verbosity > 0) {
        std::cout << "\n\nThis is the HQS Preprocessor for QBF and DQBF, compiled on " << __DATE__ << " at " << __TIME__ << std::endl << std::endl;
#ifndef NDEBUG
        std::cout << "This is the debug version.\n";
#endif
    }

    // Evaluation of parameters
    if (vm.count("help")) {
        std::cout << "Call with\n  " << argv[0] << "<options> <input file>\n\n" << public_options << "\n" << pre_options << "\n";
        std::exit(0);
    }

    if (vm.count("logfile")) {
        defaultConf.setGlobally(el::ConfigurationType::ToFile, "true");
        defaultConf.setGlobally(el::ConfigurationType::Filename, log_name);
    }

    if (vm.count("pipe")) {
        defaultConf.setGlobally(el::ConfigurationType::ToStandardOutput, "false");
    }

    el::Loggers::reconfigureAllLoggers(defaultConf);
    el::Loggers::setVerboseLevel(setting.verbosity);


    if (in_name == "") {
        LOG(ERROR) << "No input file given.";
        std::exit(-1);
    }


    std::signal(SIGINT, signalHandler);

    // Now commit all settings to the solver
    dqbf.setSettings(setting);
    dqbf.useProcessLimits(use_limits);

    VLOG(1) << "Input file: " << in_name;

    const std::string in_ext = in_name.substr(in_name.length() - 3, 3);
    try {
        if (in_ext == ".gz" || in_ext == ".GZ") {
            std::ifstream in(in_name, std::ios_base::in | std::ios_base::binary);
            if (!in) {
                LOG(ERROR) << "Could not open input file '" << in_name << "'!";
                std::exit(-1);
            }
            boost::iostreams::filtering_istream in_filter;
            in_filter.push(boost::iostreams::gzip_decompressor());
            in_filter.push(in);
            in_filter >> dqbf;
            in.close();
        } else {
            std::ifstream in(in_name);
            if (!in) {
                LOG(ERROR) << "Could not open input file '" << in_name << "'!";
                std::exit(-1);
            }
            in >> dqbf;
            in.close();
        }

#ifdef linux
        if (timeout > 0) createTimeout(timeout, signalHandler);
#endif
        val_assert(dqbf.checkConsistency());

        const auto exist_vars = dqbf.numEVars();
        const auto univ_vars = dqbf.numUVars();
        const auto clauses = dqbf.numClauses();
        const auto dependencies = dqbf.numDependencies();
        const auto literals = dqbf.numLiterals();

        // Apply basic preprocessing.
        dqbf.preprocess();

        val_assert(dqbf.checkConsistency());

        std::signal(SIGINT, nullptr);
        std::signal(SIGUSR1, nullptr);


        // Output some statistics.
        if (setting.verbosity > 0) {
            dqbf.printStatistics();
            std::cout << std::endl;
            std::cout << "c preprocess -> after / before" << std::endl;
            std::cout << "c exist_vars = " << dqbf.numEVars() << " / " << exist_vars << std::endl;
            std::cout << "c univ_vars  = " << dqbf.numUVars() << " / " << univ_vars << std::endl;
            std::cout << "c literals   = " << dqbf.numLiterals() << " / " << literals << std::endl;
            std::cout << "c clauses    = " << dqbf.numClauses() << " / " << clauses <<  std::endl;
            std::cout << "c dependies  = " << dqbf.numDependencies() << " / " << dependencies << std::endl;
            std::cout << "c maxVarIndex() = " << dqbf.maxVarIndex() << std::endl;
        }
    } catch (UNSATException& e) {

        dqbf.printStatistics();
        LOG(INFO) << "Preprocessor determined unsatisfiability.";
        VLOG(1) << e.what();
        if (setting.verbosity > 0 && !vm.count("pipe")) std::cout << "UNSAT" << std::endl;

        if (vm.count("pipe")) {
            std::cout << unsat;
            return 0;
        }

        std::ofstream out;
        if (out_name != "") {
            const std::string out_ext = out_name.substr(out_name.length() - 3, 3);
            boost::iostreams::filtering_ostream out_filter;
            if (out_ext == ".gz" || out_ext == ".GZ") {
                out.open(out_name, std::ios_base::out | std::ios_base::binary);
                if (!out) {
                    LOG(ERROR) << "Could not open output file '" << out_name << "'!";
                    std::exit(-1);
                }
                out_filter.push(boost::iostreams::gzip_compressor());
            } else {
                out.open(out_name);
                if (!out) {
                    LOG(ERROR) << "Could not open output file '" << out_name << "'!";
                    std::exit(-1);
                }
            }
            out_filter.push(out);
            out_filter << unsat;
        }
        return 20;
    } catch (SATException& e) {

        dqbf.printStatistics();
        LOG(INFO) << "Preprocessor determined satisfiability.";
        VLOG(1) << e.what();
        if (setting.verbosity == 0 && !vm.count("pipe")) std::cout << "SAT" << std::endl;

        if (vm.count("pipe")) {
            std::cout << sat;
            return 0;
        }

        std::ofstream out;
        if (out_name != "") {
            const std::string ext = out_name.substr(out_name.length() - 3, 3);
            boost::iostreams::filtering_ostream out_filter;
            if (ext == ".gz" || ext == ".GZ") {
                out.open(out_name, std::ios_base::out | std::ios_base::binary);
                if (!out) {
                    LOG(ERROR) << "Could not open output file '" << out_name << "'!";
                    std::exit(-1);
                }
                out_filter.push(boost::iostreams::gzip_compressor());
            } else {
                out.open(out_name);
                if (!out) {
                    LOG(ERROR) << "Could not open output file '" << out_name << "'!";
                    std::exit(-1);
                }
            }
            out_filter.push(out);
            out_filter << sat;
        }
        return 10;
    }

    if (setting.verbosity > 0) { LOG(INFO) << "Preprocessor could not solve the formula."; }
    if (setting.verbosity == 0 && !vm.count("pipe")) std::cout << "UNKNOWN\n";

    if (vm.count("pipe")) {
        dqbf.write(std::cout, compress_output);
        return 0;
    }

    if (out_name != "") {
        const std::string ext = out_name.substr(out_name.length() - 3, 3);
        if (ext == ".gz" || ext == ".GZ") {
            std::ofstream out(out_name, std::ios_base::out | std::ios_base::binary);
            if (!out) {
                LOG(ERROR) << "Could not open output file '" << out_name << "'!";
                std::exit(-1);
            }
            boost::iostreams::filtering_ostream out_filter;
            out_filter.push(boost::iostreams::gzip_compressor());
            out_filter.push(out);
            dqbf.write(out_filter, compress_output);
        } else {
            std::ofstream out(out_name);
            if (!out) {
                LOG(ERROR) << "Could not open output file '" << out_name << "'!";
                std::exit(-1);
            }
            dqbf.write(out, compress_output);
        }
    }

    return 0;
}
