#ifndef UTILS_HPP
    #define UTILS_HPP

    #include <sstream>
    #include <iostream>

    class Utils {
        public:
            static std::string to_str(int num) {
                std::stringstream ss;
                ss << num;
                std::string numstr = ss.str();
                return numstr;
            }
    };

#endif