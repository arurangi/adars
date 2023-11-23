#include "../includes/Checks.hpp"

void check::failure(int status) {
    if (status <= 0)
        throw http::Abort();
}