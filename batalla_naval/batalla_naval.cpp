#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
#include <string>

int main() {
    std::cout << "=================================================\n";
    std::cout << "------------Welcome to Battle Ship!--------------\n";
    std::cout << "=================================================\n\n";

    const short BOARD_SIZE = 10;
    const char WATER_CELL = '~';
    const char HIT_MARK = 'O';
    const char MISS_MARK = 'X';

    const short FIRST_SHIP = 6;
    const short LAST_SHIP = 3;
    const short MAX_PLACEMENT_ATTEMPTS = BOARD_SIZE * BOARD_SIZE;

    char hiddenBoardP1[BOARD_SIZE][BOARD_SIZE];
    char hiddenBoardP2[BOARD_SIZE][BOARD_SIZE];
    char viewBoardP1[BOARD_SIZE][BOARD_SIZE];
    char viewBoardP2[BOARD_SIZE][BOARD_SIZE];

    bool gameOver = false;
    bool grantExtraTurn = false;
    bool stayOnTurn = false;
    short currentPlayer = 0;

    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // -------- Initialize boards --------
    for(short i = 0; i < BOARD_SIZE; ++i) {
        for(short j = 0; j < BOARD_SIZE; ++j) {
            hiddenBoardP1[i][j] = WATER_CELL;
            hiddenBoardP2[i][j] = WATER_CELL;
            viewBoardP1[i][j] = WATER_CELL;
            viewBoardP2[i][j] = WATER_CELL;
        }
    }

    // -------- Place ships for both players --------
    for(short player = 1; player <= 2; ++player) {
        char (*board)[BOARD_SIZE] =
            (player == 1) ? hiddenBoardP1 : hiddenBoardP2;

        for(short shipSize = FIRST_SHIP; shipSize >= LAST_SHIP; --shipSize) {
            bool placed = false;
            short attempts = 0;
            char shipChar = '0' + shipSize;

            while(!placed && attempts < MAX_PLACEMENT_ATTEMPTS) {
                ++attempts;

                short orientation = std::rand() % 2;
                short row, col;

                if(orientation == 0) {
                    row = std::rand() % BOARD_SIZE;
                    col = std::rand() % (BOARD_SIZE - shipSize + 1);
                }
                else {
                    row = std::rand() % (BOARD_SIZE - shipSize + 1);
                    col = std::rand() % BOARD_SIZE;
                }

                bool canPlace = true;
                for(short i = 0; i < shipSize; ++i) {
                    short r = row + (orientation ? i : 0);
                    short c = col + (orientation ? 0 : i);
                    if(board[r][c] != WATER_CELL) {
                        canPlace = false;
                        break;
                    }
                }

                if(canPlace) {
                    for(short i = 0; i < shipSize; ++i) {
                        short r = row + (orientation ? i : 0);
                        short c = col + (orientation ? 0 : i);
                        board[r][c] = shipChar;
                    }
                    placed = true;
                }
            }

            if(!placed) {
                std::cout << "ERROR: Could not place ship size "
                    << shipSize << "\n";
                return 1;
            }
        }
    }

    std::string pause;
    std::cout << "Press Enter to start the game.";
    std::getline(std::cin, pause);

    // -------- Game loop --------
    while(!gameOver) {
        system("cls");

        if(!grantExtraTurn && !stayOnTurn) currentPlayer = (currentPlayer % 2) + 1;

        stayOnTurn = false;
        grantExtraTurn = false;

        // -------- Print view boards --------
        for(short p = 1; p <= 2; ++p) {
            std::cout << "Player " << p << ":\n";
            char (*view)[BOARD_SIZE] =
                (p == 1) ? viewBoardP1 : viewBoardP2;

            for(short i = 1; i <= BOARD_SIZE; ++i)
                std::cout << i << ' ';
            std::cout << '\n';

            for(short i = 0; i < BOARD_SIZE; ++i) {
                for(short j = 0; j < BOARD_SIZE; ++j)
                    std::cout << view[i][j] << ' ';
                std::cout << i + 1 << '\n';
            }
            std::cout << "----------------------\n\n";
        }

        // -------- Input --------
        short selectedRow = 0;
        short selectedCol = 0;
        bool validInput = false;

        while(!validInput) {
            short col, row;
            std::cout << "Player " << currentPlayer << ", enter column [1-10]: ";
            if(!(std::cin >> col)) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                continue;
            }

            std::cout << "Enter row [1-10]: ";
            if(!(std::cin >> row)) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                continue;
            }

            if(col < 1 || col > 10 || row < 1 || row > 10) {
                std::cout << "Out of bounds.\n";
                continue;
            }

            selectedCol = col - 1;
            selectedRow = row - 1;
            validInput = true;
        }

        // -------- Resolve shot --------
        if(currentPlayer == 1) {

            if(viewBoardP2[selectedRow][selectedCol] != WATER_CELL) {
                std::cout << "Already shot there!\n";
                stayOnTurn = true;
                continue;
            }

            if(hiddenBoardP2[selectedRow][selectedCol] != WATER_CELL) {
                viewBoardP2[selectedRow][selectedCol] = HIT_MARK;
                hiddenBoardP2[selectedRow][selectedCol] = HIT_MARK;
                std::cout << "Hit!\n";
                grantExtraTurn = true;
            }
            else {
                viewBoardP2[selectedRow][selectedCol] = MISS_MARK;
                hiddenBoardP2[selectedRow][selectedCol] = MISS_MARK;
                std::cout << "Miss!\n";
            }

        }
        else { // Player 2

            if(viewBoardP1[selectedRow][selectedCol] != WATER_CELL) {
                std::cout << "Already shot there!\n";
                stayOnTurn = true;
                continue;
            }

            if(hiddenBoardP1[selectedRow][selectedCol] != WATER_CELL) {
                viewBoardP1[selectedRow][selectedCol] = HIT_MARK;
                hiddenBoardP1[selectedRow][selectedCol] = HIT_MARK;
                std::cout << "Hit!\n";
                grantExtraTurn = true;
            }
            else {
                viewBoardP1[selectedRow][selectedCol] = MISS_MARK;
                hiddenBoardP1[selectedRow][selectedCol] = MISS_MARK;
                std::cout << "Miss!\n";
            }
        }

        // -------- Win check --------
        bool p1Alive = false, p2Alive = false;

        for(short i = 0; i < BOARD_SIZE; ++i) {
            for(short j = 0; j < BOARD_SIZE; ++j) {
                if(hiddenBoardP1[i][j] >= '3' && hiddenBoardP1[i][j] <= '6')
                    p1Alive = true;
                if(hiddenBoardP2[i][j] >= '3' && hiddenBoardP2[i][j] <= '6')
                    p2Alive = true;
            }
        }

        if(!p1Alive) {
            std::cout << "\n*** Player 2 wins! ***\n";
            break;
        }
        if(!p2Alive) {
            std::cout << "\n*** Player 1 wins! ***\n";
            break;
        }
    }

    return 0;
}