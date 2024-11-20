#ifndef SOLVE_H
#define SOLVE_H

#include "board.h"

/*
 * solve() -
 *
 * This function solves the Game of Life problem for the given board
 * `b` and computes the immediate previous state (t0) of the board,
 * which is then stored in the result board `r`. The function uses the
 * Kissat SAT solver to reverse the Game of Life state from `t1` to
 * `t0` based on the current state in `b`.
 *
 * @b: The input Game of Life board representing the current state.
 * @r: The result board where the previous state (t0) of the Game of
 * Life will be stored. 
 *
 * return: 
 *  - '1' on success. 
 *  - '0' otherwise.
 */
extern int solve(const Board* b, Board* r);

#endif  /* SOLVE_H */
