#!/usr/bin/python3
#the above is different for all machines might have to change it
import os
import cgitb; cgitb.enable()  # Enable CGI error reporting

print("Content-Type: text/html")  # HTML is following
print()                           # blank line, end of headers

if "letter" in os.environ:
    sign = os.environ["letter"]
    print(f"<h1>Received ASL sign: {sign}</h1>")
    print("<p>Your ASL sign has been processed.</p>")
else:
    print("<h1>Error: ASL sign not provided!</h1>")