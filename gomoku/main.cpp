#include <iostream>
#include <vector>
#include <limits>

const int BOARD_SIZE = 15;
const int WINNING_LENGTH = 5;
const int MAX_DEPTH = 3;

enum Player { HUMAN = -1, EMPTY = 0, AI = 1 };

struct Move {
    int row;
    int col;
};

class Gomoku {
public:
    Gomoku() : board(BOARD_SIZE, std::vector<int>(BOARD_SIZE, EMPTY)) {}

    void printBoard() {
        std::cout << "   ";
        for (int i = 0; i < BOARD_SIZE; ++i) {
            if (i < 10) std::cout << i << "  ";
            else std::cout << i << " ";
        }
        std::cout << std::endl;

        for (int i = 0; i < BOARD_SIZE; ++i) {
            if (i < 10) std::cout << i << "  ";
            else std::cout << i << " ";
            for (int j = 0; j < BOARD_SIZE; ++j) {
                char symbol = '.';
                if (board[i][j] == HUMAN) symbol = 'X';
                else if (board[i][j] == AI) symbol = 'O';
                std::cout << symbol << "  ";
            }
            std::cout << std::endl;
        }
    }

    bool isValidMove(int row, int col) {
        // Проверяем, является ли ячейка пустой
        if (board[row][col] != EMPTY) {
            return false;
        }

        // Проверяем, есть ли на доске фишки в пределах 1 ячейки
        for (int r = std::max(0, row - 1); r <= std::min(BOARD_SIZE - 1, row + 1); ++r) {
            for (int c = std::max(0, col - 1); c <= std::min(BOARD_SIZE - 1, col + 1); ++c) {
                if (board[r][c] != EMPTY) {
                    return true;
                }
            }
        }
        return false; // Если нет фишек в пределах 1 ячейки
    }

    bool makeMove(int row, int col, Player player) {
        if (player == AI && isValidMove(row, col)) {
            board[row][col] = player;
            return true;
        }
        else if(player == HUMAN)
        {
            if (board[row][col] == EMPTY)
            {
                board[row][col] = player;
                return true;
            }
        }
        return false;
    }

    bool isWinningMove(int row, int col, Player player) {
        return checkDirection(row, col, player, 1, 0) ||  // Горизонталь
            checkDirection(row, col, player, 0, 1) ||  // Вертикаль
            checkDirection(row, col, player, 1, 1) ||  // Диагональ 
                           checkDirection(row, col, player, 1, -1);   // Диагональ /
    }

    bool isBoardFull() {
        for (int i = 0; i < BOARD_SIZE; ++i)
            for (int j = 0; j < BOARD_SIZE; ++j)
                if (board[i][j] == EMPTY)
                    return false;
        return true;
    }

    Move getBestMove() {
        int bestScore = std::numeric_limits<int>::min();
        Move bestMove = { -1, -1 };

        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (isValidMove(i, j)) {
                    board[i][j] = AI;
                    if (isWinningMove(i, j, AI))
                    {
                        board[i][j] = EMPTY;
                        bestMove = { i, j };
                        return bestMove;
                    }
                    int score = minimax(0, false, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
                    board[i][j] = EMPTY;

                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = { i, j };
                    }
                }
            }
        }
        return bestMove;
    }

