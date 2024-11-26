#ifndef BOARD_HPP
#define BOARD_HPP

#include <iostream>
#include <optional>
#include <z3++.h>

#include "matrix.hpp"

class Board {

    public:

    /*
     *  Board(n, m)
     *
     *  @n: The number of rows for the board.
     *  @m: The number of columns for the board.
     *
     *  Initializes a Board object with a table of size n x m.
     */
    Board(std::size_t n, std::size_t m);

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
    std::optional<Board> previous_state(unsigned wait_time = 300) const;

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
    friend std::istream& operator>>(std::istream& is, Board& board);

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
    friend std::ostream& operator<<(std::ostream& os, const Board& board);

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
    friend std::ostream& operator<<(std::ostream& os, const std::optional<Board>& opt);

    private:

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
     *      - The first `bool` (`a`) indicates whether the "any solution"
     *      task completed successfully.
     *
     *      - The second `bool` (`m`) indicates whether the "minimum
     *      alive" task completed successfully.
     */
    std::pair<bool, bool> launch_tasks(Board& any, Board& min, unsigned wait_time) const;

    private:

    Matrix<int> table;
};

#endif  /* BOARD_HPP */
