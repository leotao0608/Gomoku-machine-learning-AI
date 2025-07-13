neutral network model 0-4 are based on training data 1-4. 
5: training data 1 augmented all data
6-7: training data 2 augmented. 0-29000; 29001-58000 
8-9: training data 3 augmented. 0-29000; 29001-58000 
10：training data offence moves. 
11: training data offence moves augmented.
12: training data 7
13: training data 7 augmented

Use MCTS AI to generate self play data, each training data(1-5) has 500 games included, about 20000 groups of training data. After training model 4, it has the ability of making live three, and blocking opponent attacks. But lack the ability of offence, especially when it has threat four of live four, it didn't know to add a chess piece to form five in a row. This is because the lack of training data and especially there are too less data cases where current player connect five to win. In other words, it didn't learn to connect five to win. So I flipped current data horizontally, vertically and both, creating about 200,0000 more groups of training, 4*50000. After training model 8, the model can play like normal but still lack the ability to offence. Next step is to manually add training data that forms five in a row, double live three and live three and threat four to increase the model's offence abilities. Training data 7 has 100 complete games included.
.
After training model 10, the model learned to connect the fifth chess piece.


cpp -> .json -> .npz -> .npz (augmented)
