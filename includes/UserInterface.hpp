/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserInterface.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arurangi <arurangi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/27 19:39:54 by arurangi          #+#    #+#             */
/*   Updated: 2023/09/29 10:04:25 by arurangi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USERINTERFACE
    #define USERINTERFACE

    #include <iostream>

    #define CGRAY       "\033[90m"
    #define CRED      "\033[0;31m"
    #define CGREEN    "\033[0;32m"
    #define CBLUE    "\033[0;94m"
    #define CYELLOW   "\033[0;33m"
    #define CMAGENTA  "\033[0;35m"
    #define CRESET    "\033[0m"
    #define CBOLD     "\x1b[1m"

    // Simplify display mode states
    #define ON true
    #define OFF false

    // Shorten inclusion of function name and line number in error messages
    #define FN __func__
    #define LN __LINE__

    class UserInterface {
    
        private:
            bool _displayMode;

        public:

            // constructors
            UserInterface() : _displayMode(ON) {}
            UserInterface( UserInterface& ) {}
            UserInterface& operator= ( UserInterface& ) { return *this; }
            ~UserInterface() {}

            // member functions
            void error( const std::string& msg,  const std::string& fn = "", const int& ln = -1) const {
                std::cout << CRED << "✘ Error " << CRESET
                          << msg;
                
                if (fn.empty() && ln == -1)
                    return ;
                else if (!fn.empty() && ln == -1)
                    std::cout << CGRAY << " (" << fn << ")" << std::endl;
                else if (ln != -1 && fn.empty())
                    std::cout << CGRAY << " (" << ln << ")" << std::endl;
                else
                    std::cout << CGRAY << " (" << fn << ":" << ln << ")" << CRESET;
            }

            void success( const std::string& msg ) const {
                std::cout << CGREEN << "✔ " << CRESET << msg << std::endl;
            }
            void warning( const std::string& msg ) const {
                if (_displayMode == OFF) return ;
                std::cout << CYELLOW << "Warning: " << CRESET << msg << std::endl;
            }

    };

#endif