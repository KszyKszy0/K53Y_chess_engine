# K53Y chess engine

K53Y chess engine is my amateur attempt at creating what always puzzled me. Working of chess engines in general.

### 1. Current Features
- Alpha-Beta pruning
-  Transposition Table
- Check extensions
- Quiescence search
- Move Ordering: 
	- MVV-LVA
	- TT-move
	- History Heuristic
- PVS
- NMP
- Killer moves
- Basic Time Management
- Aspiration Windows
- Iterative deepening
***

### 2. Evaluation
- First version used hand-crafted PSQT, which later were tuned.
- Now, I slowly move toward using NNUE with SIMD, I've made some preparations already but as of now results aren't great (*Who would've guessed that experimenting on my own would yield worse results than doing what everyone else does*).
***

### 3. Future plans
- I still lack a few basic upgrades, but I haven't got enough time recently to add them.
- The evaluation is still miserable, it will need a lot of work to make it at least viable.
***

### 4. Special thanks
- Sebastian Lague for sharing his work on chess engines, which heavily inspired me to start making one.
- Whole Chess Programming community on discord for helping with some questions.
