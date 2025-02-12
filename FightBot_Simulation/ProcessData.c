#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "fight_game.h"

#define BOARD_SIZE 89
#define ACTIONS 8889

#define RECENT_MOVES 3
// Constants for Q-learning
#define ALPHA 0.1
#define GAMMA 0.9
#define EPSILON 0.2

// Q-tables for each player
double Q_table_blue[BOARD_SIZE][ACTIONS] = {0};
double Q_table_red[BOARD_SIZE][ACTIONS] = {0};

int BASE_POSITION_RED[] = {86, 87, 88, 68, 77, 78};
int BASE_POSITION_BLUE[] = {11, 12, 13, 21, 22, 31};

int firstMove_Red = -1;
int firstMove_Blue = -1;
//check Q-table
void printCheckQtable(){
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < ACTIONS; j++){
            if ( Q_table_blue[i][j] != 0){
            printf("Q_table_blue[%d][%d] = %f\n", i, j, Q_table_blue[i][j]);
            }
        }
    }
}


// Load the Q-table for the second bot
void loadQTableRedFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return;
    }

    char line[1000000];
    int row = 0;

    // Skip the header row
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file) != NULL && row < BOARD_SIZE) {
        int col = 0;
        char* token = strtok(line, ",");
        token = strtok(NULL, ","); // Skip the first column (State X)
        while (token != NULL && col < ACTIONS) {
            Q_table_red[row][col] = atof(token);
            token = strtok(NULL, ",");
            col++;
        }
        row++;
    }

    fclose(file);
    printf("Q-table for RED loaded from %s\n", filename);
}


// Load the Q-table from a file
void loadQTableFromFile(const char* filename) {
    FILE *file = fopen(filename, "r"); // Open the file for reading
    if (file == NULL) {
        perror("Error opening file for reading");
        return;
    }

    char line[1000000];
    int row = 0;

    // Skip the header row
    fgets(line, sizeof(line), file); // Read and discard the header row
    //printf("%s", fgets(line, sizeof(line), file));
    // Read the Q-table values from the CSV

    while (fgets(line, sizeof(line), file) != NULL && row < BOARD_SIZE) {
        int col = 0;
        char* token = strtok(line, ","); // Tokenize the line by commas

        //printf ("token: %s\n", token);
        // Skip the first token (State X), which is the row header
        token = strtok(NULL, ","); // Get the first action value
        while (token != NULL && col < ACTIONS) {
            Q_table_blue[row][col] = atof(token); // Convert the string to a float and store it
            token = strtok(NULL, ","); // Get the next action value
            col++;
            //printf ("token: %s\n", token);
        }
        row++;
    }

    fclose(file); // Close the file after reading
    printf("Q-table loaded from %s\n", filename);
}

// Select the best move based on the Q-table
int mirrorPosition(int pos) {
    int row = pos / 10;
    int col = pos % 10;

    // Mirror row and column positions for RED's perspective
    int mirroredRow = 9 - row;  // Rows: 1 ↔ 8, 2 ↔ 7, etc.
    int mirroredCol = 9 - col;  // Columns: 1 ↔ 8, 2 ↔ 7, etc.

    return mirroredRow * 10 + mirroredCol;
}

