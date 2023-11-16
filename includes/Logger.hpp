#ifndef LOGGER_HPP
    #define LOGGER_HPP

    #include <iostream>
    #include <vector>
    #include <map>
    #include <cstdio>

    #include "Utils.hpp"

    using std::string;
    using std::vector;
    using std::map;


    typedef map< string, map<string, vector<string> > >          LocationMap;
    typedef vector< LocationMap > LocationsList;

    #define CBLUE    "\033[0;94m"
    #define CYELLOW   "\033[0;33m"
    #define CGREEN   "\033[0;32m"
    #define CMAGENTA  "\033[0;35m"
    #define CRED      "\033[0;31m"
    #define CB_YELLOW      "\033[0;43m"
    #define CBOLD   "\x1b[1m"

    #define BKBLUE  "\x1B[34m"

    #define CRESET    "\033[0m"

    class Log {
        private:
            //
        public:
            static void status(std::string msg);

            static void listening(std::string host, int port, std::string server_name);

            static void error(std::string msg);

            static void param(std::string title, std::string msg);

            static void simple(std::string msg, std::string color);

            static int out(std::string msg);

            static int warn(std::string msg);

            static int success(std::string msg);

            static int mark(std::string msg);

            static void ltree(LocationsList ll);

            template <typename T>
            static void highlight(T data) {
                std::cout << CYELLOW << "/////////////////////////////\n" CRESET;
                std::cout << CYELLOW << "$ " << CRESET << data << std::endl;
                std::cout << CYELLOW << "/////////////////////////////\n" CRESET;
            }

            template <typename T, typename K>
            static void pairs(T key, K pair) {
                std::cout << CBOLD
                          << key << ": " << pair
                          << CRESET << std::endl;
            }
    };

#endif