#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "game_Teach.h"
#define RECENT_MOVES 3

// Constants for Q-learning
#define ACTIONS 8889
#define ALPHA 0.05
#define GAMMA 0.9
static double EPSILON = 0.99;

// Q-tables for each player
double Q_table_blue[BOARD_SIZE][ACTIONS] = {0};
double Q_table_red[BOARD_SIZE][ACTIONS] = {0};

int win_streak;
int firstMove = -1;
int BASE_POSITION_RED[] = {86, 87, 88, 68, 77, 78};
int BASE_POSITION_BLUE[] = {11, 12, 13, 21, 22, 31};

//Load Data Function
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


void printCheckQtable(){
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < ACTIONS; j++){
            if ( Q_table_red[i][j] != 0){
            printf("Q_table_red[%d][%d] = %f\n", i, j, Q_table_red[i][j]);
            }
        }
    }
}

//Input Data Checker
void printQTable(double Q_table[BOARD_SIZE][ACTIONS], const char* player_name) {
    printf("Q-table for player %s:\n", player_name);

    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("State %d: ", i);
        for (int j = 0; j < ACTIONS; j++) {
            if (Q_table[i][j] != 0) {  // Only print non-zero values for clarity
                printf("Action %d -> Q-value: %.2f  ", j, Q_table[i][j]);
            }
        }
        printf("\n");
    }
}
//Trained Data Saver Function
void saveQTableToFile(double Q_table[BOARD_SIZE][ACTIONS], const char* filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for Q-table");
        return;
    }

    // Write the header row
    fprintf(file, "State/Action");
    for (int j = 0; j < ACTIONS; j++) {
        fprintf(file, ",Action %d", j); // Add actions as column headers
    }
    fprintf(file, "\n");

    // Write the Q-table values
    for (int i = 0; i < BOARD_SIZE; i++) {
        fprintf(file, "State %d", i); // Add state as row header
        for (int j = 0; j < ACTIONS; j++) {
            fprintf(file, ",%.2f", Q_table[i][j]); // Write Q-values
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("Q-table saved to %s\n", filename);
}
//Check DiagonalMove
bool isDiagonalMove(int start, int end) {
    int start_x = start % 10;
    int start_y = start / 10;
    int end_x = end % 10;
    int end_y = end / 10;

    return (abs(end_x - start_x) == abs(end_y - start_y)); 
}

// Optimizing Q-table Function
void updateQTable(int start, int end, int player, int reward) {
    int bestAction = selectBestMove(end, player);
    double bestFutureQ = (bestAction != -1) ? Q_table_blue[end][bestAction] : 0.0;  // Use the max Q-value
    int action = start * 100 + end;

    // Update Q-table based on player and print Debug Text (You can used it to see how the value optimizing 
    if (player == BLUE) {
        //printf ("Blue bestFutureQ: %lf\n",bestFutureQ );
        //printf ("Q_table before %lf\n", Q_table_blue[start][action]);
        Q_table_blue[start][action] += ALPHA * (reward + GAMMA * bestFutureQ - Q_table_blue[start][action]);
        //printf ("Q_table after %lf\n", Q_table_blue[start][action]);
        //printf("BLUE player Q-table updated: start %d, end %d, reward %d, Q-value %.2f\n", start, end, reward, Q_table_blue[start][action]);
    }
}

// Select the best move based on Q-table for the current player Funciton
int selectBestMove(int player) {
    
    double maxQ = -1e9;  // Start with a very low Q-value
    int bestStart = -1, bestEnd = -1;

    // Iterate through all possible moves to find the best valid move
    for (int s = 0; s < BOARD_SIZE; s++) {
        for (int e = 0; e < BOARD_SIZE; e++) {
            if (isValidMove(s, e, player)) {
                int action = s * 100 + e;  // Encode the move
                if (Q_table_blue[s][action] > maxQ) {
                    maxQ = Q_table_blue[s][action];
                    bestStart = s;
                    bestEnd = e;
                }
            }
        }
    }

    // If no valid moves (shouldn't happen if rules are correct)
    if (bestStart == -1 || bestEnd == -1) {
        //printf("No valid moves available. Forfeiting turn.\n");
        return -1;  // Indicate no move possible
    }

    //printf("Bot Blue chooses: %d -> %d (Q-value: %f)\n", bestStart, bestEnd, maxQ);
    return bestStart * 100 + bestEnd;  // Encoded move
}

// Initializes the game state Function
int initializeGameState() {
    initializeBoard();
    return 11;
}

// Array to track the state of BLUE's base
static int blue_base_state[6] = {EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};

// Function to penalize previous moves
void penalizePreviousMove(int start, int end, int player, int reward) {
    updateQTable(start, end, player, reward);  // Apply the penalty
}


//The Reward and Punishment Function
int makeMove(int start, int end, int player, int movetrain) {
    int reward = 0;
    static int previous_start = -1, previous_end = -1, previous_player = -1;
    int number_blue = 0;
    // Track BLUE base state before move
    static int blue_in_base_count[6] = {0};
    int blue_base_before[6];
    //printf("DEBUG: BLUE base state before move:\n");
    for (int i = 0; i < 6; i++) {
        blue_base_before[i] = board[BASE_POSITION_BLUE[i]];
        //printf("  Position %d: %d\n", BASE_POSITION_BLUE[i], blue_base_before[i]);
    }

    // Move logic
    if (isValidMove(start, end, player)) {

        board[end] = board[start];
        board[start] = EMPTY;

        // Detect game over and assign rewards or penalties
        if (isGameOver(movetrain) == BLUE) {
            win_streak++;
            reward += 270;

        } else if (isGameOver(movetrain) == RED) {
            win_streak = 0;
            //reward -= 200;

        } else if (isGameOver(movetrain) == -1) {
            win_streak = 0;
            //reward -= 150;

        }

        if (player == BLUE) {
            for (int i = 0; i < 6; i++) {
                if (board[BASE_POSITION_RED[i]] == BLUE) {
                    number_blue++;
                    if (start == BASE_POSITION_RED[i] && end == BASE_POSITION_RED[i]) {
                        blue_in_base_count[i]++;
                        if (blue_in_base_count[i] > 3) {  // Penalize repeated moves within the base
                            //reward -= 25 * blue_in_base_count[i];  // Reduced penalty to avoid overly harsh punishment
                        }
                    }
                }
                if (BASE_POSITION_RED[i] == end) {
                    reward += 60;  // Reward for occupying opponent's base
                }
            }
            reward += 70 * number_blue;  // Reward for the number of BLUE pieces in the opponent's base
            if (number_blue == 6) reward += 300;  // Increased reward for fully occupying the opponent's base
        }

        //new-metode
        int start_row = start / 10;
        int start_col = start % 10;
        int end_row = end / 10;
        int end_col = end % 10;

        int start_distance, end_distance;

        if (player == BLUE) {  // Target is (8,8)
            start_distance = (8 - start_row) + (8 - start_col);
            end_distance = (8 - end_row) + (8 - end_col);
        }
        int distance_change = start_distance - end_distance;

        if (distance_change > 0) {
            reward += 10 * distance_change;  // Reward for progress
            //printf("Reward for progress: +%d\n", 5 * distance_change);

        } else {
            reward -= 8 * (-distance_change);  // Penalty for regression
            //printf("Reward for regression: %d\n", 5 * distance_change);
        }


        for (int i = 0; i < 6; i++) {
            int current_piece = board[BASE_POSITION_BLUE[i]];
            //printf("  Position %d: %d -> %d\n", BASE_POSITION_BLUE[i], blue_base_before[i], current_piece);
            if (blue_base_before[i] == EMPTY && current_piece == RED) {
                // New RED piece detected in BLUE's base
                int penalty = -12;
                //printf("DEBUG: New RED piece detected in BLUE base at position %d. Penalty: %d\n",BASE_POSITION_BLUE[i], penalty);
                penalizePreviousMove(previous_start, previous_end, previous_player, penalty);
                break;
            }
        }

        previous_start = start;
        previous_end = end;
        previous_player = player;


        for (int i = 0; i < 6; i++) {
            if (start == BASE_POSITION_RED[i] && end != BASE_POSITION_RED[i]) {
                reward -= 70;  // Reduced penalty for leaving base
                break;
            }
        }


    } else {

    }
    //IF you want to see the how it moving you can use the displayBoard()
    //displayBoard();

    return reward;
}



// Training function for both players
void train(int episodes) {
    for (int episode = 0; episode < episodes; episode++) {
        int start = initializeGameState();  // Initializes or resets the game state at the start of each episode
        int player = BLUE;  // Starting player
        int done = 0;
        int movetrain = 0;

        // Store moves and rewards for this episode

        typedef struct {
            int startPos;
            int endPos;
            int player;
            int reward;
        } MoveRecord;

        MoveRecord moves[500];  // Array to store moves (adjust size if needed)
        int moveCount = 0;      // Counter for the number of moves in this episode

        printf("\n=== Episode %d ===\n", episode + 1);
        initializeBoard();  // Reset the board for each new episode

        while (!done) {
            int move = getMove(player);  // Choose a move using epsilon-greedy policy
            int startPos = move / 100;
            int endPos = move % 100;

            movetrain++;

            // Execute the move and get the reward
            int reward = makeMove(startPos, endPos, player, movetrain);

            // Store move and reward for this episode
            moves[moveCount++] = (MoveRecord){startPos, endPos, player, reward};

            // Update the Q-table based on the move and obtained reward
            updateQTable(startPos, endPos, player, reward);

            // Check if the game is finished after the move
            done = isGameOver(movetrain);  // Checks if the game has reached a terminal state

            if (done) {
                printf("Game over! Winner: %d\n", done);


                // If BLUE wins, boost rewards for all its moves in this episode
                if (done == BLUE) {
                    printf("BLUE wins! Boosting rewards for all BLUE moves...\n");

                    for (int i = 0; i < moveCount; i++) {
                            //&&  (win_streak%2 == 0)
                        if ((moves[i].player == BLUE) ) {
                            int boostedReward = moves[i].reward + 5;  // Add a bonus reward (adjust as needed)

                            //printf("Boosting reward for move %d -> %d: original %d, boosted %d\n", moves[i].startPos, moves[i].endPos, moves[i].reward, boostedReward);
                            updateQTable(moves[i].startPos, moves[i].endPos, moves[i].player, boostedReward);
                        }

                    }

                }
                else {
                    printf ("lost\n");
                    for (int i = 0; i < moveCount; i++) {
                        if (moves[i].player == BLUE) {
                            int boostedReward;
                            if (episodes > 100){

                             boostedReward = moves[i].reward - 5;  // Add a bonus reward (adjust as needed)
                            }

                            //printf("Boosting reward for move %d -> %d: original %d, boosted %d\n", moves[i].startPos, moves[i].endPos, moves[i].reward, boostedReward);
                            updateQTable(moves[i].startPos, moves[i].endPos, moves[i].player, boostedReward);
                        }

                    }

                }



                break;
            }

            // Switch to the other player
            player = (player == BLUE) ? RED : BLUE;
        }

        // Optionally decay EPSILON to reduce exploration over time
        if (EPSILON > 0.01) {
            EPSILON *= 0.997;  // Gradually reduce EPSILON
            printf("EPSILON adjusted to: %.3f\n", EPSILON);
        }
        printf("Episode %d finished\n", episodes + 1);
        printf ("winst: %d\n", win_streak);
        if (win_streak == 1){
            saveQTableToFile(Q_table_blue, "Q_table_blue_last.csv");
            printf("END\n");
            break;
        }
    }
}


// Function to get a move for the bot (epsilon-greedy) based on the player’s Q-table
int getMove(int player) {
    int start, end;
    double (*Q_table)[ACTIONS] = (player == BLUE) ? Q_table_blue : Q_table_red;
    if ((((double)rand() / RAND_MAX) < EPSILON) && (player == BLUE)) {  // Exploration
    do {
        start = rand() % BOARD_SIZE;
        end = rand() % BOARD_SIZE;
    } while (!isValidMove(start, end, player));
    }

    else{ // Exploitation
        int bestAction;
        if (player == BLUE){
            bestAction = selectBestMove(player);
            //printf ("player %d: BestAction %d\n", player, bestAction);
        }
        else{
            bestAction = getMoveRed(player);
            //printf ("player %d: BestAction %d\n", player, bestAction);
        }


        if (bestAction != -1) {
            start = bestAction / 100;
            end = bestAction % 100;
            //printf("Player %d is exploiting: selected best move %d -> %d\n", player, start, end);
        } else {
            do {
                start = rand() % BOARD_SIZE;
                end = rand() % BOARD_SIZE;
            } while (!isValidMove(start, end, player));
            printf("No valid best move for player %d, selected random move %d -> %d\n", player, start, end);
        }
    }
    return start * 100 + end;
}

//Detect the nearly final movement 
int mirrorPosition(int pos) {
    int row = pos / 10;
    int col = pos % 10;

    // Mirror row and column positions for RED's perspective
    int mirroredRow = 9 - row;  // Rows: 1 ↔ 8, 2 ↔ 7, etc.
    int mirroredCol = 9 - col;  // Columns: 1 ↔ 8, 2 ↔ 7, etc.

    return mirroredRow * 10 + mirroredCol;
}

//The Teacher's Bot moving function processing from Q-value table 
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

    if ((redInBase == 5) && (targetBase == 11)&& (firstMove == -1)){
        printf("move for my frindss\n");
        firstMove = 0;
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
            //printf("Bot RED moves: %d -> %d (targeting base %d, distance: %d)\n",bestStart, bestEnd, targetBase, minDistance);
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
                // Mirror positions for RED's perspective
/*
                int mirroredStart = mirrorPosition(s);
                int mirroredEnd = mirrorPosition(e);

                // Map action for mirrored positions
                int mirroredAction = mirroredStart * 100 + mirroredEnd;

                //printf("%lf \t action: %d\n", Q_table_red[mirroredStart][mirroredAction], 9999 - mirroredAction);

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

