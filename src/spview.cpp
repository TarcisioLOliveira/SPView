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

#include <iostream>
#include <boost/asio.hpp>
#include "spview.hpp"

namespace spview{

namespace bp = boost::process;

Server::Server():buffer(4096, 0){

    boost::asio::io_service ios;

    bp::async_pipe ap(ios);

    this->proc = bp::child("SPView", "",
                           bp::std_out > ap,
                           ios);

    boost::asio::async_read(ap, boost::asio::buffer(this->buffer),
                    [this](const boost::system::error_code &ec, std::size_t size){
                        std::cout << this->buffer;
                    });


    ios.run();

    // while (pipe_stream && std::getline(pipe_stream, line) && !line.empty())
    //     std::cout << line << std::endl;
}

Server::~Server(){
    proc.terminate();
}


}
