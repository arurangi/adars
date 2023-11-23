# Mandatory
read & write in CGI: add checking for -1 and 0
recv & send in handle_request: check for 0 and -1

# Configuration
Setup multiple servers with different hostnames (use something like: curl --resolve example.com:80:127.0.0.1 http://example.com/)
- remove default error page and must still display some error message (40x or 50x)
- error 404 when change root_dir of location

# Basic checks
- UNKNOWN requests should not result in a crash.
- For every test you should receive the appropriate status code.
- Upload some file to the server and get it back.

# CGI
- You need to test with files containing errors to see if the error handling works properly. You can use a script containing an infinite loop or an error; you are free to do whatever tests you want within the limits of acceptability that remain at your discretion. The group being evaluated should help you with this.
- free in CGI

# Check with browser
- Try a redirected URL

# Siege & stress test
- Check if there is no hanging connection