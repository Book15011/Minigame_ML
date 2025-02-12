#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "Original_game.h"
#define RECENT_MOVES 3
// Constants for Q-learning
#define ACTIONS 8889
#define ALPHA 0.1
#define GAMMA 0.9
static double EPSILON = 0.99;

// Q-tables for each player
double Q_table_blue[BOARD_SIZE][ACTIONS] = {0};
double Q_table_red[BOARD_SIZE][ACTIONS] = {0};


int BASE_POSITION_RED[] = {86, 87, 88, 68, 77, 78};
int BASE_POSITION_BLUE[] = {11, 12, 13, 21, 22, 31};

int recent_positions_red[RECENT_MOVES] = {0};
int recent_positions_blue[RECENT_MOVES] = {0};

void loadQTableFromFile1(const char* filename) {
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
            Q_table_red[row][col] = atof(token); // Convert the string to a float and store it

            token = strtok(NULL, ","); // Get the next action value
            col++;

            //printf ("token: %s\n", token);
        }
        row++;

    }

    fclose(file); // Close the file after reading
    printf("Q-table loaded from %s\n", filename);
}


void loadQTableFromFile2(const char* filename) {
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

void printCheckQtable(){
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < ACTIONS; j++){
            if ( Q_table_blue[i][j] != 0){
            printf("Q_table_blue[%d][%d] = %f\n", i, j, Q_table_blue[i][j]);
            }
        }
    }
}


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



//check repetitive
bool isRepetitiveMove(int end, int player) {
    int* recent_positions = (player == RED) ? recent_positions_red : recent_positions_blue;

    for (int i = 0; i < RECENT_MOVES; i++) {
        if (recent_positions[i] == end) {
            //printf("Player %d repeated move detected. recent_positions[%d]: %d, end: %d\n", player, i, recent_positions[i], end);
            return true;  // Position is in recent positions, indicating a loop
        }
    }
    return false;
}

void updateRecentPositions(int position, int player) {
    int* recent_positions = (player == RED) ? recent_positions_red : recent_positions_blue;

    for (int i = RECENT_MOVES - 1; i > 0; i--) {
        recent_positions[i] = recent_positions[i - 1];
    }
    recent_positions[0] = position;
}

bool isDiagonalMove(int start, int end) {
    int start_x = start % 10; // Assuming the board has positions like 11, 12, ..., 99
    int start_y = start / 10;
    int end_x = end % 10;
    int end_y = end / 10;

    return (abs(end_x - start_x) == abs(end_y - start_y));  // Diagonal condition
}

// Update Q-table
void updateQTable(int start, int end, int player, int reward) {
    int bestAction = selectBestMove(end, player);
    double bestFutureQ = (bestAction != -1) ? Q_table_blue[end][bestAction] : 0.0;  // Use the max Q-value
    int action = start * 100 + end;
    //1133

    // Update Q-table based on player and print debug info
    if (player == BLUE) {
        //printf ("Blue bestFutureQ: %lf\n",bestFutureQ );
        //printf ("Q_table before %lf\n", Q_table_blue[start][action]);
        Q_table_blue[start][action] += ALPHA * (reward + GAMMA * bestFutureQ - Q_table_blue[start][action]);
        //printf ("Q_table after %lf\n", Q_table_blue[start][action]);
        //printf("BLUE player Q-table updated: start %d, end %d, reward %d, Q-value %.2f\n", start, end, reward, Q_table_blue[start][action]);
    } else if (player == RED) {
        //printf ("Red bestFutureQ: %lf\n", bestFutureQ);
        //printf ("Q_table before %lf\n", Q_table_red[start][action]);
        Q_table_red[start][action] += ALPHA * (reward + GAMMA * bestFutureQ - Q_table_red[start][action]);
        //printf ("Q_table after %lf\n", Q_table_red[start][action]);
        //printf("RED player Q-table updated: start %d, end %d, reward %d, Q-value %.2f\n", start, end, reward, Q_table_red[start][action]);
    }
}

// Select the best move based on Q-table for the current player
int selectBestMove(int start, int player) {
    int bestAction = -1;
    double maxQ = -1e9;
    double (*Q_table)[ACTIONS] = (player == BLUE) ? Q_table_blue : Q_table_red;

    //printf("Evaluating best move for start position %d, player %d\n", start, player);

    for (int end = 1; end < BOARD_SIZE; end++) {
        int action = start * 100 + end;
        if (isValidMove(start, end, player)) {
            //printf ("\n%d\t%d\n\n", start, action);
            //printf("Checking move %d -> %d with Q-value: %.2f\n", start, end, Q_table[start][action]);
            if (Q_table[start][action] > maxQ) {
                maxQ = Q_table[start][action];
                bestAction = action;
                //printf("New best action for player %d: %d -> %d with Q-value: %.2f\n", player, start, end, maxQ);
            }
        }
    }
    //printf("Best action chosen for player %d: %d -> %d\n", player, bestAction / 100, bestAction % 100);
    return bestAction;
}

