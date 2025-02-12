#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Original_game.h"
#include "Original_Bot.c"

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

    srand(time(NULL));  // Seed random number generator

    // Training phase
    int episodes = 10000;  // Adjust as needed for training duration


    // Gameplay phase
    int player = BLUE;  // Blue goes first
    int move, start, end;
    int gameResult = 0;
    int moves = 0;
    train(episodes);       // Train the bot
    initializeBoard();  // Initialize game board

    printf("Starting game between BLUE and RED bots\n");

    while (!gameResult) {
        displayBoard();  // Show board state
        printf("Turn %d\n", moves/2 + 1);
        printf("\n%s's turn\n", player == BLUE ? "Blue" : "Red");

        // Bot move selection
        int validMove = 0;
        while (!validMove) {
            move = getMove(player);  // Get move from bot
            start = move / 100;
            end = move % 100;

            if (isValidMove(start, end, player)) {  // Validate move with isValidMove
                validMove = 1;
                printf("%s bot selected valid move: %d -> %d\n", player == BLUE ? "Blue" : "Red", start, end);
            } else {
                printf("Invalid move selected by %s bot, reattempting.\n", player == BLUE ? "Blue" : "Red");
            }
        }

        // Execute the move
        board[end] = board[start];
        board[start] = EMPTY;
        moves++;

        printf("Move executed: %d -> %d\n", start, end);

        // Check if the game has ended
        gameResult = isGameOver(moves);
        if (gameResult) {
            printf("Game over! Winner: %s\n", gameResult == BLUE ? "Blue" : "Red");
            break;
        }

        // Switch to the other player
        player = (player == BLUE) ? RED : BLUE;
    }

    displayBoard();       // Display final board state
    displayResult(gameResult);  // Display game outcome
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

            if (piecesInMiddle == 1 && nextPosRow >= 1 && nextPosRow <= 8 &&
                nextPosCol >= 1 && nextPosCol <= 8 && board[nextPos] == EMPTY) {

                // This for check between mid and end have to clean
                int temp1 = currRow + directionRow;
                int temp2 = currCol + directionCol;
                int tempPos = temp1*10 + temp2;
                while (tempPos != nextPos){
                    if (board[tempPos] != EMPTY) return 0;
                        temp1 += directionRow;
                        temp2 += directionCol;
                        tempPos = temp1*10 + temp2;
                }

                return nextPos;
            }
            break;
        }

        currRow += directionRow;
        currCol += directionCol;


    }

    return -1;
}

// Recursive multi-jump
int multiJump(int start, int end, int player, int count, int visited[89]) {
    if (count > 16 || start == end) {
        return start == end ? 1 : 0;
    }

    visited[start] = 1;

    int rowStart = start / 10;
    int colStart = start % 10;

    int directions[8][2] = {
        {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
    };


    for (int i = 0; i < 8; i++) {
        int newRow = rowStart + directions[i][0];
        int newCol = colStart + directions[i][1];

        if (newRow < 1 || newRow > 8 || newCol < 1 || newCol > 8) continue;

        int newPos = longJump(start, directions[i][0], directions[i][1]);

        if (newPos != -1 && !visited[newPos]) {
            if (newPos == end) {
                return 1;
            }

            if (multiJump(newPos, end, player, count + 1, visited)) {
                return 1;
            }
        }
    }
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
        displayBoard();
        printf("Blue(#) wins!\n");
    }
    else if (result == RED){
        printf("Red(O) wins!\n");
        displayBoard();
    }

    else if (result == -1) {
            displayBoard();
            printf("draw\n");
    }
}
