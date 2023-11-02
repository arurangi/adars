#ifndef LOGGER_HPP
    #define LOGGER_HPP

    #include <iostream>
    #include <vector>
    #include <map>

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
            static void status(std::string msg) {
            std::cout << std::endl << CYELLOW CBOLD
                      << "◌" 
                      << " " << msg << " " << CRESET << std::endl;
            }

            static void error(std::string msg) {
            std::cerr << std::endl << CRED CBOLD
                      << "✗ error: " << CRESET 
                    << msg << " " << std::endl;

            }

            static void param(std::string title, std::string msg) {
            std::cout << std::endl << CMAGENTA CBOLD
                      << ">>> " << title << CRESET 
                    << ": " << msg << std::endl;

            }

            static void simple(std::string msg, std::string color) {
            std::cout << std::endl << color << CBOLD
                      << msg << CRESET 
                      << std::endl;

            }

            static int out(std::string msg) {
                std::cerr << CRED CBOLD
                          << "✗ error: " << msg << CRESET;
                perror("");
                return -1;
            }

            static int warn(std::string msg) {
                std::cerr << std::endl << CYELLOW CBOLD
                          << "⚠️ warning: " << CRESET
                          << msg << std::endl;
                return -1;
            }

            static int success(std::string msg) {
                std::cout << std::endl << CGREEN CBOLD
                          << "✔ " << CRESET
                          << msg << std::endl;
                return -1;
            }

            static int mark(std::string msg) {
                std::cout << std::endl << CMAGENTA CBOLD
                          << "# " << CRESET
                          << msg << std::endl;
                return -1;
            }

            static void ltree(LocationsList& ll) {
                LocationsList::iterator ll_it = ll.begin();
                while (ll_it != ll.end()) {
                    LocationMap::iterator lm_it = (*ll_it).begin() ;
                    while (lm_it != (*ll_it).end()) {
                        Log::mark((*lm_it).first);
                        lm_it++;
                    }
                    ll_it++;
                }
            }
    };

#endif