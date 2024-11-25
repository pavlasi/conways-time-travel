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
     *  return:
     *    - `std::optional<Board>`:
     *        - If a valid previous state is found, the function returns a
     *        `Board` object representing this state.
     *
     *        - If no valid previous state exists (unsatisfiable
     *        constraints), the function returns `std::nullopt` to
     *        indicate failure.
     */
    std::optional<Board> previous_state(std::size_t wait_time = 600) const;

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

    bool launch_tasks(Board& any, Board& min, std::size_t wait_time) const;

    private:

    Matrix<int> table;
};

#endif  /* BOARD_HPP */
