#include "../includes/Checks.hpp"

void check::failure(int status) {
    if (status <= 0)
        throw http::Abort();
}

bool check::failed(int status) {
    if (status < 0)
        Log::error("select() call failed");
    else if (status == 0)
        Log::status("Server timed-out");
    else
        return false;
    return true;
}