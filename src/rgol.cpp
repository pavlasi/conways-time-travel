
#include <chrono>
#include <z3++.h>

#include "matrix.hpp"

/*
 *  time_it()
 *
 *  Measures the execution time of a provided code block and adjusts
 *  the remaining timeout accordingly.
 *
 *  @timeout: A reference to an unsigned integer representing the
 *  remaining timeout in milliseconds. After executing the code block,
 *  the elapsed time will be subtracted from this timeout. If the
 *  elapsed time exceeds the current timeout, the timeout is set to
 *  zero.
 *
 *  @code: A code block that is executed and whose execution time is
 *  measured.  This block can contain any valid C++ statements or
 *  expressions.
 *
 *  Usage Example:
 *    unsigned timeout = 1000; // 1000 milliseconds
 *    time_it(timeout, {
 *        // Code block whose execution time is to be measured
 *        perform_heavy_computation();
 *    });
 *    // After execution, `timeout` is reduced by the time taken to
 *    // run `perform_heavy_computation()`, or set to zero if
 *    // exceeded.
 */
#define time_it(timeout, code)                                                                  \
    {                                                                                           \
        auto start = std::chrono::steady_clock::now();                                          \
        { code }                                                                                \
        auto end = std::chrono::steady_clock::now();                                            \
        auto k   = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();  \
        if(k < timeout) {                                                                       \
            timeout -= k;                                                                       \
        } else {                                                                                \
            timeout = 0;                                                                        \
        }                                                                                       \
    }


namespace rgol {

    namespace {

        constexpr std::size_t max_neigh = 8;

        template <class T>
        struct State {

            z3::config&  cfg;
            z3::context& ctx;
            T& solver;

            struct Env {

                z3::expr zero;
                z3::expr one;
                z3::expr two;
                z3::expr three;
                z3::expr four;

                z3::expr expr_false;
                z3::expr expr_true;

                public:

                /*
                 *  Env()
                 *
                 *  Constructs the Env structure by initializing common Z3
                 *  integer and boolean expressions using the provided Z3
                 *  context.
                 *
                 *  @ctx: Reference to the Z3 context used to initialize Z3
                 *  expressions.
                 */
                Env(z3::context& ctx) : 
                    zero (ctx.int_val(0)),
                    one  (ctx.int_val(1)),
                    two  (ctx.int_val(2)),
                    three(ctx.int_val(3)),
                    four (ctx.int_val(4)),
                    expr_false(ctx.bool_val(false)),
                    expr_true (ctx.bool_val(true)) 
                {}

            } env;

            public:

            /*
             *  State()
             *  
             *  Constructs the State structure by initializing its reference
             *  members and the embedded Env struct. The Env struct is
             *  initialized with the provided Z3 context, allowing for the
             *  creation and management of common Z3 expressions needed for
             *  constraint definitions.
             *
             *  @cfg: Reference to the Z3 configuration object used to configure
             *  solver settings.
             *  
             *  @ctx: Reference to the Z3 context within which all Z3
             *  expressions are created.
             *  
             *  @solver: Reference to the Z3 solver or optimizer used to add and
             *  solve constraints.
             */
            State(z3::config& cfg, z3::context& ctx, T& solver) : cfg(cfg), ctx(ctx), solver(solver), env(ctx) {}
        };

        /*
         *  neigh_sum()
         *
         *  Computes the symbolic sum of live neighbors for a cell at
         *  position (i, j) in the `ct0` matrix, using the Z3 solver
         *  environment.
         *
         *  @st: The state object containing the Z3 context and solver
         *  attributes.
         *
         *  @ct0: A symbolic matrix of Z3 expressions representing the
         *  state of the Game of Life board at time t0.
         *
         *  @i: The row index of the cell for which the neighbor sum is
         *  being computed.
         *
         *  @j: The column index of the cell for which the neighbor sum is
         *  being computed.
         *
         *  return:
         *    - A Z3 symbolic expression representing the total number of
         *    live neighbors for the cell at position (i, j) in the `ct0`
         *    matrix.
         */
        template <class T>
        static z3::expr neigh_sum(State<T>& st, const Matrix<z3::expr>& ct0, std::size_t i, std::size_t j) {

            int x;
            int y;
            int n;
            int m;
            std::size_t k;

            z3::expr sum = st.env.zero;

            const int off[8][2] = {
                {-1, -1}, {-1, 0}, {-1, 1},
                { 0, -1},          { 0, 1},
                { 1, -1}, { 1, 0}, { 1, 1}
            };

            n = (int)ct0.n();
            m = (int)ct0.m();
            for(k = 0; k < max_neigh; k++) {
                x = (int)(off[k][0] + i);
                y = (int)(off[k][1] + j);
                if((x >= 0 && x < n) && (y >= 0 && y < m)) {
                    sum = sum + z3::ite(ct0(x, y), st.env.one, st.env.zero);
                }
            }

            return sum;
        }