// Initializes the game state
int initializeGameState() {
    initializeBoard();
    return 11;  // Starting position for blue pieces
}

//reward
int makeMove(int start, int end, int player, int movetrain) {
    int reward = 0;
    int number_blue = 0;
    int number_red = 0;

    static int blue_in_base_count[6] = {0};  // Tracks how many times each BLUE piece is moved in the base
    static int red_in_base_count[6] = {0};   // Tracks how many times each RED piece is moved in the base

    int check_blue_not = 0;
    int check_red_not = 0;


    // Check if the move is valid
    if (isValidMove(start, end, player)) {
        board[end] = board[start];
        board[start] = EMPTY;

        // High reward for winning the game

        if (isGameOver(movetrain) == player) {
            reward += 200;
            printf ("hey I donneeeee\n");
        }
        // Base position rewards to encourage reaching the opponent's base
        if (player == BLUE) {
            for (int i = 0; i < 6; i++){
                for (int j = 0; j < 6; j++){
                    if (start == BASE_POSITION_RED[i] && end == BASE_POSITION_RED[j] )
                        check_blue_not = -1;
                    }
                if (BASE_POSITION_RED[i] == end) {
                        reward += 85;
                    }
            }
            if (check_blue_not) reward -= 85;
        }

        else if (player == RED) {
            for (int i = 0; i < 6; i++){
                for (int j = 0; j < 6; j++){
                    if (start == BASE_POSITION_BLUE[i] && end == BASE_POSITION_BLUE[j] )
                        check_red_not = -1;
                    }
            if (BASE_POSITION_RED[i] == end) {
                    reward += 85;
                }
            }
            if (check_red_not) reward -= 85;
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
        } else if (player == RED) {  // Target is (1,1)
            start_distance = (start_row - 1) + (start_col - 1);
            end_distance = (end_row - 1) + (end_col - 1);
        }

        int distance_change = start_distance - end_distance;

        if (distance_change > 0) {
            reward += 5 * distance_change;  // Reward for progress
            //printf("Reward for progress: +%d\n", 5 * distance_change);

        } else {
            reward -= 5 * (-distance_change);  // Penalty for regression
            //printf("Reward for regression: %d\n", 10 * distance_change);
        }

        // Diagonal move logic
        /*
        if (isDiagonalMove(start, end)) {
            int rowChange = (end / 10) - (start / 10);
            int colChange = (end % 10) - (start % 10);

            if (player == BLUE && !(rowChange > 0 && colChange > 0)) {
                reward -= 10;  // Penalize diagonal moves not towards bottom-right
            } else if (player == RED && !(rowChange < 0 && colChange < 0)) {
                reward -= 10;  // Penalize diagonal moves not towards top-left
            } else {
                reward += rowChange * rowChange + colChange * colChange;  // Reward valid diagonal movement
            }
        }*/

        // Penalty for leaving opponent's base
        if (player == BLUE) {
            for (int i = 0; i < 6; i++) {
                if (start == BASE_POSITION_RED[i] && end != BASE_POSITION_RED[i]) {
                    reward -= 70;  // Penalty for leaving base
                    break;
                }
            }
        } else if (player == RED) {
            for (int i = 0; i < 6; i++) {
                if (start == BASE_POSITION_BLUE[i] && end != BASE_POSITION_BLUE[i]) {
                    reward -= 70;  // Penalty for leaving base
                    break;
                }
            }
        }
/*
        // Penalize idleness: Staying in the same or nearby positions
        if (abs(end - start) == 1 || abs(end - start) == 10) {
            reward -= 7;  // Small penalty for minimal progress
        }

  // Move History Logic for Repetitive Moves
        static int blue_move_history[10][2];  // History for BLUE player: [start, end]
        static int red_move_history[10][2];   // History for RED player: [start, end]
        static int blue_move_index = 0;       // Circular index for BLUE
        static int red_move_index = 0;        // Circular index for RED

        if (player == BLUE) {
            // Add the current move to BLUE's history
            blue_move_history[blue_move_index][0] = start;
            blue_move_history[blue_move_index][1] = end;
            blue_move_index = (blue_move_index + 1) % 3;  // Keep history size at 10

            // Check for repetition within BLUE's history
            int repeat_count = 0;
            for (int i = 0; i < 3; i++) {
                if (blue_move_history[i][0] == start && blue_move_history[i][1] == end) {
                    repeat_count++;
                }
            }

            if (repeat_count > 2) {  // Allow some repetitions before penalizing
                reward -= 50 * (repeat_count - 2);  // Penalize excess repetition
            }
        }
        else if (player == RED) {
            // Add the current move to RED's history
            red_move_history[red_move_index][0] = start;
            red_move_history[red_move_index][1] = end;
            red_move_index = (red_move_index + 1) % 3;  // Keep history size at 10

            // Check for repetition within RED's history
            int repeat_count = 0;
            for (int i = 0; i < 3; i++) {
                if (red_move_history[i][0] == start && red_move_history[i][1] == end) {
                    repeat_count++;
                }
            }

            if (repeat_count > 2) {  // Allow some repetitions before penalizing
                reward -= 50 * (repeat_count - 2);  // Penalize excess repetition
            }
        }

        // Penalty for repetitive moves (not that work have to mark which ball move)
        /*
        if (isRepetitiveMove(end, player)) {
            reward -= 30;  // Larger penalty for repeating moves
            printf("Penalty for looping move applied. New reward: %d\n", reward);
        }

        // Update recent positions to prevent loops
        updateRecentPositions(end, player);*/

        //printf("Player %d made a move: %d -> %d, Reward: %d\n", player, start, end, reward);

        //displayBoard();
    } else {
        // Penalty for invalid moves
        //reward = -30;
        printf("Invalid move for player %d: %d -> %d, Penalty applied: %d\n", player, start, end, reward);
    }

    return reward;
}

