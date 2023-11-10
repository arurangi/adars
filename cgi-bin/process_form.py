#!/usr/bin/env python3
import cgi

# Get form data from POST request
form = cgi.FieldStorage()

# Get values submitted through the form
name = form.getvalue("name")
email = form.getvalue("email")

# HTML response
print("<html>")
print("<head>")
print("<title>Form Submission Result</title>")
print("</head>")
print("<body>")
print("<h1>Form Submission Result</h1>")
print(f"<p>Name: {name}</p>")
print("<p>Email: %s</p>" % (email))
print("</body>")
print("</html>")