        /*
         *  add_clauses()
         *
         *  Encodes the Game of Life rules as constraints into the Z3
         *  solver. The rules ensure the correct transition from the
         *  symbolic `ct0` (state at time t0) to `ct1` (state at time t1)
         *  based on the number of live neighbors.
         *
         *  @st: The state object containing the Z3 context, environment
         *  (constants like true, false, integers, etc.), and solver
         *  attributes.
         *
         *  @t1: The matrix representing the known state of the Game of
         *  Life board at time t1.
         *
         *  @ct1: A symbolic matrix of Z3 expressions representing the
         *  state of the board at t1.
         *
         *  @ct0: A symbolic matrix of Z3 expressions representing the
         *  state of the board at t0.
         *
         *  return:
         *    - A Z3 symbolic expression representing the total number
         *    of alive cells in `ct0`.  This expression can be used
         *    for optimization to minimize the alive cell count in t0.
         */
        template <class T>
        static z3::expr add_clauses(State<T>& st, const Matrix<int>& t1, const Matrix<z3::expr>& ct1, Matrix<z3::expr>& ct0) {

            std::size_t i;
            std::size_t j;
            std::size_t n;
            std::size_t m;

            n = t1.n();
            m = t1.m();

            z3::expr total = st.env.zero;
            z3::expr neigh = st.env.zero;

            for(i = 0; i < n; i++) {
                for(j = 0; j < m; j++) {
                    neigh = neigh_sum(st, ct0, i, j);
                    total = total + z3::ite(ct0(i, j), st.env.one, st.env.zero);

                    /* Game of Life rules */
                    st.solver.add(
                        ct1(i, j) == z3::ite(
                            (neigh == st.env.three) ||
                            (ct0(i, j) && (neigh == st.env.two)),
                            st.env.expr_true,
                            st.env.expr_false
                        )
                    );
                }
            }

            return total;
        }

        /*
         *  init_repr()
         *
         *  Initializes the symbolic representation of t0 and t1 matrices
         *  using Z3. This function initializes the Z3 symbolic variables
         *  for both t0 and t1, assigns names to the variables for
         *  debugging and interpretation, and adds constraints to the
         *  solver that match the state of t1 to its corresponding symbolic
         *  representation.
         *
         *  @st: The state object containing the Z3 context, configuration,
         *  and solver.
         *
         *  @t1: The input matrix representing the known state of the Game
         *  of Life board at time t1.
         *
         *  @ct1: A symbolic matrix of Z3 expressions representing the
         *  state of t1 in the solver.
         *
         *  @ct0: A symbolic matrix of Z3 expressions representing the
         *  state of t0 in the solver.
         */
        template <class T>
        static void init_repr(State<T>& st, const Matrix<int>& t1, Matrix<z3::expr>& ct1, Matrix<z3::expr>& ct0) {
            
            std::size_t i;
            std::size_t j;
            std::size_t n;
            std::size_t m;

            std::string n0;
            std::string n1;
            std::string k;

            n = t1.n();
            m = t1.m();
            for(i = 0; i < n; i++) {
                for(j = 0; j < m; j++) {
                    k  = std::to_string(i) + "_" + std::to_string(j);
                    n0 = "t0_" + k;
                    n1 = "t1_" + k;

                    ct0(i, j) = st.ctx.bool_const(n0.c_str());
                    ct1(i, j) = st.ctx.bool_const(n1.c_str());
                    st.solver.add(
                        ct1(i, j) == (
                            t1(i, j) ?
                            st.env.expr_true
                            :
                            st.env.expr_false
                        )
                    );
                }
            }
        }

        /*
         *  fill_t0()
         *
         *  Extracts the solution for the t0 matrix from the Z3 model and
         *  populates it with the computed values representing the state of
         *  the Game of Life board at time t0.
         *
         *  @st: The state object containing the Z3 context, solver, and
         *  environment.
         *
         *  @ct0: A symbolic matrix of Z3 expressions representing the
         *  state of the Game of Life board at time t0, as defined in the
         *  solver.
         *
         *  @t0: The integer matrix to be filled with the solution for t0.
         *  Each cell will be set to `1` if it is alive in the solution or
         *  `0` if it is dead.
         */
        template <class T>
        static void fill_t0(State<T>& st, const Matrix<z3::expr>& ct0, Matrix<int>& t0) {
            
            std::size_t i;
            std::size_t j;
            std::size_t n;
            std::size_t m;

            n = t0.n();
            m = t0.m();

            z3::model model = st.solver.get_model();

            for(i = 0; i < n; i++) {
                for(j = 0; j < m; j++) {
                    z3::expr e = model.eval(ct0(i, j), true);
                    if(e.is_true()) {
                        t0(i, j) = 1;
                    } else {
                        t0(i, j) = 0;
                    }
                }
            }
        }

