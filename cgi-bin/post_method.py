#!/usr/bin/env python3
import cgi
# Parse the form data
form = cgi.FieldStorage()

# Get the value of the 'name' field from the form
name = form.getvalue('name')
age = form.getvalue('age')
# Start the HTML response
print("<html>")
print("<head>")
print("<title>POST Example</title>")
print("</head>")
print("<body>")

# Check if the 'name' field is present in the form
if name:
    print("<h1>Hello, {}!</h1>".format(name))
else:
    print("<h1>Please provide a name.</h1>")

if age:
	print("<h1>You are {}!</h1>".format(age))
else:
	print("<h1>Please provide an age.</h1>")

# End the HTML response
print("</body>")
print("</html>")