#include "../includes/Http.hpp"

string
http::handle_cgi(Request &req)
{
    int stdin_pipe[2];
    int stdout_pipe[2];
    string tmp = "." + req._uri;
    string final;

    const char *cmd = tmp.c_str();

    if (pipe(stdin_pipe) < 0 || pipe(stdout_pipe) < 0)
    {
        Log::error("in pipes");
        return NULL;
    }
    pid_t child_pid = fork();
    if (child_pid < 0)
        Log::error("Fork failed");
    else if (child_pid == 0)
    {
        dup2(stdin_pipe[0], 0);
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);

        dup2(stdout_pipe[1], 1);
        close(stdout_pipe[0]);
        close(stdout_pipe[1]);

        execve(cmd, NULL, NULL);
    } else {
        close(stdin_pipe[0]);
        close(stdout_pipe[1]);
        close(stdin_pipe[1]);

        char buffer[1024];
        ssize_t bytes;

        while ((bytes = read(stdout_pipe[0], buffer, sizeof(buffer))) > 0)
        {
            write(STDOUT_FILENO, buffer, bytes);
        }
        close(stdout_pipe[0]);
        final = buffer;
    }
    return final;
}