// Training function for both players
void train(int episodes) {
    for (int episode = 0; episode < episodes; episode++) {
        int start = initializeGameState();  // Initializes or resets the game state at the start of each episode
        int player = BLUE;  // Starting player
        int done = 0;

        int movetrain = 0;
        printf("\n=== Episode %d ===\n", episode + 1);
        initializeBoard();  // Reset the board for each new episode
        //displayBoard();  // Display initial board

        while (!done) {
            int move = getMove(player);  // Choose a move using epsilon-greedy policy
            int startPos = move / 100;
            int endPos = move % 100;

            //printf("Player %d selected move: %d -> %d\n", player, startPos, endPos);

            movetrain++;

            // Execute the move and get the reward
            int reward = makeMove(startPos, endPos, player, movetrain);
            //printf("Reward for player %d's move: %d\n", player, reward);


            // Update the Q-table based on the move and obtained reward
            updateQTable(startPos, endPos, player, reward);

            // Check if the game is finished after the move
            done = isGameOver(movetrain);  // Checks if the game has reached a terminal state
            if (done) {
                printf("Game over! Winner: %d\n", done);
                break;
            }

            // Switch to the other player
            player = (player == BLUE) ? RED : BLUE;  // Switches player between BLUE and RED
        }

        // Optionally decay EPSILON to reduce exploration over time
        if (EPSILON > 0.01) {
            EPSILON *= 0.997;  // Gradually reduce EPSILON
            printf("EPSILON adjusted to: %.3f\n", EPSILON);
        }
        printf("Episode %d finished\n", episode + 1);

        if (episode == 9999) {

            //printf("Before saving Q-table for BLUE player:\n");
            //printQTable(Q_table_blue, "BLUE");

            //printf("Before saving Q-table for RED player:\n");
            //printQTable(Q_table_red, "RED");

            saveQTableToFile(Q_table_blue, "Q_table_blue_NOT_ASS.csv");
            saveQTableToFile(Q_table_red, "Q_table_red_NOT_ASS.csv");
        }


    }
}

// Function to get a move for the bot (epsilon-greedy) based on the player’s Q-table
int getMove(int player) {
    int start, end;
    double (*Q_table)[ACTIONS] = (player == BLUE) ? Q_table_blue : Q_table_red;
    //printf ("\n\n%d\n", isGameOver(movetrain));

    if (((double)rand() / RAND_MAX) < EPSILON) {  // Exploration
        do {
            start = rand() % BOARD_SIZE;
            end = rand() % BOARD_SIZE;
        } while (!isValidMove(start, end, player));
        //printf("Player %d is exploring: selected random move %d -> %d\n", player, start, end);
    }
    else {  // Exploitation
        int bestAction = selectBestMove(start, player);
        if (bestAction != -1) {
            start = bestAction / 100;
            end = bestAction % 100;
            //printf("Player %d is exploiting: selected best move %d -> %d\n", player, start, end);
        } else {
            do {
                start = rand() % BOARD_SIZE;
                end = rand() % BOARD_SIZE;
            } while (!isValidMove(start, end, player));
            //printf("No valid best move for player %d, selected random move %d -> %d\n", player, start, end);
        }
    }
    return start * 100 + end;
}