int getMove(int player) {
    printf("here\n");

    int start = -1, end = -1;
    double maxQ = -1e9;
    int bestStart = -1, bestEnd = -1;


    int piecesInBase = 0;
    int targetBase = -1;


    for (int i = 0; i < 6; i++) {
        if (board[BASE_POSITION_RED[i]] == BLUE) {
            piecesInBase++;
        } else {
            targetBase = BASE_POSITION_RED[i];

        }
    }

    if ((piecesInBase == 5) && (targetBase == 88)&& (firstMove_Blue == -1)){
        printf("move for my frindss\n");
        firstMove_Blue = 0;
        return 7788;
    }

    if (piecesInBase == 5 && targetBase != -1) {

        for (int s = 0; s < BOARD_SIZE; s++) {
            if (board[s] == BLUE) {
                bool inBase = false;

                // Check if the piece is already in the opponent's base
                for (int i = 0; i < 6; i++) {
                    if (s == BASE_POSITION_RED[i]) {
                        inBase = true;
                        break;
                    }
                }

                if (!inBase) {
                    start = s;  // Found the RED piece outside the base
                    break;
                }
            }
        }

        if (start == -1) {
            //printf("No remaining RED player outside the base. Forfeiting turn.\n");
            return -1;
        }

        int minDistance = 1e9;  // Large initial value for distance comparison

        // Find the best move to get closer to the target base
        for (int e = 0; e < BOARD_SIZE; e++) {
            if (isValidMove(start, e, player)) {
                int startRow = start / 10, startCol = start % 10;
                int endRow = e / 10, endCol = e % 10;
                int targetRow = targetBase / 10, targetCol = targetBase % 10;

                int distanceAfterMove = abs(endRow - targetRow) + abs(endCol - targetCol);

                if (distanceAfterMove < minDistance) {
                    minDistance = distanceAfterMove;
                    bestStart = start;
                    bestEnd = e;
                }
            }
        }

        if (bestStart != -1 && bestEnd != -1) {
            //printf("Bot Blue moves: %d -> %d (targeting base %d, distance: %d)\n",bestStart, bestEnd, targetBase, minDistance);
            return bestStart * 100 + bestEnd;
        } else {
            //printf("No valid moves found for the last Blue player. Forfeiting turn.\n");
            return -1;
        }
    }

    // General Q-table logic for normal moves
    for (int s = 0; s < BOARD_SIZE; s++) {
        for (int e = 0; e < BOARD_SIZE; e++) {
            if (isValidMove(s, e, player)) {


                // Mirror positions for RED's perspective
                int mirroredStart = mirrorPosition(s);
                int mirroredEnd = mirrorPosition(e);

                // Map action for mirrored positions
                int mirroredAction = mirroredStart * 100 + mirroredEnd;

                // Debug: Print Q-value and mirrored action
                //printf("%lf \t action: %d\n", Q_table_red[mirroredStart][mirroredAction], 9999 - mirroredAction);

                if (Q_table_red[mirroredStart][mirroredAction] > maxQ) {
                    maxQ = Q_table_red[mirroredStart][mirroredAction];
                    bestStart = s;
                    bestEnd = e;

/*

                int action = s * 100 + e;  // Encode the move
                //printf("%lf \t action: %d\n", Q_table_blue[s][action], action);
                if (Q_table_blue[s][action] > maxQ) {
                    maxQ = Q_table_blue[s][action];
                    bestStart = s;
                    bestEnd = e;
*/
                }
            }
        }
    }

    if (bestStart == -1 || bestEnd == -1) {
        //printf("No valid moves available. Forfeiting turn.\n");
        return -1;
    }

    //printf("Bot Blue chooses: %d -> %d (Q-value: %f)\n", bestStart, bestEnd, maxQ);
    return bestStart * 100 + bestEnd;  // Encoded move
}



