import json
import numpy as np
import os


json_file = "C:/Users/Lisaq/Documents/gomoku AI/self play code/trainning data offensive moves 1/training_data_offensive_moves_1.json"

x_list = []
y_list = []

with open(json_file, "r") as f:
    steps = json.load(f)


def augment(board, x, y):
    boards = []
    moves = []

    # initial
    #boards.append(board.copy())
    #moves.append((x, y))

    # vertical flip
    boards.append(np.flipud(board))
    moves.append((14 - x, y))

    # horizontal flip
    boards.append(np.fliplr(board))
    moves.append((x, 14 - y))

    # vertical + horizontal
    boards.append(np.fliplr(np.flipud(board)))
    moves.append((14 - x, 14 - y))

    return boards, moves


for step in steps:
    board = np.array(step["board"], dtype=np.float32)
    move_x = step["move_x"]
    move_y = step["move_y"]
    player = step["player"]

    board *= player  # opponent always -1

    boards_aug, moves_aug = augment(board, move_x, move_y)

    for b, (mx, my) in zip(boards_aug, moves_aug):
        move_index = mx * 15 + my
        move_onehot = np.zeros(225, dtype=np.float32)
        move_onehot[move_index] = 1.0

        x_list.append(b.reshape(15, 15, 1))
        y_list.append(move_onehot)


X = np.array(x_list, dtype=np.float32)
Y = np.array(y_list, dtype=np.float32)

np.savez("C:/Users/Lisaq/Documents/gomoku AI/self play code/x_train_augmented.npz", X)
np.savez("C:/Users/Lisaq/Documents/gomoku AI/self play code/y_train_augmented.npz", Y)
