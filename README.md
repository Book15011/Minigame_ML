# Chinese Checker (Simplified Version) – Reinforcement Learning (RL)

## Overview  

This project focuses on developing an AI bot using reinforcement learning techniques in three phases. The bot is trained through a structured learning process, progressively improving its performance through self-play and strategic adjustments.  

For details on the game mechanics and rules, please refer to **[GAME_RULES.md](GAME_RULES.md)**.  

## Mini-Game Development  
- Creating a playable game with two modes:  
  - **Human vs. Human**  
  - **Human vs. Bot**  

### Bot Training Process
- Implementing reinforcement learning to create an intelligent AI opponent.

## Bot Training Phases

### Phase 1: Initial Training (Q-Learning)
- Train two AI bots using Machine Learning (ML) to generate a Q-table.
- Bots play against each other in the mini-game, updating their Q-values based on the reward system.

### Phase 2: Self-Improvement via Competition
- The bot trained in Phase 1 is used as the baseline.
- A new bot is created and optimized to find the best Q-values that can beat the best-performing bot at that time.
- The new bot competes against the best bot over a streak of 100 games.
- The Q-values from the best-performing bot are collected and used for further training.
- This process is repeated iteratively to progressively improve the AI.

### Phase 3: Reward Optimization and Tournament
- Adjust the weighting of rewards and punishments to fine-tune the bot’s decision-making.
- Implement a tournament system where multiple versions of the bot compete.
- The best-performing bot is selected as the final version.

## Features
- Q-learning-based AI training
- Progressive bot improvement through self-play
- Dynamic reward system for fine-tuning performance
- Support for both Human vs. Human and Human vs. Bot gameplay modes

## Installation

Clone the repository:
```sh
git clone https://github.com/Book15011/Minigame_ML/FullProject.git
```

Install dependencies:
```sh
pip install -r requirements.txt
```

Run the game:
```sh
./FullProject.c
```

## Quick Test: 
- In the FullProject folder, run FullProject.c to see how the game works. You can play against the bot, which already has pre-trained Q-values implemented in aiplayer.c.

To train the bot, follow these methods:

### 1. Creating a New Bot (Training from Zero)
- Open the `BaseTraining` folder.
- Run the `Original_project1` file.
- Set the number of episodes to train. More training results in a more stable bot.

### 2. Improving the Current Bot
- Open the `Improving_Bot` folder.
- Run `project1_Teach` to find and refine a better bot.

### 3. Evaluating and Selecting the Best Bot
- Open the `FightBot_Simulation` folder.
- Run the `Battle_Field` file to compare multiple bot models.
- Modify the `.csv` data input at the top of the `int main()` function to specify the models being tested.
- The evaluation process determines the strongest bot among all available models.

## Future Enhancements  
- Implement deep reinforcement learning (DQN) for more complex decision-making.
- Expand the game with additional challenges to further train the AI.
- Improve GUI for better user interaction.

## Contributions
Contributions are welcome! Feel free to submit pull requests or report issues.
