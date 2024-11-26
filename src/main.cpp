
#include "board.hpp"

int main() {

    std::size_t n;
    std::size_t m;

    std::cin >> n >> m;

    Board board(n, m);

    std::cin  >> board;
    std::cout << board.previous_state() << std::endl;

    return 0;
}
