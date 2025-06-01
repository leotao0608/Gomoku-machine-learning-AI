import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Conv2D, Dense, Flatten
from tensorflow.keras.optimizers import Adam
import numpy as np
import os
from tensorflow.keras.callbacks import EarlyStopping
from tensorflow.keras.callbacks import ReduceLROnPlateau
from tensorflow.keras.losses import CategoricalCrossentropy

from sklearn.utils import shuffle


os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'
os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'

x_data = np.load('C:/Users/Lisaq/Documents/gomoku AI/dataset-1.1/x_train.npz')
X_train = x_data["arr_0"]  

y_data = np.load('C:/Users/Lisaq/Documents/gomoku AI/dataset-1.1/y_train.npz')
Y_train = y_data["arr_0"] 
#print(X_train.shape)
#print(Y_train.shape)
#print(np.sum(Y_train[0])) 

X_train = X_train[400000:500000]
Y_train = Y_train[400000:500000]


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
model = tf.keras.models.load_model("C:/Users/Lisaq/Documents/gomoku AI/gomoku_cnn_model_6.keras")



model.compile(
    optimizer=Adam(learning_rate=0.0001),
    loss=CategoricalCrossentropy(label_smoothing=0.05),
    
    metrics=['accuracy']
)


model.fit(X_train, Y_train, epochs=15, batch_size=128, validation_split=0.1, callbacks=[reduce_lr, early_stop])


model.save("C:/Users/Lisaq/Documents/gomoku AI/gomoku_cnn_model_7.keras")
