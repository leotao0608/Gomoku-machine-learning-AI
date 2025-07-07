# **Gomoku AI**
An AI-powered Gomoku game implemented in C++ featuring intelligent move selection using Minimax algorithm with Alpha-Beta pruning. 
## Project Highlits
- **Language:** C++
- **Implementation Algrithm:** MinMax + Alpha Beta Pruning + Board Evaluation
- **About This Project:** This is a Gomoku (five-in-a-row) game with an intelligent computer player, developed in C++. The AI can challenge human players by evaluating moves using offensive and defensive strategies. The goal is to create a smart and efficient game engine and later expand it with machine learning.
- **What Can It Do:**
  - Play against a human  
  - Attach and defend intelligently
  - Find a move with highest winnign rate
## **[Try Game](https://leotao0608.github.io/Gomoku-machine-learning-AI/)**
## Introduction to Gomoku
Gomoku, also known as "Five in a Row" or "Gobang", is a two-player abstract strategy board game played on a 15*15 Go board.  
In this game, a player win by lining up five chess pieces of the same color, it can be in four directions, vertical, horizontal and 
diagnally.  
**See** *[Detailed Gomoku Rules](https://en.wikipedia.org/wiki/Gomoku)*

## Code description
## Gomoku AI (MinMax + Alpha-Beta Pruning) - C++ Implementation

### Overview
A C++ implementation of the Gomoku (Five-in-a-Row) game featuring an AI opponent using the **Minimax algorithm with Alpha-Beta pruning**. The game supports a 15×15 board with a console-based interface.

---

### Key Components

#### 1. `chessBoard` Class
Manages the game state and player interactions.

##### Features:
- **Board Representation**:  
  - `-1` = White (O)  
  - `0` = Empty (.)  
  - `1` = Black (X)  
- **Win Detection**: Checks 4 directions (horizontal, vertical, 2 diagonals) for 5 consecutive stones.
- **Move Validation**: Ensures moves are within bounds and on empty cells.
- **Player Switching**: Alternates turns between human (Black) and AI (White).

##### Methods:
- `placeAMove(x, y)`: Places a stone if valid.
- `checkStatus(x, y)`: Checks if the last move won the game.
- `printBoard()`: Displays the board with coordinates.

---

#### 2. `Node` Class
Represents game states for AI decision-making.

##### Features:
- **Move Generation**: Prioritizes moves near existing stones for efficiency.
- **Board Evaluation**: Scores positions using heuristic patterns:
  | Pattern        | Score  |
  |----------------|--------|
  | Five-in-a-Row  | 100000 |
  | Live Four      | 50000  |
  | Threat Four    | 10000  |
  | Live Three     | 5000   |
  | Threat Three   | 1000   |
  | Live Two       | 500    |
  | Threat Two     | 100    |
- **Defensive Bias**: Penalizes opponent threats 10% more (`enemy_score * 1.1`).

##### Methods:
- `GameOver(player)`: Checks if `player` has won.
- `evaluateBoard(player)`: Computes heuristic score.

---

#### 3. AI Logic (Minimax + Alpha-Beta Pruning)
##### Algorithm:
1. **Search Depth**: Configurable (`search_depth=3` by default).
2. **Recursive Evaluation**:
   - Maximizes AI's score while minimizing the human's best response.
   - Prunes irrelevant branches using Alpha-Beta to speed up search.
3. **Move Selection**: Returns the highest-scoring valid move.

##### Optimization:
- Focuses on nearby cells (`±2 units` around existing stones) to reduce the search space.

---

### How to Play
1. **Human Turn**:  
   - Input `(x, y)` coordinates (e.g., `7 7` for the center).  
   - Invalid moves are rejected.  

2. **AI Turn**:  
   - Computes the best move using Minimax.  
   - Displays thinking time and chosen move.  

3. **Winning**:  
   - The first player to align 5 chess pieces wins.  
