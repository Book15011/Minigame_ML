# BaseTraining - User Guide

## **Overview**
The `BaseTraining` folder contains the initial Q-learning model used to train AI bots from scratch. This phase is essential for generating a baseline Q-table that allows the bot to make strategic decisions based on reinforcement learning.

## **Training Model Used**
- **Q-Learning Algorithm:** A reinforcement learning technique that helps the bot learn optimal moves by updating Q-values based on rewards.
- **Exploration Strategy:** Uses an **epsilon-greedy approach**, where the bot explores random moves with probability `epsilon` and exploits the best-known moves otherwise.
- **Reward System:** Encourages winning strategies and penalizes poor moves to refine decision-making.

## **How to Train a New Bot**

1. **Navigate to the BaseTraining folder.**
2. **Run the training script:**
   ```sh
   gcc Original_project1.c -o train_bot
   ./train_bot
   ```
3. **Set the number of episodes:**
   - The more training episodes, the more refined the Q-table.
   - Modify `#define EPISODES` in `Original_project1.c` to adjust training duration.
4. **Monitor Training Progress:**
   - The program will output Q-values and performance stats.
   - A trained Q-table is generated and stored for future bot improvements.

## **Modifying the Training Process**

- **Adjusting Epsilon (Exploration vs. Exploitation):**  
  - Modify `epsilon` value in `Original_project1.c` to control randomness.

- **Changing the Reward System:**  
  - Edit the reward assignment logic inside `update_q_values()`.

- **Training with Different Game Rules:**  
  - Modify the `evaluate_move()` function to reflect rule variations.

- **Using the Commented Reward Function:**  
  - In `Original_Bot.c`, the reward function inside
    it have 
    ```c
    int makeMove(int start, int end, int player, int movetrain)
    ```
    and some sub-funcitons are commented out for a specific reason in training process. If you want to use it, simply remove the comment.

After completing base training, you can move to the `/Improving_Bot/` section to enhance the AI further!

