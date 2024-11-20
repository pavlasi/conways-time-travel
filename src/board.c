
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "board.h"

/*
 * board_read() -
 *
 * Reads the current state of the Game of Life board from a given
 * input stream and stores it in the provided Board structure.
 *
 * @board: A pointer to the Board structure where the read data will
 * be stored.  The board's 2D `table` isso com  array will be
 * populated with the input values.
 *
 * @stream: The input stream (e.g., file pointer or stdin) from which
 * the board data will be read. The board's state is expected to be in
 * the format of space-separated integers (either 0 or 1) representing
 * dead and alive cells.
 */
void board_read(Board* board, FILE* stream) {

    int v;
    size_t i; 
    size_t j;
    size_t ind;

    assert(board);
    assert(stream);

    fscanf(stream, "%zu %zu", &board->n, &board->m);

    for(i = 0; i < board->n; i++) {
        for(j = 0; j < board->m; j++) {
            fscanf(stream, "%d", &v);

            ind = i * board->n + j + 1;
            board->table[i][j] = (int)(v == 0 ? -ind : ind);
        }
    }
}

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
void board_print(const Board* board, FILE* stream) {

    size_t i;
    size_t j;

    assert(board);
    assert(stream);

    for(i = 0; i < board->n; i++) {
        for(j = 0; j < board->m; j++) {
            fprintf(stream, "%hhu ", board->table[i][j] > 0 ? 1 : 0);
        }
        fputs("\n", stream);
    }
}
