#https://github.com/masterLazy/LazyFish/tree/main/dataset
#Tensor flow library, numpy
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Flatten
from tensorflow.keras.optimizers import Adam
import numpy as np
import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'  
os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'  

x_data = np.load('C:/Users/tao/Documents/GitHub/Gomoku AI two algrithms/self play code/dataset-1.1/x_train.npz')
X_train=x_data["arr_0"]
y_data = np.load('C:/Users/tao/Documents/GitHub/Gomoku AI two algrithms/self play code/dataset-1.1/y_train.npz')
Y_train=y_data["arr_0"]
#(258280, 15, 15)
X_train = X_train.reshape((-1, 225))  
Y_train = Y_train.reshape((-1, 225)) 

x_train=X_train[:5000]
y_train=Y_train[:5000]
#print(X_train[10])
#print(Y_train[10])

x_data.close()
y_data.close()

model = Sequential([
    Flatten(input_shape=(225,)),   
    Dense(128, activation='relu'), 
    Dense(128, activation='relu'), 
    Dense(225, activation='softmax')  
])

model.compile(
    optimizer=Adam(learning_rate=0.001),
    loss='categorical_crossentropy', 
    metrics=['accuracy']
)

model.fit(X_train, Y_train, epochs=20, batch_size=64)

model.save("C:/Users/tao/Documents/GitHub/Gomoku AI two algrithms/self play code/gomoku_model.h5")


