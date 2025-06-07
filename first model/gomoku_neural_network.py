#https://github.com/masterLazy/LazyFish/tree/main/dataset
#Tensor flow library, numpy
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D, Dense, Flatten
from tensorflow.keras.optimizers import Adam
import numpy as np
import os
from tensorflow.keras.callbacks import ReduceLROnPlateau

os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'
os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'


x_data = np.load('C:/Users/Lisaq/Documents/gomoku AI/dataset-1.0/x_train.npz')
X_train = x_data["arr_0"]  # (258280, 15, 15)

y_data = np.load('C:/Users/Lisaq/Documents/gomoku AI/dataset-1.0/y_train.npz')
Y_train = y_data["arr_0"]  # (258280, 15, 15)

X_train = X_train[:50000]
Y_train = Y_train[:50000]

X_train = X_train.reshape((-1, 15, 15, 1)).astype(np.float32)   
Y_train = Y_train.reshape((-1, 225)).astype(np.float32)       


x_data.close()
y_data.close()


reduce_lr = ReduceLROnPlateau(
    monitor='loss',      
    factor=0.5,        
    patience=3,           
    min_lr=1e-6,        
    verbose=1             
)


model = Sequential([
    Conv2D(64, kernel_size=(3, 3), activation='relu', padding='same', input_shape=(15, 15, 1)),
    Conv2D(64, kernel_size=(3, 3), activation='relu', padding='same'),
    Flatten(),
    Dense(256, activation='relu'),
    Dense(225, activation='softmax')  
])


model.compile(
    optimizer=Adam(learning_rate=0.0005),
    loss='categorical_crossentropy',
    metrics=['accuracy']
)


model.fit(X_train, Y_train, epochs=20, batch_size=64, callbacks=[reduce_lr])


model.save("C:/Users/Lisaq/Documents/gomoku AI/gomoku_cnn_model.keras")
