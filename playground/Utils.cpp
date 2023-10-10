#include "Utils.hpp"

std::string
Utils::to_str(int num) {
    std::stringstream ss;
    ss << num;
    std::string numstr = ss.str();
    return numstr;
}