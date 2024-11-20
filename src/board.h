#ifndef BOARD_H
#define BOARD_H

#include <stdio.h>

#define N 20

/*
 *  A structure representing the Game of Life board.
 */
struct Board {

    size_t n;
    size_t m;

    int table[N][N];
};

typedef struct Board Board;

/*
 * board_read() -
 *
 * Reads the current state of the Game of Life board from a given
 * input stream and stores it in the provided Board structure.
 *
 * @board: A pointer to the Board structure where the read data will
 * be stored.  The board's 2D `table` array will be populated with the
 * input values.
 *
 * @stream: The input stream (e.g., file pointer or stdin) from which
 * the board data will be read. The board's state is expected to be in
 * the format of space-separated integers (either 0 or 1) representing
 * dead and alive cells.
 */
extern void board_read(Board* board, FILE* stream);

/*
 * board_print() -
 *
 * Prints the current state of the Game of Life board to the specified
 * output stream. Each cell's state (either 0 for dead or 1 for alive)
 * is printed as a space-separated integer, followed by a newline
 * after each row.
 *
 * @board: A pointer to the Board structure whose state will be
 * printed.  The board's 2D `table` array will be printed row by row.
 *
 * @stream: The output stream (e.g., file pointer or stdout) where the
 * board's state will be printed. 
 */
extern void board_print(const Board* board, FILE* stream);

#endif  /* BOARD_H */