        /*
         *  count_ones() -
         *
	 *  Counts the number of alive cells in the `ct0` matrix based
	 *  on the current model provided by the Z3 solver.
         *
	 *  @st: A reference to the `State<T>` object that contains
	 *  the Z3 context and solver. This state object is used to
	 *  access the current model from which the values of the
	 *  symbolic expressions in `ct0` are evaluated.
         *
	 *  @ct0: A constant reference to a `Matrix<z3::expr>`
	 *  representing the symbolic state of the Game of Life board
	 *  at time t0.
         *
         *  return:
	 *    - A `std::size_t` value representing the total number of
	 *      cells in `ct0` that are alive in the current model. 
         */
        template <class T>
        static std::size_t count_ones(State<T>& st, const Matrix<z3::expr>& ct0) {

            std::size_t i;
            std::size_t j;
            std::size_t n;
            std::size_t m;
            std::size_t sum;
            
            n = ct0.n();
            m = ct0.m();

            z3::model model = st.solver.get_model();

            sum = 0;
            for(i = 0; i < n; i++) {
                for(j = 0; j < m; j++) {
                    z3::expr e = model.eval(ct0(i, j), true);
                    sum += e.is_true() ? 1 : 0;
                }
            }

            return sum;
        }
    }

    /*
     *  solve_iter()
     *
     *  Attempts to find any valid previous state (t0) of the Game of
     *  Life board that evolves into the given state (t1) at the next
     *  time step using an iterative deepening approach. This method
     *  progressively searches for solutions with decreasing numbers
     *  of alive cells, thereby facilitating the discovery of a state
     *  with the minimal number of alive cells that satisfies the Game
     *  of Life rules.
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
     *  @timeout: The time limit (in milliseconds) for the solver. If
     *  the solver exceeds this limit, it terminates and returns no
     *  solution.
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
    bool solve_iter(const Matrix<int>& t1, Matrix<int>& t0, unsigned timeout, unsigned threads, bool& sat) {

        std::size_t cur;
        std::size_t max;

        z3::config cfg;
        z3::context ctx(cfg);
        z3::solver sol(ctx);
        z3::params p(ctx);

        p.set("threads", threads);

        Matrix<z3::expr> ct0(t0.n(), t0.m(), ctx);
        Matrix<z3::expr> ct1(t1.n(), t1.m(), ctx);

        /* Plain-Old Data Type to aggregate the solver attributes */
        State st = {
            cfg, 
            ctx, 
            sol
        };

        init_repr(st, t1, ct1, ct0);
        z3::expr total = add_clauses(st, t1, ct1, ct0);

        sat = false;
        max = t0.n() * t0.m();
        for(int i = max; i >= 0 && timeout; i--) {
            time_it(timeout, 
                p.set("timeout", timeout);
                sol.set(p);
                sol.push(); 
                sol.add(total <= ctx.int_val(max));
                if(sol.check() == z3::sat) {
                    cur = count_ones(st, ct0);
                    if(cur <= max) {
                        fill_t0(st, ct0, t0);
                        max = cur;
                    }
                    sat = true;
                } else {
                    break;
                }
                sol.pop();
            );
        }

        return sat;
    }

    /*
     *  solve()
     *
     *  Attempts to find a minimal previous state (t0) of the Game of
     *  Life board that evolves into the given state (t1) at the next
     *  time step.
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
     *  @timeout: The time limit (in milliseconds) for the solver. If
     *  the solver exceeds this limit, it terminates and returns no
     *  solution.
     *
     *  return:
     *    - `true`: Indicates that a valid previous state (`t0`) was
     *    found that evolves into the given state (`t1`) under the
     *    Game of Life rules.
     *
     *    - `false`: Indicates that no such previous state exists for
     *    the provided `t1` state.
     */
    bool solve(const Matrix<int>& t1, Matrix<int>& t0, unsigned timeout) {

        bool sat;

        z3::config cfg;
        z3::context ctx(cfg);
        z3::optimize opt(ctx);
        z3::params p(ctx);

        p.set("timeout", timeout);
        opt.set(p);

        Matrix<z3::expr> ct0(t0.n(), t0.m(), ctx);
        Matrix<z3::expr> ct1(t1.n(), t1.m(), ctx);

        /* Plain-Old Data Type to aggregate the solver attributes */
        State st = {
            cfg, 
            ctx, 
            opt 
        };

        sat = false;
        init_repr(st, t1, ct1, ct0);
        opt.minimize(add_clauses(st, t1, ct1, ct0));
        if(opt.check() == z3::sat) {
            fill_t0(st, ct0, t0);
            sat = true;
        }

        return sat;
    }
}
