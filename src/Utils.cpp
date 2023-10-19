#include "Utils.hpp"

std::string
utils::to_str(int num) {
    std::stringstream ss;
    ss << num;
    std::string numstr = ss.str();
    return numstr;
}