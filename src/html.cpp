#include "../includes/Html.hpp"

std::string
html::filter(std::string str) {
    std::string s;
    
    s += "<form action=\"filter\" method=\"get\">\n";
    s += "<input type=\"submit\" name=\"";
    s += str;
    s += "\" value=\"";
    s += str;
    s += "\" /></form>";

    return s;
}