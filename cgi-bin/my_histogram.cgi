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

def plot_histogram(data, filename='file3_type_histogram.jpeg'):
    # path to save the image 
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_path = os.path.join(script_dir, filename)

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
    print(f"""
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
                margin: auto;
            }}
            form {{
                text-align: center;
                margin-top: 20px;
            }}
        </style>
    </head>
    <body>
        <h1>CS410 Webserver</h1>
        <img src='{os.path.basename(image_path)}' alt='File Type Histogram'>
        <form action='' method='get'>
            <input type='text' name='directory' placeholder='Enter directory path'>
            <input type='submit' value='Generate Histogram'>
        </form>
    </body>
    </html>
    """)


# Setup CGI and read parameters
form = cgi.FieldStorage()
directory = form.getvalue('directory')

if directory:
    if os.path.exists(directory):
        file_type_counts = count_file_types(directory)
        image_path = plot_histogram(file_type_counts)
        print_html(image_path)
    else:
        print("<html><body><h1>Error: Directory does not exist.</h1></body></html>")
else:
    print(f"""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>File Type Histogram</title>
    <link rel="stylesheet" href="style-hist.css">
</head>
<body>
    <header>
        <h1>File Type Histogram Generator</h1>
        <nav>
            <ul>
                <li><a href="../index.html">ASL</a></li>
                <li><a href="../about.html">About</a></li>
                <li><a href="/src/index.html">Gif</a></li>
                <li><a href="/cgi-bin/directory_listing.cgi">CGI 'sh'</a></li>
            </ul>
        </nav>
    </header>
    <main>
        <form action="" method="get">
            <input type="text" name="directory" placeholder="Enter directory path" style="width: 70%;">
            <input type="submit" value="Generate Histogram">
        </form>
    </main>
    <footer>
         <p>&copy; Project by Meera Malhotra, Isabella Teixeira & Muhammad Ahmad Ghani.</p>
    </footer>
</body>
</html>


    """)
