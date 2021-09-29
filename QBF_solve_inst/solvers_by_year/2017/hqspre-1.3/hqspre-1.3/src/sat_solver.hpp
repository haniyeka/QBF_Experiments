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

#ifndef HQSPRE_SAT_SOLVER_HPP_
#define HQSPRE_SAT_SOLVER_HPP_

#include <vector>

#include "../libs/antom/solver/antombase.h"

extern "C" {
#include <picosat/picosat.h>
}
#include "aux.hpp"
#include "literal.hpp"
#include "clause.hpp"
#include "timer.hpp"

/**
 * \file sat_solver.hpp
 * \author Ralf Wimmer
 * \date 2016-08
 * \brief Implementation of a consistent interface for different SAT solvers.
 */

namespace hqspre {

/**
 * \brief Base class for different SAT solvers.
 *
 * It provides a common interface to solvers like Antom, Picosat, and Lingeling.
 * The interface is taylored towards the usage in the HQSpre preprocessor and
 * does not provide access to all supported functions.
 */
class SatSolverInterface
{
public:
    SatSolverInterface() = default;
    SatSolverInterface(const SatSolverInterface&) = delete;
    SatSolverInterface(SatSolverInterface&&) = default;
    virtual ~SatSolverInterface() noexcept = default;
    SatSolverInterface& operator=(const SatSolverInterface&) = delete;
    SatSolverInterface& operator=(SatSolverInterface&&) = default;

    /**
     * \brief Gives the solver a hint on the number of used variables.
     */
    virtual void setMaxIndex(Variable var) = 0;

    /**
     * \brief Adds a clause to the SAT solver's clause database.
     */
    virtual void addClause(const std::vector<Literal>& clause) = 0;

    /**
     * \brief Adds a unit clause to the SAT solver's clause database.
     */
    virtual bool addUnit(Literal lit) = 0;

    /**
     * \brief Instructs the decision heuristics of the SAT solver how to assign variables first.
     *
     * If `value` is true, then the decision heuristics tries value `true` first
     * when assigning `var`, otherwise `false`.
     */
    virtual void setPolarity(Variable var, bool value) = 0;

    /**
     * \brief Sets a time limit (in seconds) for the solution of the formula.
     */
    virtual void setTimeout(double time) = 0;

    /**
     * \brief Solves the formula.
     * \return TruthValue::TRUE if the formula is satisfiable, TruthValue::FALSE if it
     *         is unsatisfiable, and TruthValue::UNKNOWN if the formula could not be solved.
     */
    virtual TruthValue solve() = 0;

    /**
     * \brief Solves the formula under the given assumptions.
     * \return TruthValue::TRUE if the formula is satisfiable, TruthValue::FALSE if it
     *         is unsatisfiable, and TruthValue::UNKNOWN if the formula could not be solved.
     */
    virtual TruthValue solve(const std::vector<Literal>& assumptions) = 0;


    /**
     * \brief Returns the value of the given variable in the computed satisfying assignment.
     * \pre SatSolver::solve must be called before and the formula must have been determined
     *      to be satisfiable. Otherwise the behavior of this function is undefined.
     */
    virtual TruthValue getValue(Variable var) = 0;

private:
};


/**
 * \brief An interface for the Antom SAT solver by Tobias Schubert and Sven Reimer
 */
class Antom: public SatSolverInterface
{
public:
    Antom():
        SatSolverInterface(),
        _solver()
    {}

    virtual ~Antom() noexcept = default;

    virtual void setMaxIndex(const Variable var) override
    {
        _solver.SetMaxIndex(var);
    }

    virtual void addClause(const std::vector<Literal>& clause) override
    {
        // Due to the strange interface of Antom, we need a const_cast here ...
        // WARNING: Antom may change the order of literals in the passed clause
        //          and remove duplicates!
        _solver.AddClause(const_cast<std::vector<Literal>&>(clause));
    }

    virtual bool addUnit(const Literal lit) override
    {
        return _solver.AddUnit(lit);
    }

