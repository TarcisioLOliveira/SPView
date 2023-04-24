/*
 *   Copyright (C) 2023 Tarcísio Ladeia de Oliveira.
 *
 *   This file is part of SPView
 *
 *   SPView is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   SPView is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with SPView.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>
#include <utility>
#include "utils.hpp"
#include <vector>
#include <signal.h>

namespace spview::logger{

    enum AssertType{
        SILENT,
        WARNING,
        ERROR
    };

    /**
     * Logs a message to stderror based on an assertion.
     *
     * @param expr Truth to assert.
     * @param t Message type.
     * @param message The message (with formatting).
     * @param args Arguments for message formatting.
     *
     * @returns expr itself.
     */
    template<typename ... Args>
    static bool log_assert(bool expr, AssertType t, std::string message, Args&& ... args){
        if(!expr && t != SILENT){
            if(t == WARNING){
                message = "WARNING: "+message;
            } else if(t == ERROR){
                message = "ERROR: "+message;
            }
            //fprintf(stderr, message.c_str(), args ...);
            std::cout << utils::format(message, std::forward<Args>(args) ...) << std::endl;
            if(t == ERROR){
                // exit(EXIT_FAILURE);
                //
                // Exit and get stacktrace on gdb. Dirty but works.
                kill(getpid(),SIGSEGV);
            }
        }
        return expr;
    }

    template<typename T>
    static void quick_log(T t){
        std::cout << "SPView: " << t << std::endl;
    }

    template<typename T, typename ... Args>
    static void quick_log(T t, Args&& ... args){
        std::cout << "SPView: " << t << " ";
        quick_log(args...);
    }

    template<typename T>
    static void quick_log(const std::vector<T>& v){
        std::cout << "SPView: ";
        for(const T& e:v){
            std::cout << e << " ";
        }
        std::cout << std::endl;
    }
}

#endif
