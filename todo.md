# Mandatory
recv & send in handle_request: check for 0 and -1

# Configuration
Setup multiple servers with different hostnames (use something like: curl --resolve example.com:80:127.0.0.1 http://example.com/)
- error 404 when change root_dir of location

# Basic checks
- UNKNOWN requests should not result in a crash.
- For every test you should receive the appropriate status code.
- Upload some file to the server and get it back.

# Siege & stress test
- Check if there is no hanging connection

# errors 

- in parse_header