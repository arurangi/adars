#include "Utils.hpp"

std::string
utils::to_str(int num) {
    std::stringstream ss;
    ss << num;
    std::string numstr = ss.str();
    return numstr;
}

bool utils::startswith(const std::string& str, const std::string& keyword)
{
    if (str.size() == 0 || keyword.size() == 0)
        return false;
    size_t found = str.find(keyword);
    if (found == std::string::npos )
        return false;
    if (found != 0)
        return false;
    return true;
}

bool utils::endswith(const std::string& str, const std::string& keyword)
{
    if (str.size() == 0 || keyword.size() == 0)
        return false;
    size_t found = str.find(keyword);
    if (found == std::string::npos )
        return false;
    if (found + keyword.size() != str.size())
        return false;
    return true;

}