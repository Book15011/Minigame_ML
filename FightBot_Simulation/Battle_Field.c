#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "fight_game.h"
#include "fight_Bot.c"

// constant for the board representation
#define EMPTY 0
#define BLUE 1
#define RED 2
#define OUT_OF_BOUNDS -1
#define BOARD_SIZE 89
#define MAX_MOVES 200

// Global
int board[BOARD_SIZE]; // board

// prototypes
void initializeBoard();
void displayBoard();
int getMoveFromUser();
int isValidMove(int start, int end, int player);
int isGameOver(int moves);
void changePlayer(int *player);
void displayResult(int result);


// main function
int main() {
    const char* filename = "Q_table_blue_last.csv";
    // Load Q-table once for both bots
    loadQTableFromFile(filename);
    const char* filename1 = "Q_table_red_test2.csv";
    loadQTableRedFromFile(filename1);
    printCheckQtable();
    int player = BLUE;  // Blue starts
    int moveBotBlue, moveBotRed, start, end;
    int gameResult = 0;
    int moves = 0;

    initializeBoard();  // Initialize game board

    printf("Starting game: Bot (BLUE) vs Bot (RED)\n");

    while (!gameResult) {
        displayBoard();
        printf("\nTurn %d\n", moves / 2 + 1);
        printf("%s's turn\n", player == BLUE ? "Bot (BLUE)" : "Bot (RED)");

        if (player == BLUE) {
            moveBotBlue = getMove(player);  // BLUE bot uses original Q-table logic
            start = moveBotBlue / 100;
            end = moveBotBlue % 100;
            printf("Bot BLUE move: %d -> %d\n", start, end);
        } else {
            moveBotRed = getMoveRed(player);  // RED bot uses mirrored logic
            start = moveBotRed / 100;
            end = moveBotRed % 100;
            printf("Bot RED move: %d -> %d\n", start, end);
        }

        // Update the board
        board[end] = board[start];
        board[start] = EMPTY;

        moves++;
        // Check if the game is over
        gameResult = isGameOver(moves);

        // Change player
        if (!gameResult) {
            changePlayer(&player);
        }
    }

    displayResult(gameResult);
    return 0;
}


// initialize board
void initializeBoard() {
    // set out of bounds
    for (int i = 0; i < BOARD_SIZE; i++) {
        board[i] = OUT_OF_BOUNDS;
    }

    // define empty
    for (int row = 1; row <= 8; row++) {
        for (int col = 1; col <= 8; col++) {
            board[row * 10 + col] = EMPTY;
        }
    }

    // player base
    board[11] = board[12] = board[13] = board[21] = board[22] = board[31] = BLUE;
    board[86] = board[87] = board[88] = board[68] = board[77] = board[78] = RED;
}

// display the board
void displayBoard() {
    // Print column numbers
    printf("  ");
    for (int col = 1; col <= 8; col++) {
        printf("%d ", col);
    }
    printf("\n");

    for (int row = 1; row <= 8; row++) {
        printf("%d ", row);  // Print row number
        for (int col = 1; col <= 8; col++) {
            int pos = row * 10 + col;
            if (board[pos] == BLUE) printf("# ");
            else if (board[pos] == RED) printf("O ");
            else if (board[pos] == EMPTY) printf(". ");
        }
        printf("\n");
    }
}


// input position
int getMoveFromUser() {
    int move;
    //printf("Enter your move: ");
    scanf("%d", &move);
    return move;
}


//  check long-distance possible (including single jump)
int longJump(int start, int directionRow, int directionCol) {
    int rowStart = start / 10;
    int colStart = start % 10;

    int currRow = rowStart + directionRow;
    int currCol = colStart + directionCol;
    int piecesInMiddle = 0;



    //printf("Starting longJump from position %d in direction (%d, %d)\n", start, directionRow, directionCol);

    // finding first mid in the direction assign
    while (currRow >= 1 && currRow <= 8 && currCol >= 1 && currCol <= 8) {
        int currPos = currRow * 10 + currCol;
        //printf("Checking position %d (row: %d, col: %d)\n", currPos, currRow, currCol);

        int rowDistance = currRow-rowStart;
        int colDistance = currCol-colStart;

        // Check bounds or found the mid-piece
        if (board[currPos] != EMPTY) {
            piecesInMiddle++;
            //printf ("rowDistance:%d\t", rowDistance);
            //printf ("colDistance:%d\n", colDistance);
            int nextPosRow = currRow + rowDistance;
            int nextPosCol = currCol + colDistance;
            int nextPos = nextPosRow * 10 + nextPosCol;
            //printf("Found piece at %d, piecesInMiddle = %d\n", currPos, piecesInMiddle);
            //printf("nextPos %d\n", nextPos);

            // return possiblePos if valid
            if (piecesInMiddle == 1 && nextPosRow >= 1 && nextPosRow <= 8 &&
                nextPosCol >= 1 && nextPosCol <= 8 && board[nextPos] == EMPTY) {

                // This for check between mid and end have to clean
                int temp1 = currRow + directionRow;
                int temp2 = currCol + directionCol;
                int tempPos = temp1*10 + temp2;
                while (tempPos != nextPos){
                    if (board[tempPos] != EMPTY) return -1;
                        temp1 += directionRow;
                        temp2 += directionCol;
                        tempPos = temp1*10 + temp2;
                }
                //printf("Valid long jump position to %d\n", nextPos);
                return nextPos;
            }
            //printf("Breaking out of loop due to piece in middle or invalid position\n\n");
            break;
        }

        currRow += directionRow;
        currCol += directionCol;


    }

    //printf("No valid long jump found\n");
    return -1;
}

