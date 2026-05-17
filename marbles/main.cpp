#include <iostream>
#include <utility>
#include <vector>

enum STATE {
    EMPTY = 0,
    FULL
};

enum DIR {
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

typedef std::pair<int, int> Coord;

static const int N = 7;

STATE board[N][N];
int left = 32;
Coord trackFrom[31];
DIR trackDir[31];

void assert(bool cond, const char *text) {
    if (!cond) {
        std::cerr << "assert failed: " << text << std::endl;
        exit(1);
    }
}

constexpr bool inBounds(int x, int y) {
    const bool xoob = x < 2 || x > 4;
    const bool yoob = y < 2 || y > 4;
    return (x >= 0 && x < N && y >= 0 && y < N) && (!xoob || !yoob);
}

void printBoard() {
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

constexpr std::pair<int, int> newCoord(Coord from, DIR dir, int len) {
    const int xdiff = dir == DIR::LEFT ? -1 : (dir == DIR::RIGHT ? 1 : 0);
    const int ydiff = dir == DIR::UP ? -1 : (dir == DIR::DOWN ? 1 : 0);

    const auto ret = Coord(from.first + xdiff * len, from.second + ydiff * len);
    //std::cout << "newCord(" << from.first << "," << from.second << ", " << dirToStr(dir) << ") = " << "(" << ret.first << "," << ret.second << ")\n";
    return ret;
}

constexpr bool canMove(Coord from, DIR dir) {
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
    assert(board[from.first][from.second] == STATE::FULL, "move() - valid move start pos");
    assert(canMove(from, dir), "move() - can move");

    board[from.first][from.second] = STATE::EMPTY;
    board[middle.first][middle.second] = STATE::EMPTY;
    board[to.first][to.second] = STATE::FULL;

    trackFrom[31 - left] = from;
    trackDir[31 - left] = dir;

    --left;

    return to;
}

int solutions = 0;
int centerSolutions = 0;

void backtrack() {
    if (left == 1) {
        // std::cout << "found solution: " << std::endl;
        // printBoard();
        solutions++;
        if (board[3][3] == STATE::FULL) {
            ++centerSolutions;
            std::cout << "printing solution:" << std::endl;
            for (int i = 0; i < 31; ++i) {
                std::cout << "(" << trackFrom[i].first << ", " << trackFrom[i].second << ")  " << dirToStr(trackDir[i]) << std::endl;
            }
            exit(0);
        }
        return;
    }
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            const Coord from(x, y);
            if (inBounds(x, y) && board[from.first][from.second] == STATE::FULL) {
                for (int i = 0; i < 4; ++i) {
                    const DIR dir = static_cast<DIR>(i);
                    if (canMove(from, dir)) {
                        const Coord to = move(from, dir);
                        //std::cout << "(" << from.first << ", " << from.second << ") " << dirToStr(dir) << " (" << to.first << ", " << to.second << ")\n";
                        backtrack();
                        // undo move
                        const Coord middle = newCoord(from, dir, 1);
                        board[from.first][from.second] = STATE::FULL;
                        board[middle.first][middle.second] = STATE::FULL;
                        board[to.first][to.second] = STATE::EMPTY;
                        ++left;
                    }
                }
            }
        }
    }
}

int main() {
    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            board[x][y] = STATE::FULL;
        }
    }
    // to avoid duplicating for symmetry + only 1 Coordsible move
    // just make it to start with
    board[3][1] = STATE::EMPTY;
    board[3][2] = STATE::EMPTY;
    left = 31;
    backtrack();
    std::cout << "final solutions: " << solutions << " | " << centerSolutions << std::endl;
}