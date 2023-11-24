#!/usr/bin/env python3

print("<html>")
print("<head>")
print("<title>CGI Script with Infinite Loop</title>")
print("</head>")
print("<body>")
print("<h1>CGI Script with Infinite Loop</h1>")
print("<p>This CGI script contains an intentional infinite loop.</p>")

while True:
	print("<p>This loop will run forever!</p>")

# print("</body>")
# print("</html>")