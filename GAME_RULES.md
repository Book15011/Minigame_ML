# Chinese Checker (Simplified Version) – Game Rules  

## 1. Introduction  
Chinese Checker is a turn-based board game for two players. The objective is to move all of one's pieces from their starting camp to the opponent’s camp before the other player does. Players can move by taking single-step moves or multi-jump moves over other pieces.  

## 2. Game Board  
- The game board consists of an **8x8 grid** (64 cells).  
- Each player has **six pieces**, represented by **blue** and **red** colors.  
- Each player's **camp** consists of **six cells** at opposite corners of the board:  
  - **Blue camp**: Upper-left corner.  
  - **Red camp**: Bottom-right corner.  
- The board is stored as a **one-dimensional array** of length **89**, where:  
  - **Valid board positions** range from **(11 to 88)**.  
  - **Unused positions** are marked as **-1** to prevent placement.  

## 3. Piece Representation  
- **0** → Empty cell.  
- **1** → Blue player's piece.  
- **2** → Red player's piece.  
- **-1** → Invalid position outside the board.  

## 4. Game Play Sequence  
### 4.1 Turn Order  
- The **blue player always moves first**.  
- Players take turns, moving one piece per turn.  

### 4.2 Valid Moves  
A player may move a piece in one of the following ways:  
1. **Single-step move**: Move to an **adjacent empty cell** in one of **eight possible directions** (orthogonal or diagonal).  
2. **Single-jump move**: Jump over a **single piece (own or opponent's)** into an **empty cell** immediately after it.  
3. **Long-distance jump**: Jump over **one piece** into an empty cell beyond it.  
4. **Multi-jump move**: Perform **multiple successive jumps** over single pieces, landing in empty cells.  

### 4.3 Invalid Moves  
A move is **invalid** if:  
- The piece attempts to move into a **non-empty cell**.  
- A jump move does not have **exactly one piece** between the start and end positions.  

## 5. Game End Conditions  
The game ends in one of the following ways:  
1. **Win Condition**: A player **wins** if, after a move, their opponent’s camp is **completely filled**, with **at least one of their own pieces in it**.  
2. **Draw Condition**: If **both players have made 100 moves** and neither has won, the game **ends in a draw**.  
