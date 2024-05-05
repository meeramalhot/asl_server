#!/Users/muhammadahmadghani/anaconda3/bin/python3

import os
import sys
import cgi
import cgitb; cgitb.enable()  # Enable CGI debugging
import matplotlib.pyplot as plt
import stat

print("Content-Type: text/html\n")  # HTML header

def count_file_types(directory):
    type_counts = {
        'regular': 0,
        'directory': 0,
        'link': 0,
        'fifo': 0,
        'socket': 0,
        'block': 0,
        'character': 0
    }
    for root, dirs, files in os.walk(directory, followlinks=False):
        dirs[:] = [d for d in dirs if not d.startswith('.')]
        files = [f for f in files if not f.startswith('.')]
        for dir in dirs:
            dir_path = os.path.join(root, dir)
            type_counts['directory'] += 1
        for file in files:
            path = os.path.join(root, file)
            try:
                stat_info = os.lstat(path)
            except OSError:
                continue
            if stat.S_ISLNK(stat_info.st_mode):
                type_counts['link'] += 1
            elif stat.S_ISREG(stat_info.st_mode):
                type_counts['regular'] += 1
            elif stat.S_ISFIFO(stat_info.st_mode):
                type_counts['fifo'] += 1
            elif stat.S_ISSOCK(stat_info.st_mode):
                type_counts['socket'] += 1
            elif stat.S_ISBLK(stat_info.st_mode):
                type_counts['block'] += 1
            elif stat.S_ISCHR(stat_info.st_mode):
                type_counts['character'] += 1
    return type_counts

def plot_histogram(data, output_path='file_type_histogram.jpeg'):
    types = list(data.keys())
    counts = list(data.values())
    plt.figure(figsize=(10, 6))
    plt.bar(types, counts, color='blue')
    plt.xlabel('File Types')
    plt.ylabel('Frequency')
    plt.title('Histogram of File Types')
    plt.savefig(output_path)
    return output_path

def print_html(image_path):
    print("""
    <html>
    <head>
        <title>CS410 Webserver</title>
        <style>
            body {{ 
                background-color: white; 
                font-family: Arial, sans-serif; 
            }}
            h1 {{
                color: red;
                font-size: 16pt;
                text-align: center;
            }}
            img {{
                display: block;
                margin: 20px auto;
            }}
        </style>
    </head>
    <body>
        <h1>CS410 Webserver</h1>
        <img src='{}' alt='File Type Histogram'>
    </body>
    </html>
    """.format(image_path))

# Setup CGI and read parameters
form = cgi.FieldStorage()
directory = form.getvalue('directory')

if directory:
    file_type_counts = count_file_types(directory)
    image_path = plot_histogram(file_type_counts)
    print_html(image_path)
else:
    print("<html><body><h1>Error: No directory provided or directory does not exist.</h1></body></html>")
