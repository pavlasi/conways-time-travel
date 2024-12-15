
#include <optional>
#include <unistd.h>
#include <chrono>

#include "board.hpp"
#include "utils.hpp"
#include "rgol.hpp"

/*
 *  Board(n, m)
 *
 *  @n: The number of rows for the board.
 *  @m: The number of columns for the board.
 *
 *  Initializes a Board object with a table of size n x m.
 */
Board::Board(std::size_t n, std::size_t m) : table(n, m) {}

/*
 *  launch_tasks()
 *
 *  This function coordinates the execution of two asynchronous solve
 *  tasks: one to find any solution (`rgol::solve`) and another to
 *  find the solution with the minimum number of alive cells
 *  (`rgol::solve_min_alive`).  It allocates available threads to the
 *  two tasks, waits for their completion within a specified time, and
 *  returns their results as a pair of booleans.
 *
 *  @any: A reference to a Board object where the result of the "any
 *  solution" task will be stored.  The task uses the `rgol::solve`
 *  function to find any valid solution.
 *
 *  @min: A reference to a Board object where the result of the
 *  "minimum alive" task will be stored.  The task uses the
 *  `rgol::solve_min_alive` function to find a solution with the
 *  fewest live cells.
 *
 *  @wait_time: The total time in milliseconds to spend waiting for
 *  both tasks to complete. The function ensures that the tasks
 *  collectively do not exceed this time limit.
 *
 *  return:
 *    - A `std::pair<bool, bool>` where:
 *      - The first indicates whether the "any solution"
 *      task completed successfully.
 *
 *      - The second indicates whether the "minimum
 *      alive" task completed successfully.
 */
std::pair<bool, bool> Board::launch_tasks(Board& any, Board& min, unsigned wait_time) const {

    std::pair<bool, bool> ret(false, false);

    auto anyfut = utils::launch_future(
        [&]() {
            return rgol::solve_iter(
                table,
                any.table,
                wait_time - 200,
                std::thread::hardware_concurrency() - 1,
                ret.first
            );
        }
    );

    auto minfut = utils::launch_future(
        [&]() {
            return rgol::solve(
                table,
                min.table,
                wait_time
            );
        }
    );

    if(anyfut.has_value() && minfut.has_value()) {
        auto start  = std::chrono::steady_clock::now();
        utils::wait_future(
            anyfut.value(), 
            std::chrono::milliseconds(wait_time)
        );

        if(ret.first) {

            /*
             *  If the "any solution" task returns `false`, it means the
             *  problem is unsatisfiable (UNSAT). In this case, there is no
             *  valid solution, so the "minimum alive" task is abandoned,
             *  and we return early with the result.
             *
             *  (I haven't found a way to kill the async task)
             */
            if(ret.first) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - start
                ).count();

                auto minopt = utils::wait_future(minfut.value(), std::chrono::milliseconds(wait_time > elapsed ? wait_time - elapsed : 0));
                if(minopt.has_value()) {
                    ret.second = minopt.value();
                }
            }
        }
    }

    return ret;
}

/*
 *  previous_state()
 *
 *  This function computes the previous state of the current board in
 *  Conway's Game of Life.
 *
 *  @wait_time: Timeout in seconds.
 *
 *  return:
 *    - `std::optional<Board>`:
 *        - If a valid previous state is found, the function returns a
 *        `Board` object representing this state.
 *
 *        - If no valid previous state exists (unsatisfiable
 *        constraints), the function returns `std::nullopt` to
 *        indicate failure.
 */
std::optional<Board> Board::previous_state(unsigned wait_time) const {

    std::size_t n;
    std::size_t m;

    std::pair<bool, bool> status;

    n = table.n();
    m = table.m();

    Board unsat(n, m);
    Board any(n, m);
    Board min(n, m);

    status = launch_tasks(any, min, 1000 * wait_time);
    if(!status.first) {
        return unsat;
    } else {
        if(status.second) {
            return min;
        }
    }

    return any;
}

/*
 *  operator>>()
 *
 *  @is: Input stream from which data is read.
 *  @board: The Board object to populate with the data read from the
 *  stream.
 *
 *  return:
 *    - A reference to the input stream (std::istream&) to allow
 *      chaining of input operations.
 */
std::istream& operator>>(std::istream& is, Board& board) {
    is >> board.table;
    return is;
}

/*
 *  operator<<()
 *
 *  @os: The output stream to which the board's content is written.
 *  @board: The Board object whose content will be serialized and
 *  written to the output stream.
 *
 *  return:
 *    - A reference to the output stream (std::ostream&) to allow
 *      chaining of output operations.
 */
std::ostream& operator<<(std::ostream& os, const Board& board) {
    os << board.table;
    return os;
}

/*
 *  operator<<()
 *
 *  Overloads the output stream operator for std::optional<Board>. 
 *  If the optional contains a value, the Board is printed using its
 *  operator<< implementation. Otherwise, a fallback message is
 *  printed.
 *
 *  @os: The output stream to which the content of the optional is
 *  written.
 *
 *  @opt: The std::optional<Board> object to be printed.
 *
 *  return:
 *    - A reference to the output stream (std::ostream&) to allow
 *      chaining of output operations.
 */
std::ostream& operator<<(std::ostream& os, const std::optional<Board>& opt) {

    if(opt.has_value()) {
        os << opt.value();
    } else {
        os << "No solution found.";
    }

    return os;
}
