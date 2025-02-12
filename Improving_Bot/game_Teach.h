#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define EMPTY 0
#define BLUE 1
#define RED 2
#define OUT_OF_BOUNDS -1
#define BOARD_SIZE 89

// Global board array
extern int board[BOARD_SIZE];

// Game function prototypes
void initializeBoard();
void displayBoard();
int isValidMove(int start, int end, int player);
int isGameOver(int moves);
void changePlayer(int *player);
void displayResult(int result);

// Bot function prototypes
int getMove(int player);
int getMoveRed(int player);
void train(int episodes);
void updateQTable(int start, int end, int player, int reward);
void loadQTableRedFromFile(const char* filename);
void printCheckQtable();
#endif
