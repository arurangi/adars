#ifndef HTML_HPP
    #define HTML_HPP

    #include <string>

    namespace html {
        std::string filter(std::string str) {
            std::string s;
            
            s += "<form action=\"filter\" method=\"get\">";
            s += "<input type=\"submit\" name=\"";
            s += str;
            s += "\" value=\"";
            s += str;
            s += "\" /></form>";

            return s;
        }
    }

#endif