# Code source and credit to https://github.com/computervisioneng/sign-language-detector-python/tree/master
# Physical Computing components (Raspberry Pi, camera and connection to webserver) by Isabella Teixeira, Meera Malhotra & Muhammad Ghani

import pickle  # Import the pickle module for loading serialized Python objects
import cv2  # Import OpenCV for handling image and video operations
import mediapipe as mp  # Import the MediaPipe library for easy access to pre-trained models for hand tracking
import numpy as np  # Import numpy for numerical operations
import requests
import sys

model_dict = pickle.load(open('./model.p', 'rb'))  # Load the model from a pickle file
model = model_dict['model']  # Retrieve the model from the dictionary

cap = cv2.VideoCapture(0)  # Start video capture on the device at index 0

mp_hands = mp.solutions.hands  # Access the MediaPipe hands solution
# mp_drawing = mp.solutions.drawing_utils  # Utility for drawing landmarks
# mp_drawing_styles = mp.solutions.drawing_styles  # Styles for drawing landmarks

hands = mp_hands.Hands(static_image_mode=True, min_detection_confidence=0.3)  # Initialize the hand tracking model

labels_dict = {
    0: 'A', 1: 'B', 2: 'C', 3: 'D', 4: 'E', 5: 'F', 6: 'G', 7: 'H', 8: 'I', 9: 'J',
    10: 'K', 11: 'L', 12: 'M', 13: 'N', 14: 'O', 15: 'P', 16: 'Q', 17: 'R', 18: 'S', 19: 'T',
    20: 'U', 21: 'V', 22: 'W', 23: 'X', 24: 'Y', 25: 'Z', 26: '0', 27: '1', 28: '2', 29: '3',
    30: '4', 31: '5', 32: '6', 33: '7', 34: '8', 35: '9'
} # Dictionary to map model output labels to characters

while True:  # Begin an infinite loop for real-time processing

    ret, frame = cap.read()  # Read a frame from the video capture
    if not ret:
        break

    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)  # Convert frame to RGB (MediaPipe requirement)
    results = hands.process(frame_rgb)  # Process the frame through the hand tracking model

    data_aux = []  # List to hold normalized landmark data
    x_ = []  # List to store x coordinates of landmarks
    y_ = []  # List to store y coordinates of landmarks
    
    # H, W, _ = frame.shape  # Get the height and width of the frame
    
    if results.multi_hand_landmarks:  # Check if any hands are detected
        for hand_landmarks in results.multi_hand_landmarks:
            # mp_drawing.draw_landmarks(
                # frame,  # Image to draw on
                # hand_landmarks,  # Detected hand landmarks
                # mp_hands.HAND_CONNECTIONS,  # Connections between landmarks
                # mp_drawing_styles.get_default_hand_landmarks_style(),
                # mp_drawing_styles.get_default_hand_connections_style())

        # for hand_landmarks in results.multi_hand_landmarks:
            for i in range(len(hand_landmarks.landmark)):
                x = hand_landmarks.landmark[i].x  # Get the x coordinate
                y = hand_landmarks.landmark[i].y  # Get the y coordinate
                x_.append(x)  # Append x to list
                y_.append(y)  # Append y to list

            for i in range(len(hand_landmarks.landmark)):
                x = hand_landmarks.landmark[i].x  # Get the x coordinate
                y = hand_landmarks.landmark[i].y  # Get the y coordinate
                data_aux.append(x - min(x_))  # Normalize x and append to list
                data_aux.append(y - min(y_))  # Normalize y and append to list

        # x1 = int(min(x_) * W) - 10  # Calculate the left boundary of bounding box
        # y1 = int(min(y_) * H) - 10  # Calculate the upper boundary of bounding box

        # x2 = int(max(x_) * W) + 10  # Calculate the right boundary of bounding box
        # y2 = int(max(y_) * H) + 10  # Calculate the lower boundary of bounding box

            prediction = model.predict([np.asarray(data_aux)])  # Make a prediction based on normalized landmarks

            predicted_character = labels_dict[int(prediction[0])]  # Map prediction to corresponding character

            #TAKE IN THE ADDRESS OF THE SERVER AS A COMMAND LINE ARGUMENT
            #LOCAL HOST BECOMES COMMAND LINE ARG
            x = requests.post(f"http://{sys.argv[1]}/submit_letter", data={"letter":predicted_character})
            
        # cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 0, 0), 4)  # Draw a rectangle around the hand
        # cv2.putText(frame, predicted_character, (x1, y1 - 10), cv2.FONT_HERSHEY_SIMPLEX, 1.3, (0, 0, 0), 3,
        #          cv2.LINE_AA)  # Label the rectangle with the predicted character

    # cv2.imshow('frame', frame)  # Display the frame
    # cv2.waitKey(1)  # Short pause, keep frame visible

cap.release()  # Release the video capture device
cv2.destroyAllWindows()  # Close all OpenCV windows
