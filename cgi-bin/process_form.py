#!/usr/bin/env python3

import cgi

print("<html>")
print("<head>")
print("    <title>Form Submission Result</title>")
print("</head>")
print("<body>")

# Retrieve form data from the query string
form = cgi.FieldStorage()

# Get the values submitted in the form
name = form.getvalue('name')
email = form.getvalue('email')

# Display the submitted data
print("<h2>Form Submission Result:</h2>")
print(f"<p><strong>Name:</strong> {name}</p>")
print(f"<p><strong>Email:</strong> {email}</p>")

print("</body>")
print("</html>")