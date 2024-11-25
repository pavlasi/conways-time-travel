#ifndef RGOL_HPP
#define RGOL_HPP

#include "matrix.hpp"

namespace rgol {

    /*
     *  solve()
     *
     *  Attempts to find any valid previous state (t0) of the Game of
     *  Life board that evolves into the given state (t1) at the next
     *  time step. This function sets up the necessary Z3 solver
     *  environment, encodes the Game of Life rules as constraints,
     *  and searches for a configuration of t0 that satisfies these
     *  constraints without optimizing for the minimal number of alive
     *  cells.
     *
     *  @t1: A constant reference to a `Matrix<int>` representing the
     *  known state of the Game of Life board at time t1. Each cell in
     *  the matrix should be either `1` (alive) or `0` (dead).
     *
     *  @t0: A reference to a `Matrix<int>` that will be populated
     *  with the computed previous state of the Game of Life board at
     *  time t0. Each cell will be set to `1` if it is alive in the
     *  solution or `0` if it is dead. This matrix should be
     *  pre-initialized with the appropriate dimensions corresponding
     *  to `t1`.
     *
     *  @threads: An integer specifying the number of threads to
     *  enable for the Z3 solver.
     *
     *  return:
     *    - `true`: Indicates that a valid previous state (`t0`) was
     *    found that evolves into the given state (`t1`) under the
     *    Game of Life rules.
     *
     *    - `false`: Indicates that no such previous state exists for
     *    the provided `t1` state.
     */
    extern bool solve(const Matrix<int>& t1, Matrix<int>& t0, int threads = 0);

    /*
     *  solve_min_alive()
     *
     *  Attempts to find a minimal previous state (t0) of the Game of
     *  Life board that evolves into the given state (t1) at the next
     *  time step. This function sets up the necessary Z3 solver
     *  environment, encodes the Game of Life rules as constraints,
     *  and searches for a valid configuration of t0 that satisfies
     *  these constraints while minimizing the number of alive cells.
     *
     *  @t1: A constant reference to a `Matrix<int>` representing the
     *  known state of the Game of Life board at time t1. Each cell in
     *  the matrix should be either `1` (alive) or `0` (dead).
     *
     *  @t0: A reference to a `Matrix<int>` that will be populated
     *  with the computed previous state of the Game of Life board at
     *  time t0. Each cell will be set to `1` if it is alive in the
     *  solution or `0` if it is dead. This matrix should be
     *  pre-initialized with the appropriate dimensions corresponding
     *  to `t1`.
     *
     *  @threads: An integer specifying the number of threads to
     *  enable for the Z3 solver. 
     *
     *  return:
     *    - `true`: Indicates that a valid previous state (`t0`) was
     *    found that evolves into the given state (`t1`) under the
     *    Game of Life rules.
     *
     *    - `false`: Indicates that no such previous state exists for
     *    the provided `t1` state.
     */
    extern bool solve_min_alive(const Matrix<int>& t1, Matrix<int>& t0, int threads = 0);
};

#endif  /* RGOL_HPP */
