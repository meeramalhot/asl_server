# Code source and credit to https://github.com/computervisioneng/sign-language-detector-python/tree/master
# Physical Computing components (Raspberry Pi, camera and connection to webserver) by Isabella Teixeira, Meera Malhotra & Muhammad Ghani

import os                    # Import the os module to interact with the operating system
import pickle               # Import the pickle module for serializing and deserializing Python object structures
import mediapipe as mp      # Import the mediapipe library for media processing, commonly used for pose estimation and face/hand tracking
import cv2                  # Import the cv2 module (OpenCV) for handling image operations
# import matplotlib.pyplot as plt  # Import the matplotlib's pyplot to provide a MATLAB-like plotting framework

mp_hands = mp.solutions.hands  # Shorten the namespace for easier access to the hands module in mediapipe
mp_drawing = mp.solutions.drawing_utils  # Access drawing utilities for rendering annotations on images
mp_drawing_styles = mp.solutions.drawing_styles  # Access predefined drawing styles for annotations

hands = mp_hands.Hands(static_image_mode=True, min_detection_confidence=0.3)  # Create a hands detection object with specified parameters

DATA_DIR = './data'  # Define the path to the data directory

data = []  # Initialize an empty list to store processed data points
labels = []  # Initialize an empty list to store labels corresponding to the data

# Iterate through each directory in the DATA_DIR
for dir_ in os.listdir(DATA_DIR):
    dir_path = os.path.join(DATA_DIR, dir_)
    if os.path.isdir(dir_path):  # Check if it is a directory
        # Iterate through each image file in the current directory
        for img_path in os.listdir(dir_path):
            data_aux = []  # Temporary list to hold normalized coordinates of landmarks for one image
    
            x_ = []  # List to store x coordinates of landmarks
            y_ = []  # List to store y coordinates of landmarks
    
            img = cv2.imread(os.path.join(DATA_DIR, dir_, img_path))  # Read the image
            img_rgb = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)  # Convert the image from BGR to RGB
    
            results = hands.process(img_rgb)  # Process the image to find hands and their landmarks
            if results.multi_hand_landmarks:  # Check if any hands are detected
                for hand_landmarks in results.multi_hand_landmarks:  # Iterate through each detected hand
                    for i in range(len(hand_landmarks.landmark)):  # Iterate through each landmark point
                        x = hand_landmarks.landmark[i].x  # Extract the x coordinate
                        y = hand_landmarks.landmark[i].y  # Extract the y coordinate
    
                        x_.append(x)  # Append x to the list of x coordinates
                        y_.append(y)  # Append y to the list of y coordinates
    
                    for i in range(len(hand_landmarks.landmark)):  # Normalize coordinates relative to the smallest x and y
                        x = hand_landmarks.landmark[i].x
                        y = hand_landmarks.landmark[i].y
                        data_aux.append(x - min(x_))  # Normalize x and append to data_aux
                        data_aux.append(y - min(y_))  # Normalize y and append to data_aux
    
                data.append(data_aux)  # Append the processed coordinates to the main data list
                labels.append(dir_)  # Append the directory name as label for this data point

f = open('data.pickle', 'wb')  # Open a file in write-binary mode to store serialized data
pickle.dump({'data': data, 'labels': labels}, f)  # Serialize the data and labels dictionary and write it to the file
f.close()  # Close the file
