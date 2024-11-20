
#include <kissat/kissat.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>

#include "solve.h"

#define __unused __attribute__((unused))

#define skip(cond) if(cond) continue;

static __always_inline void dead_two_neigh(kissat* solver, const int* neigh, int curr) {

    size_t i;
    size_t j;
    size_t k;

    assert(solver);
    assert(neigh);

    for(i = 0; i < 8; i++) {
        skip(!neigh[i]);

        for(j = i + 1; j < 8; j++) {
            skip(!neigh[j]);
            kissat_add(solver, -neigh[i]);
            kissat_add(solver, -neigh[j]);

            for(k = j + 1; k < 8; k++) {
                skip(!neigh[k]);
                if(k != i && k != j) {
                    kissat_add(solver, neigh[k]);
                }
            }
            kissat_add(solver, 0);
        }
    }
}

static __always_inline void dead_one_neigh(kissat* solver, const int* neigh, int curr) {

    size_t i;
    size_t j;

    assert(solver);
    assert(neigh);

    for(i = 0; i < 8; i++) {
        skip(!neigh[i]);
        kissat_add(solver, -neigh[i]);

        for(j = 0; j < 8; j++) {
            skip(!neigh[j]);
            if(j != i) {
                kissat_add(solver, neigh[j]);
            }
        }
        kissat_add(solver, 0);
    }
}

static __always_inline void dead_no_neigh(kissat* solver, const int* neigh, int curr) {

    size_t i;

    assert(solver);
    assert(neigh);

    for(i = 0; i < 8; i++) {
        skip(!neigh[i]);
        kissat_add(solver, -neigh[i]);
    }

    kissat_add(solver, 0);
}

static void dead(kissat* solver, const int* neigh, int curr) {

    assert(solver);
    assert(neigh);

    dead_no_neigh(solver, neigh, curr);
    dead_one_neigh(solver, neigh, curr);
    //dead_two_neigh(solver, neigh, curr);
}

static void life(kissat* solver, const int* neigh, int curr) {

    size_t i;
    size_t j;
    size_t k;

    assert(solver);
    assert(neigh);

    for(i = 0; i < 8; i++) {
        skip(!neigh[i]);
        
        for(j = i + 1; j < 8; j++) {
            skip(!neigh[j]);
            
            kissat_add(solver, abs(curr));
            kissat_add(solver, neigh[i]);
            kissat_add(solver, neigh[j]);

            for(k = 0; k < 8; k++) {
                skip(!neigh[k]);
                if(k != i && k != j) {
                    kissat_add(solver, -neigh[k]);
                }
            }
            kissat_add(solver, 0);
        }
    }
}

/*
 * neigh_init_i() -
 *
 * Initializes the `neigh` array with the coordinates of the 8
 * neighbors surrounding a given cell on the Game of Life board. The
 * function also calculates the number of live neighbors and stores it
 * in the `alive` parameter.
 *
 * @neigh: An array of integers where the literals (representing the
 * states of the 8 neighboring cells) will be stored.
 *
 * @b: The Game of Life board represented as a Board structure.
 *
 * @i: The row index of the cell for which neighbors are being
 * calculated.
 *
 * @j: The column index of the cell for which neighbors are being
 * calculated.
 *
 * @alive: A pointer to a size_t variable where the function will
 * store the number of live neighbors. A neighbor is considered live
 * if its literal is positive.
 */
static void neigh_init(int* neigh, const Board* b, size_t i, size_t j, size_t* alive) {

    size_t k;
    size_t x;
    size_t y;

    assert(b);
    assert(neigh);
    assert(alive);

    int pos[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0 , -1}, {0 , 1},
        {1 , -1}, {1 , 0}, {1, 1}
    };

    *alive = 0;
    memset(neigh, 0, 8 * sizeof *neigh);

    for(k = 0; k < 8; k++) {
        x = i + pos[k][0];
        y = j + pos[k][1];

        if (x < b->n && y < b->m) {
            neigh[k] = b->table[x][y];
            if(neigh[k] > 0) {
                (*alive)++; 
            }
            neigh[k] = abs(neigh[k]);
        } 
    }
}

/*
 *  add_clauses() -
 *
 *  @solver:
 *  @b:
 */
static void add_clauses(kissat* solver, const Board* b) {

    size_t i;
    size_t j;
    size_t alive;

    int curr;
    int neigh[8];

    assert(solver);
    assert(b);

    for(i = 0; i < b->n; i++) {
        for(j = 0; j < b->m ; j++) {
            curr = b->table[i][j];
            neigh_init(neigh, b, i, j, &alive);
            if(curr > 0) {
                life(solver, neigh, curr);
            } else {
                dead(solver, neigh, curr);
            }
        }
    }
}

/*
 * fill_result_board() -
 *
 * This function fills the result board `r` with the values from the
 * solver.  It uses the state of the board `b` and the solver to
 * update `r` with the results of the SAT solver, where the values are
 * determined by the literal values in the solver (positive for alive
 * cells, negative for dead cells).
 *
 * @solver: A pointer to the Kissat SAT solver that has been used to
 * solve the problem and contains the literal values for each cell.
 *
 * @b: The input Game of Life board that contains the initial state of
 * the cells, represented by integers. The values represent the
 * literals for each cell (positive for alive, negative for dead).
 *
 * @r: The result board where the final state of the Game of Life will
 * be stored, with 1 for alive cells and -1 for dead cells.
 */
static __always_inline void fill_result_board(kissat* solver, const Board* b, Board* r) {

    size_t i;
    size_t j;

    assert(solver);
    assert(b);
    assert(r);

    r->n = b->n;
    r->m = b->m;
    for(i = 0; i < b->n; i++) {
        for(j = 0; j < b->m; j++) {
            r->table[i][j] = kissat_value(solver, abs(b->table[i][j]));
        }
    }
}

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
int solve(const Board* b, Board* r) {

    kissat* solver;

    assert(b);
    assert(r);

    solver = kissat_init();
    if(solver) {
        add_clauses(solver, b);
        if(kissat_solve(solver) == 10) {
            fill_result_board(solver, b, r);
            return 1;
        }
    }

    kissat_release(solver);

    return 0;
}