// Recursive multi-jump
int multiJump(int start, int end, int player, int count, int visited[89]) {
    if (count > 16 || start == end) {
        //printf("Reached maximum depth or end position\n");
        return start == end ? 1 : 0;
    }

    // Mark visit
    visited[start] = 1;

    int rowStart = start / 10;
    int colStart = start % 10;

    int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},   // Up, Down, Right, Left
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}  // all Diagonals
    };

    //printf("Starting multiJump from %d to %d, attempt #%d\n", start, end, count);

    for (int i = 0; i < 8; i++) {
        int newRow = rowStart + directions[i][0];
        int newCol = colStart + directions[i][1];

        // check bounds
        if (newRow < 1 || newRow > 8 || newCol < 1 || newCol > 8) continue;

        int newPos = longJump(start, directions[i][0], directions[i][1]);

        // skip if if cant long and already visited
        if (newPos != -1 && !visited[newPos]) {
            //printf("Valid jump found to %d\n", newPos);
            if (newPos == end) {
                //printf("Reached the end position %d\n", end);
                return 1;
            }

            // Continue with recursion if end not yet found
            if (multiJump(newPos, end, player, count + 1, visited)) {
                //printf("Path found via %d\n", newPos);
                return 1;
            }
        }
    }
    //printf("No valid path found from position %d\n", start);
    return 0; // No path
}

//  function check all move
int isValidMove(int start, int end, int player) {
    if (start < 11 || start > 88 || end < 11 || end > 88) return 0;

    if (board[start] != player) return 0;

    if (board[end] != EMPTY) return 0;

    int rowStart = start / 10;
    int colStart = start % 10;
    int rowEnd = end / 10;
    int colEnd = end % 10;
    int rowDiff = rowEnd - rowStart;
    int colDiff = colEnd - colStart;

    // Check single-step move
    if (abs(rowDiff) <= 1 && abs(colDiff) <= 1) return 1;

    // visited array for not inf loop
    int visited[89] = {0};

    // Check multi-jump
    int result = multiJump(start, end, player, 0, visited);
    //printf("multi output: %d\n", result);
    return result;
}


// check if the game over
int isGameOver(int moves) {
    int blueInRedCamp = 0, redInBlueCamp = 0;

    // Check Blue in red
    int redCampPositions[] = {86, 87, 88, 68, 77, 78};
    for (int i = 0; i < 6; i++) {
        if (board[redCampPositions[i]] == BLUE) {
            blueInRedCamp++;
        }
    }

    // Check Red pieces in blue
    int blueCampPositions[] = {11, 12, 13, 21, 22, 31};
    for (int i = 0; i < 6; i++) {
        if (board[blueCampPositions[i]] == RED) {
            redInBlueCamp++;
        }
    }

    // Blue wins
    if (blueInRedCamp == 6) return BLUE;

    //  Red wins
    if (redInBlueCamp == 6) return RED;

    //Draw
    //printf ("move: %d\n",moves);
    if (moves >= MAX_MOVES) return -1;
    // Game continues
    return 0;

}


// changing the current player
void changePlayer(int *player) {
    *player = (*player == BLUE) ? RED : BLUE;
}

//  result of the game
void displayResult(int result) {
    if (result == BLUE) {
        //printf("\nFinal Board State:\n");
        displayBoard();
        printf("Blue(#) wins!\n");
    }
    else if (result == RED){
        printf("Red(O) wins!\n");
        //printf("\nFinal Board State:\n");
        displayBoard();
    }

    else if (result == -1) {
            displayBoard();
            printf("draw\n");
    }
}
