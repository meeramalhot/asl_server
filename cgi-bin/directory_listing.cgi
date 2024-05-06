#!/bin/sh

# HTTP headers
echo "Content-Type: text/html"
echo ""

# Start of HTML document

cat <<EOF
<!DOCTYPE html>
<html>
<head>
    <title>Directory Listing</title>
    <link rel="stylesheet" type="text/css" href="../styles-dir.css">
</head>
<body>
    <header>
        <h1>Directory Listing</h1>
        <nav>
            <ul>
                <li><a href="../index.html">Back</a></li> 
            </ul>
        </nav>
    </header>
    
    <main>
        <ul>
EOF

# Command to list directory contents
ls | while read line; do
    echo "            <li>$line</li>"
done

# End of HTML document
cat <<EOF
        </ul>
    </main>
    <footer>
         <p>&copy; Project by Meera Malhotra, Isabella Teixeira & Muhammad Ahmad Ghani.</p>
    </footer>
</body>
</html>
EOF