int getMoveRed(int player) {
    int start = -1, end = -1;
    int targetBase = -1;
    int minDistance = 1e9; // Large initial value for comparison
    int bestStart = -1, bestEnd = -1;


    // Step 1: Count the number of RED pieces in BLUE's base and identify the target base
    int redInBase = 0;
    for (int i = 0; i < 6; i++) {
        if (board[BASE_POSITION_BLUE[i]] == RED) {
            redInBase++;
        } else {
            targetBase = BASE_POSITION_BLUE[i]; // Set the last unoccupied base
        }
    }

    if ((redInBase == 5) && (targetBase == 11)&& (firstMove_Red == -1)){
        printf("move for my frindss\n");
        firstMove_Red = 0;
        return 2211;
    }
    // If 5 pieces are already in the base, handle the last one
    if (redInBase == 5 && targetBase != -1) {
        // Find the last RED piece not in the base
        for (int s = 11; s < BOARD_SIZE; s++) {
            if (board[s] == RED) {
                // Ensure this RED piece is not already in the opponent's base
                bool inBase = false;
                for (int i = 0; i < 6; i++) {
                    if (s == BASE_POSITION_BLUE[i]) {
                        inBase = true;
                        break;
                    }
                }
                if (!inBase) {
                    start = s;
                    break;
                }
            }
        }

        if (start == -1) {
            //printf("No remaining RED player outside the base. Forfeiting turn.\n");
            return -1;
        }

        // Step 3: Find the best valid move to get closer to the target
        for (int e = 11; e < BOARD_SIZE; e++) {
            if (isValidMove(start, e, player)) {
                // Calculate Manhattan distance to the target base
                int startRow = start / 10, startCol = start % 10;
                int endRow = e / 10, endCol = e % 10;
                int targetRow = targetBase / 10, targetCol = targetBase % 10;

                int distanceAfterMove = abs(endRow - targetRow) + abs(endCol - targetCol);

                // Choose the move that minimizes distance
                if (distanceAfterMove < minDistance) {
                    minDistance = distanceAfterMove;
                    bestStart = start;
                    bestEnd = e;
                }
            }
        }

        // Step 4: Execute the best move found
        if (bestStart != -1 && bestEnd != -1) {
            //printf("Bot RED moves: %d -> %d (targeting base %d, distance: %d)\n", bestStart, bestEnd, targetBase, minDistance);
            return bestStart * 100 + bestEnd;
        } else {
            //printf("No valid moves found for the last RED player. Forfeiting turn.\n");
            return -1;
        }
    }

    // Normal Q-table logic for other cases
    double maxQ = -1e9;
    for (int s = 0; s < BOARD_SIZE; s++) {
        for (int e = 0; e < BOARD_SIZE; e++) {
            if (isValidMove(s, e, player)) {

/*
                // Mirror positions for RED's perspective
                int mirroredStart = mirrorPosition(s);
                int mirroredEnd = mirrorPosition(e);

                // Map action for mirrored positions
                int mirroredAction = mirroredStart * 100 + mirroredEnd;

                // Debug: Print Q-value and mirrored action
                printf("%lf \t action: %d\n", Q_table_red[mirroredStart][mirroredAction], 9999 - mirroredAction);

                if (Q_table_red[mirroredStart][mirroredAction] > maxQ) {
                    maxQ = Q_table_red[mirroredStart][mirroredAction];
                    bestStart = s;
                    bestEnd = e;

*/
                int action = s * 100 + e;  // Encode the move
                //printf("%lf \t action: %d\n", Q_table_red[s][action], action);
                if (Q_table_red[s][action] > maxQ) {
                    maxQ = Q_table_red[s][action];
                    bestStart = s;
                    bestEnd = e;

                }
            }
        }
    }

    if (bestStart == -1 || bestEnd == -1) {
        //printf("No valid moves for RED bot. Forfeiting turn.\n");
        return -1;
    }

    //printf("Bot RED chooses: %d -> %d (Q-value: %f)\n", bestStart, bestEnd, maxQ);
    return bestStart * 100 + bestEnd;
}


//old
/*
int getMoveRed(int player) {
    int start, end;
    double maxQ = -1e9;
    int bestStart = -1, bestEnd = -1;

    for (int s = 0; s < BOARD_SIZE; s++) {
        for (int e = 0; e < BOARD_SIZE; e++) {
            if (isValidMove(s, e, player)) {
                // Mirror positions for RED's perspective
                int mirroredStart = mirrorPosition(s);
                int mirroredEnd = mirrorPosition(e);

                // Map action for mirrored positions
                int mirroredAction = mirroredStart * 100 + mirroredEnd;

                // Debug: Print Q-value and mirrored action
                printf("%lf \t action: %d\n", Q_table_red[mirroredStart][mirroredAction], 9999-mirroredAction);

                if (Q_table_red[mirroredStart][mirroredAction] > maxQ) {
                    maxQ = Q_table_red[mirroredStart][mirroredAction];
                    bestStart = s;
                    bestEnd = e;
                }
            }
        }
    }

    if (bestStart == -1 || bestEnd == -1) {
        printf("No valid moves for RED bot. Forfeiting turn.\n");
        return -1;
    }

    printf("Bot RED chooses: %d -> %d (Q-value: %f)\n", bestStart, bestEnd, maxQ);
    return bestStart * 100 + bestEnd;
}
*/
