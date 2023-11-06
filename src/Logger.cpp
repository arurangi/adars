#include "../includes/Logger.hpp"

void
Log::status(std::string msg) {
std::cout << std::endl << CYELLOW CBOLD
            << "◌" 
            << " " << msg << " " << CRESET << std::endl;
}

void
Log::listening(std::string host, int port, std::string server_name) {
std::cout << std::endl << CYELLOW CBOLD
            << "◌ " << CRESET
            << "Listening on " << CYELLOW << host << ":" << ft::to_str(port) << CRESET
            << " (or " << CYELLOW << server_name << ":" << ft::to_str(port) << CRESET << ")\n"
            << CRESET;
}

void
Log::error(std::string msg) {
std::cerr << std::endl << CRED CBOLD
            << "✗ error: " << CRESET 
        << msg << " " << std::endl;

}

void
Log::param(std::string title, std::string msg) {
std::cout << std::endl << CMAGENTA CBOLD
            << ">>> " << title << CRESET 
        << ": " << msg << std::endl;

}

void
Log::simple(std::string msg, std::string color) {
std::cout << std::endl << color << CBOLD
            << msg << CRESET 
            << std::endl;

}

int 
Log::out(std::string msg) {
    std::cerr << CRED CBOLD
                << "✗ error: " << msg << CRESET << std::endl << ": ";
    perror("");
    return -1;
}

int 
Log::warn(std::string msg) {
    std::cerr << std::endl << CYELLOW CBOLD
                << "⚠️ warning: " << CRESET
                << msg << std::endl;
    return -1;
}

int 
Log::success(std::string msg) {
    std::cout << std::endl << CGREEN CBOLD
                << "✔ " << CRESET
                << msg << std::endl;
    return -1;
}

int 
Log::mark(std::string msg) {
    std::cout << CMAGENTA CBOLD
                << "# " << CRESET
                << msg << std::endl;
    return -1;
}

void
Log::ltree(LocationsList ll) {
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