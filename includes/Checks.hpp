#ifndef CHECKS_HPP
    #define CHECKS_HPP

    #include "Http.hpp"

    namespace check {
        void failure(int status);
        bool failed(int status);
    }

#endif