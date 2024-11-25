
#include <optional>
#include <unistd.h>
#include <future>

#include "board.hpp"
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
 *  @any:
 *  @min:
 *  @wait_time:
 *
 *  return:
 *    -
 *    -
 */
bool Board::launch_tasks(Board& any, Board& min, std::size_t wait_time) const {

    pid_t pid;
    std::size_t threads;
    std::atomic<bool> failed(false);

    threads = std::thread::hardware_concurrency();

    std::future<bool> solve_any = std::async(std::launch::async, [&]() {
        bool ret = rgol::solve(table, any.table, threads/3);
        if(!ret) {
            failed.store(true);
        }
        return ret;
    });

    return true;
}

/*
 *  previous_state()
 *
 *  This function computes the previous state of the current board in
 *  Conway's Game of Life.
 *
 *  @wait_time:
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
std::optional<Board> Board::previous_state(std::size_t wait_time) const {

    std::size_t n;
    std::size_t m;

    n = table.n();
    m = table.m();

    Board any_t0(n, m);
    Board min_t0(n, m);


    return min_t0;
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
