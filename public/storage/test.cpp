

string handle_cgi(Request &req)
{
    int stdin_pipe[2];
    int stdout_pipe[2];

    if (pipe(stdin_pipe) < 0 || pipe(stdout_pipe) < 0)
    {
        Log::error("in pipes");
        return ;
    }
    pid_t child_pid = fork();
    if (child_pid < 0)
        Log::error("Fork failed");
    else if (child_pid == 0)
    {
        dup2(st)
    }
}
-----------------------------954040167266886006878119606--
