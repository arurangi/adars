#ifndef UTILS_HPP
    #define UTILS_HPP

    #include <sstream>
    #include <string>
    #include <deque>
    #include <dirent.h>
    #include "Logger.hpp"

    using std::string;

    #define CBLUE    "\033[0;94m"
    #define CYELLOW   "\033[0;33m"
    #define CGREEN   "\033[0;32m"
    #define CMAGENTA  "\033[0;35m"
    #define CBOLD       "\x1b[1m"
    #define CRED      "\033[0;31m"
    #define CRESET    "\033[0m"

    namespace ft {
        string to_str(int num);
        bool startswith(const string& str, const string& keyword);
        bool endswith(const string& str, const string& keyword);

        string get_dir(string file_path);
        std::deque<string> list_files_in(string directory);
    };

#endif