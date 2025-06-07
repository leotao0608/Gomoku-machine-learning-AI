#uses MCTS AI self play data to train
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D, Dense, Flatten
from tensorflow.keras.optimizers import Adam
import numpy as np
import os
from tensorflow.keras.callbacks import ReduceLROnPlateau
from tensorflow.keras.callbacks import EarlyStopping
from tensorflow.keras.losses import CategoricalCrossentropy
from sklearn.utils import shuffle
import matplotlib.pyplot as plt


os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'
os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'

x_data = np.load("C:/Users/Lisaq/Documents/gomoku AI/self play code/x_train.npz")
X_train = x_data["arr_0"]  

y_data = np.load("C:/Users/Lisaq/Documents/gomoku AI/self play code/y_train.npz")
Y_train = y_data["arr_0"] 

x_data.close()
y_data.close()

#print(X_train.shape)
#print(Y_train.shape)
#print(X_train[11])
#print(Y_train[10])


X_train, Y_train = shuffle(X_train, Y_train, random_state=42)
X_train = X_train.reshape((-1, 15, 15, 1)).astype(np.float32)  
Y_train = Y_train.reshape((-1, 225)).astype(np.float32)    

x_data.close()
y_data.close()

reduce_lr = ReduceLROnPlateau(
    monitor='val_loss',    
    factor=0.5,         
    patience=3,          
    min_lr=1e-6,          
    verbose=1            
)

early_stop = EarlyStopping(
    monitor='val_loss',
    patience=5,      
    restore_best_weights=True,
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
    optimizer=Adam(learning_rate=0.001),
    loss=CategoricalCrossentropy(label_smoothing=0.0005),
    metrics=['accuracy']
)

history = model.fit(
    X_train,
    Y_train, 
    epochs=15, 
    batch_size=64, 
    validation_split=0.1, 
    callbacks=[reduce_lr, early_stop]
)
model.save("C:/Users/Lisaq/Documents/gomoku AI/self play code/gomoku_cnn_model_0.keras")


plt.plot(history.history['loss'], label='Train Loss')
plt.plot(history.history['val_loss'], label='Val Loss')
plt.xlabel('Epoch')
plt.ylabel('Loss')
plt.title('Training and Validation Loss')
plt.legend()
plt.grid(True)
plt.show()
