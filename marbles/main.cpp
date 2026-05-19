#include <cassert>
#include <iostream>
#include <utility>

typedef std::pair<int, int> Coord;

static const int N = 7;

enum class STATE {
    EMPTY = 0,
    FULL
};

enum class DIR {
    LEFT = 0,
    UP,
    RIGHT,
    DOWN
};


char dirToStr(DIR dir) {
    switch (dir) {
        case DIR::DOWN:
            return 'v';
        case DIR::LEFT:
            return '<';
        case DIR::RIGHT:
            return '>';
        case DIR::UP:
            return '^';
    }
    return '?';
}

constexpr bool inBounds(int x, int y) {
    const bool xoob = x < 2 || x > 4;
    const bool yoob = y < 2 || y > 4;
    return (x >= 0 && x < N && y >= 0 && y < N) && (!xoob || !yoob);
}

constexpr std::pair<int, int> newCoord(Coord from, DIR dir, int len) {
    const int xdiff = dir == DIR::LEFT ? -1 : (dir == DIR::RIGHT ? 1 : 0);
    const int ydiff = dir == DIR::UP ? -1 : (dir == DIR::DOWN ? 1 : 0);

    const auto ret = Coord(from.first + xdiff * len, from.second + ydiff * len);
    //std::cout << "newCord(" << from.first << "," << from.second << ", " << dirToStr(dir) << ") = " << "(" << ret.first << "," << ret.second << ")\n";
    return ret;
}

class Board {
public:
    Board()
        : left(31) {
        for (int y = 0; y < N; ++y) {
            for (int x = 0; x < N; ++x) {
                board[x][y] = STATE::FULL;
            }
        }
        // to avoid duplicating for symmetry + only 1 Coordsible move
        // just make it to start with
        board[3][1] = STATE::EMPTY;
        board[3][2] = STATE::EMPTY;
    }

    Board(STATE state[N][N])
        : left(0) {
        for (int y = 0; y < N; ++y) {
            for (int x = 0; x < N; ++x) {
                if (inBounds(x, y)) {
                    board[x][y] = state[x][y];
                    if (state[x][y] == STATE::FULL) {
                        ++this->left;
                    }
                } else {
                    board[x][y] = STATE::FULL;
                }
            }
        }
    }

    void printBoard() const {
        std::cout << "    ";
        for (int x = 0; x < N; ++x) {
            std::cout << " " << x << " ";
        }
        std::cout << "\n    ";
        for (int x = 0; x < N; ++x) {
            std::cout << "---";
        }
        std::cout << std::endl;
        for (int y = 0; y < N; ++y) {
            std::cout << " " << y << " |";
            for (int x = 0; x < N; ++x) {
                if (inBounds(x, y)) {
                    std::cout << '[' << (board[x][y] == STATE::FULL ? 'O' : ' ') << ']';
                } else {
                    std::cout << "   ";
                }
            }
            std::cout << std::endl;
        }
    }

    void printMoves() const {
        for (int i = 0; i < 31; ++i) {
            std::cout << "(" << trackFrom[i].first << ", " << trackFrom[i].second << ")  " << dirToStr(trackDir[i]) << std::endl;
        }
    }

    bool canMove(Coord from, DIR dir) {
        const Coord middle = newCoord(from, dir, 1);
        const Coord to = newCoord(from, dir, 2);

        return inBounds(to.first, to.second) && board[middle.first][middle.second] == STATE::FULL && board[to.first][to.second] == STATE::EMPTY;
    }

    Coord move(Coord from, DIR dir) {
        const Coord middle = newCoord(from, dir, 1);
        const Coord to = newCoord(from, dir, 2);

        if (board[from.first][from.second] != STATE::FULL) {
            std::cout << "uh oh: " << from.first << ", " << from.second << std::endl;
            printBoard();
        }
        assert((board[from.first][from.second] == STATE::FULL) && "move() - valid move start pos");
        assert(canMove(from, dir) && "move() - can move");

        board[from.first][from.second] = STATE::EMPTY;
        board[middle.first][middle.second] = STATE::EMPTY;
        board[to.first][to.second] = STATE::FULL;

        trackFrom[31 - left] = from;
        trackDir[31 - left] = dir;

        --left;

        return to;
    }

    template <typename F, bool findFirstOnly>
    bool backtrack(const F& onSuccess) {
        if (left == 1) {
            return onSuccess();
        }
        for (int y = 0; y < N; ++y) {
            for (int x = 0; x < N; ++x) {
                const Coord from(x, y);
                if (inBounds(x, y) && this->board[from.first][from.second] == STATE::FULL) {
                    for (int i = 0; i < 4; ++i) {
                        const DIR dir = static_cast<DIR>(i);
                        if (canMove(from, dir)) {
                            const Coord to = move(from, dir);
                            //std::cout << "(" << from.first << ", " << from.second << ") " << dirToStr(dir) << " (" << to.first << ", " << to.second << ")\n";
                            const bool ret = backtrack<F, findFirstOnly>(onSuccess);
                            if (findFirstOnly) {
                                if (ret) {
                                    return true;
                                }
                            }
                            // undo move
                            const Coord middle = newCoord(from, dir, 1);
                            this->board[from.first][from.second] = STATE::FULL;
                            this->board[middle.first][middle.second] = STATE::FULL;
                            this->board[to.first][to.second] = STATE::EMPTY;
                            ++left;
                        }
                    }
                }
            }
        }
        return false;
    }

    STATE getState(int x, int y) const {
        return this->board[x][y];
    }

private:
    STATE board[N][N];
    int left;
    Coord trackFrom[31];
    DIR trackDir[31];
};

void countSolutions() {
    Board board;

    int solutions = 0;
    int centerSolutions = 0;
    
    const auto recordAllSolutions = [&board, &solutions, &centerSolutions]() -> bool {
        ++solutions;
        if (board.getState(3, 3) == STATE::FULL) {
            ++centerSolutions;
        }
        return false;
    };
    board.backtrack<decltype(recordAllSolutions), false>(recordAllSolutions);
    
    std::cout << "final solutions: " << solutions << " | " << centerSolutions << std::endl;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        const std::string raw = argv[1];
        if (raw.size() != 33) {
            std::cerr << "Invalid input board to solve for. Expected len 33, found: " << raw.size() << std::endl;
            return 1;
        }
        STATE state[N][N];
        int y = 0;
        int prog = 0;
        for (const char c : raw) {
            const bool middle = y > 1 && y < 5;
            const int offset = middle ? 0 : 2;
            const int len = middle ? 7 : 3;
            const int x = offset + prog;
            if (c != '0' && c != '1') {
                std::cerr << "Invalid board pos [" << x << "][" << y << "]: " << c << std::endl;
                return 2;
            }
            state[x][y] = c == '1' ? STATE::FULL : STATE::EMPTY;
            if (++prog >= len) {
                ++y;
                prog = 0;
            }
        }
        Board board(state);
        board.printBoard();
        bool foundNonCenterSolution = false;
        const auto abortOnCenter = [&foundNonCenterSolution, &board](){
            foundNonCenterSolution = true;
            if (board.getState(3, 3) == STATE::FULL) {
                return true;
            }
            return false;
        };
        const bool foundCenterSolution = board.backtrack<decltype(abortOnCenter), true>(abortOnCenter);
        std::cout << (foundCenterSolution ? "Solved" : (foundNonCenterSolution ? "Non-Center" : "Impossible")) << std::endl;
    } else {
        countSolutions();
    }

    return 0;
}