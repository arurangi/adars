#ifndef LOGGER_HPP
    #define LOGGER_HPP

    #include <iostream>

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
            static void status(std::string msg) {
            std::cout << std::endl << CYELLOW CBOLD
                      << "◌" 
                      << " " << msg << " " << CRESET << std::endl;
            }
            static void error(std::string msg) {
            std::cout << std::endl << CRED CBOLD
                      << "✗ Error: " << CRESET 
                    << msg << " " << std::endl;

            }

    };

#endif