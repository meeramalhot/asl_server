# Code source and credit to https://github.com/computervisioneng/sign-language-detector-python/tree/master
# Physical Computing components (Raspberry Pi, camera and connection to webserver) by Isabella Teixeira, Meera Malhotra & Muhammad Ghani

import pickle  # Import the pickle module for serializing and deserializing Python object structures
from sklearn.ensemble import RandomForestClassifier  # Import RandomForestClassifier from scikit-learn for classification tasks
from sklearn.model_selection import train_test_split  # Import train_test_split to easily divide data into training and testing sets
from sklearn.metrics import accuracy_score  # Import accuracy_score to compute the accuracy of the classifier
import numpy as np  # Import numpy for efficient numerical computations

data_dict = pickle.load(open('./data.pickle', 'rb'))  # Load the preprocessed data from a pickle file

data = np.asarray(data_dict['data'])  # Convert the list of data points to a numpy array for efficient manipulation
labels = np.asarray(data_dict['labels'])  # Convert the list of labels to a numpy array

# Split the dataset into training and testing sets, with 20% of the data used as the test set
x_train, x_test, y_train, y_test = train_test_split(data, labels, test_size=0.2, shuffle=True, stratify=labels)

model = RandomForestClassifier()  # Create an instance of a RandomForest classifier

model.fit(x_train, y_train)  # Train the model using the training data

y_predict = model.predict(x_test)  # Predict the labels for the testing set

score = accuracy_score(y_predict, y_test)  # Calculate the accuracy score of the predictions

print('{}% of samples were classified correctly!'.format(score * 100))  # Print the accuracy in percentage form

f = open('model.p', 'wb')  # Open a file in write-binary mode to store the trained model
pickle.dump({'model': model}, f)  # Serialize the model dictionary and write it to the file
f.close()  # Close the file
