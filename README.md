# Minigame_ML
Bot Training and Mini-Game Project

Overview

This project focuses on developing an AI bot using reinforcement learning techniques in three phases. The bot is trained through a structured learning process, progressively improving its performance through self-play and strategic adjustments. The project consists of two main components:

Mini-Game Development - Creating a playable game with two modes:

Human vs. Human

Human vs. Bot

Bot Training Process - Implementing reinforcement learning to create an intelligent AI opponent.

Bot Training Phases

Phase 1: Initial Training (Q-Learning)

Train two AI bots using Machine Learning (ML) to generate a Q-table.

Bots play against each other in the mini-game, updating their Q-values based on the reward system.

Phase 2: Self-Improvement via Competition

The bot trained in Phase 1 is used as the baseline.

A new bot is trained by playing against the previous best-performing bot.

Training continues until the new bot consistently wins against the older version.

This process is repeated iteratively to progressively improve the AI.

Phase 3: Reward Optimization and Tournament

Adjust the weighting of rewards and punishments to fine-tune the bot’s decision-making.

Implement a tournament system where multiple versions of the bot compete.

The best-performing bot is selected as the final version.

Features

Q-learning-based AI training

Progressive bot improvement through self-play

Dynamic reward system for fine-tuning performance

Support for both Human vs. Human and Human vs. Bot gameplay modes

Installation

Clone the repository:

git clone https://github.com/yourusername/your-repo.git

Install dependencies:

pip install -r requirements.txt

Run the game:

python main.py

Usage

To play the mini-game, run main.py and choose your mode.

To train the bot, execute the training script:

python train.py

Future Enhancements

Implement deep reinforcement learning (DQN) for more complex decision-making.

Expand the game with additional challenges to further train the AI.

Improve GUI for better user interaction.

License

This project is open-source under the MIT License.

Contributions are welcome! Feel free to submit pull requests or report issues.