private:
    std::vector<std::vector<int>> board;

    bool checkDirection(int row, int col, Player player, int dRow, int dCol) {
        int count = 0;
        for (int i = -4; i <= 4; ++i) {
            int newRow = row + i * dRow;
            int newCol = col + i * dCol;

            if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE && board[newRow][newCol] == player) {
                ++count;
                if (count == WINNING_LENGTH)
                    return true;
            }
            else {
                count = 0;
            }
        }
        return false;
    }

    int heuristic() {
        int score = 0;

        // Оценка всех линий
        score += evaluateLines(AI);   // Оценка для AI
        score -= evaluateLines(HUMAN); // Оценка для человека (чтобы помешать выигрышу)

        return score;
    }

    int evaluateLines(Player player) {
        int score = 0;

        // Для каждого направления
        for (int dRow = -1; dRow <= 1; ++dRow) {
            for (int dCol = -1; dCol <= 1; ++dCol) {
                if (dRow == 0 && dCol == 0) continue; // Пропускаем (0, 0)

                for (int row = 0; row < BOARD_SIZE; ++row) {
                    for (int col = 0; col < BOARD_SIZE; ++col) {
                        if (board[row][col] == player) {
                            score += evaluateAdjacentLines(row, col, dRow, dCol, player);
                        }
                    }
                }
            }
        }
        return score;
    }

    int evaluateAdjacentLines(int row, int col, int dRow, int dCol, Player player) {
        int count = 0;
        int opponentCount = 0;

        // Проверяем максимум 4 ячейки в обе стороны
        for (int offset = -4; offset <= 4; ++offset) {
            int newRow = row + offset * dRow;
            int newCol = col + offset * dCol;

            if (newRow >= 0 && newRow < BOARD_SIZE && newCol >= 0 && newCol < BOARD_SIZE) {
                if (board[newRow][newCol] == player) {
                    ++count;
                }
                else if (board[newRow][newCol] == (player == AI ? HUMAN : AI)) {
                    ++opponentCount;
                }
            }
        }

        // Оценка на основе количества фишек
        if (count == 5) return 10000; // Победа
        if (count == 4 && opponentCount == 0) return 1000;
        if (count == 3 && opponentCount == 0) return 100;   
        if (count == 2 && opponentCount == 0) return 10;    
        if (count == 1 && opponentCount == 0) return 1;    
        if (opponentCount > 0) return -1000; // Блокируем

        return 0;
    }

    int minimax(int depth, bool isMaximizing, int alpha, int beta) {

        int eval;
        if (depth == MAX_DEPTH || isBoardFull()) {
            return heuristic();
        }

        if (isMaximizing) {
            int maxEval = std::numeric_limits<int>::min();
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    if (isValidMove(i, j)) {
                        board[i][j] = AI;
                        if (isWinningMove(i, j, AI))
                        {
                            eval = heuristic();
                        }
                        else
                        {
                            eval = minimax(depth + 1, false, alpha, beta);
                        }
                        board[i][j] = EMPTY;
                        maxEval = std::max(maxEval, eval);
                        alpha = std::max(alpha, eval);
                        if (beta <= alpha) break;
                    }
                }
            }
            return maxEval;
        }
        else {
            int minEval = std::numeric_limits<int>::max();
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    if (isValidMove(i, j)) {
                        board[i][j] = HUMAN;
                        if (isWinningMove(i, j, HUMAN))
                        {
                           eval = heuristic();
                        }
                        else eval = minimax(depth + 1, true, alpha, beta);
                        board[i][j] = EMPTY;
                        minEval = std::min(minEval, eval);
                        beta = std::min(beta, eval);
                        if (beta <= alpha) break;
                    }
                }
            }
            return minEval;
        }
    }
};

int main() {
    setlocale(LC_ALL, "RU");

    Gomoku game;
    game.printBoard();

    while (true) {
        int row, col;
        std::cout << "_____________________________________________\n\nВведите координаты вашего хода (строка и столбец): ";
        std::cin >> row >> col;

        if (game.makeMove(row, col, HUMAN)) {
            game.printBoard();

            if (game.isWinningMove(row, col, HUMAN)) {
                std::cout << "Вы победили!" << std::endl;
                break;
            }

            Move bestMove = game.getBestMove();
            std::cout << "_____________________________________________\n\nAI сделал ход " << bestMove.row << " " << bestMove.col << "\n";
            game.makeMove(bestMove.row, bestMove.col, AI);
            game.printBoard();

            if (game.isWinningMove(bestMove.row, bestMove.col, AI)) {
                std::cout << "AI победил!" << std::endl;
                break;
            }

            if (game.isBoardFull()) {
                std::cout << "Ничья!" << std::endl;
                break;
            }
        }
        else {
            std::cout << "Неверный ход. Попробуйте снова." << std::endl;
        }
    }

    return 0;
}
