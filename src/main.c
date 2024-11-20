
#include "solve.h"

int main(void) {

    Board b;
    Board r;

    board_read(&b, stdin);
    if(solve(&b, &r)) {
        board_print(&r, stdout);
    } else {
        puts("Failed to find a solution.");
    }

    return 0;
}