    virtual void setPolarity(const Variable var, const bool value) override
    {
        // force solver to take a certain polarity first.
        // strategy = 3 => set always true, strategy = 2 => set always false
        const unsigned int strategy = (value ? 3 : 2);
        _solver.SetDecisionStrategyForVariable(strategy, var);
    }

    virtual void setTimeout(const double time) override
    {
        _solver.SetCPULimit(time);
    }

    virtual TruthValue solve(const std::vector<Literal>& assumptions) override
    {
        const auto value = _solver.Solve(assumptions);
        if (value == ANTOM_UNSAT) return TruthValue::FALSE;
        else if (value == ANTOM_SAT) return TruthValue::TRUE;
        else return TruthValue::UNKNOWN;
    }

    virtual TruthValue solve() override
    {
        const auto value = _solver.Solve();
        if (value == ANTOM_UNSAT) return TruthValue::FALSE;
        else if (value == ANTOM_SAT) return TruthValue::TRUE;
        else return TruthValue::UNKNOWN;
    }

    virtual TruthValue getValue(const Variable var) override
    {
        if (_solver.Model()[var] == 0) return TruthValue::UNKNOWN;
        else if (isNegative(_solver.Model()[var])) return TruthValue::FALSE;
        else return TruthValue::TRUE;
    }

private:
    antom::AntomBase _solver;
};


static Timer picosat_time;
static double picosat_timeout;
static inline int picosat_callback(void* /* dummy */)
{
    if (picosat_time.read() > picosat_timeout) {
        picosat_time.stop();
        picosat_time.reset();
        return 1;
    }
    picosat_time.start();
    return 0;
}


/**
 * \brief An interface for the Picosat SAT solver by Armin Biere.
 */
class Picosat: public SatSolverInterface
{
public:
    Picosat():
        SatSolverInterface(),
        _solver(nullptr)
    {
        _solver = picosat_init();
    }

    Picosat(const Picosat&) = delete;
    Picosat(Picosat&&) = delete;

    virtual ~Picosat() noexcept
    {
        picosat_reset(_solver);
    }

    Picosat& operator=(const Picosat&) = delete;
    Picosat& operator=(Picosat&&) = delete;

    virtual void setMaxIndex(const Variable index) override
    {
        picosat_adjust(_solver, index);
    }

    virtual void addClause(const std::vector<Literal>& clause) override
    {
        for (const Literal lit: clause) {
            picosat_add(_solver, lit2dimacs(lit));
        }
        picosat_add(_solver, 0);
    }

    virtual bool addUnit(const Literal lit) override
    {
        picosat_add(_solver, lit2dimacs(lit));
        picosat_add(_solver, 0);
        return true;
    }

    virtual void setPolarity(const Variable var, const bool value) override
    {
        picosat_set_default_phase_lit(_solver, var, (value ? +1 : -1));
    }

    virtual void setTimeout(const double time) override
    {
        picosat_timeout = time;
        picosat_set_interrupt(_solver, nullptr, picosat_callback);
    }

    virtual TruthValue solve() override
    {
        picosat_time.reset();
        picosat_time.start();
        const auto result = picosat_sat(_solver, -1);
        picosat_time.stop();
        if (result == PICOSAT_SATISFIABLE) return TruthValue::TRUE;
        else if (result == PICOSAT_UNSATISFIABLE) return TruthValue::FALSE;
        else return TruthValue::UNKNOWN;
    }

    virtual TruthValue solve(const std::vector<Literal>& assumptions) override
    {
        for (Literal lit: assumptions) {
            picosat_assume(_solver, lit2dimacs(lit));
        }

        const auto result = picosat_sat(_solver, -1);
        if (result == PICOSAT_SATISFIABLE) return TruthValue::TRUE;
        else if (result == PICOSAT_UNSATISFIABLE) return TruthValue::FALSE;
        else return TruthValue::UNKNOWN;
    }

    virtual TruthValue getValue(const Variable var) override
    {
        const auto val = picosat_deref(_solver, var);
        if (val == 1) return TruthValue::TRUE;
        else return TruthValue::FALSE;
    }

private:
    PicoSAT* _solver; ///< The instance of the Picosat solver (C-interface)
};


} // end namespace hqspre

#endif
