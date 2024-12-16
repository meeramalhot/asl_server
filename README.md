# asl_server

**How to run a sample web page at a time:** <br />
make <br />
./webserv 8080 <br />
http://localhost:8080/src/index.html

**How to run the python scripts:** 
- First run the server
    - make
    - ./webserv 8080 &
    - http://localhost:8080/index.html
    
- Now you can run the python scripts interacting with the server
- Run the 'python collect_images.py' and record however many ASL hand signs you would like. Terminate this program when you are done.
- After this, run the 'python dataset_builder.py' and allow it to create the dataset from the collected images. Once this has complete, move onto the next step.
- Next, run the 'python train_classifier.py' file and allow the ML algorithm to classify everything

- Finally, on completion of all the previous scripts, you can run this 'on-going, active' script where the magic happens. You will position the camera aimed at your hand, run the 'python classifier.py localhost:8000' script and start practicing your ASL in front of the camera.

- This will output the associated ASCII text to the webserver.
- If you want to test with curl, use: curl -X POST -d "letter=c" http://localhost:8000/submit_letter
  
- **NOTE**: use **ONE** hand at a time. We cannot handle more than that at the moment. :)
