# Code source and credit to https://github.com/computervisioneng/sign-language-detector-python/tree/master
# Physical Computing components (Raspberry Pi, camera and connection to webserver) by Isabella Teixeira, Meera Malhotra & Muhammad Ghani

import os # Import the os module to interact with the operating system
import cv2 # Import the cv2 module (OpenCV) for handling image and video operations

DATA_DIR = './data'  # Define the path to the directory where data will be stored
if not os.path.exists(DATA_DIR):  # Check if the directory does not exist
    os.makedirs(DATA_DIR)         # Create the directory if it does not exist

number_of_classes = 36  # Set the number of classes for the dataset
dataset_size = 100      # Set the number of images to collect for each class

cap = cv2.VideoCapture(0)  # Start video capture from the default camera (usually the webcam)

# Loop over each class
for j in range(number_of_classes):
    if not os.path.exists(os.path.join(DATA_DIR, str(j))):  # Check if a subdirectory for the class doesn't exist
        os.makedirs(os.path.join(DATA_DIR, str(j)))  # Create the subdirectory for the class

    print('Collecting data for class {}'.format(j))  # Print a message indicating which class data is being collected for

    done = False  # Initialize a variable to manage the loop
    while True:  # Infinite loop to capture the initial frame where the user can start capturing by pressing 'Y'
        ret, frame = cap.read()  # Read a frame from the video capture
        # Display a message on the frame
        cv2.putText(frame, 'Welcome...ready? Press "Y"!', (100, 50), cv2.FONT_HERSHEY_DUPLEX, 1.3, (0, 0, 0), 3,
                    cv2.LINE_AA)
        cv2.imshow('frame', frame)  # Display the frame in a window
        if cv2.waitKey(25) == ord('y'):  # Wait for the 'y' key to be pressed
            break  # Exit the loop if 'y' is pressed

    counter = 0  # Initialize a counter to track the number of saved images
    while counter < dataset_size:  # Loop to capture and save the specified number of images per class
        ret, frame = cap.read()  # Read a frame from the video capture
        cv2.imshow('frame', frame)  # Show the frame in a window
        cv2.waitKey(25)  # Wait for a short delay
        # Save the current frame as a JPEG file in the respective class directory
        cv2.imwrite(os.path.join(DATA_DIR, str(j), '{}.jpg'.format(counter)), frame)

        counter += 1  # Increment the counter

cap.release()  # Release the video capture object
cv2.destroyAllWindows()  # Close all OpenCV windows