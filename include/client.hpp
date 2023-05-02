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

#ifndef SPVIEW_CLIENT_HPP
#define SPVIEW_CLIENT_HPP

#include <string>
#include <boost/thread.hpp>
#include <boost/process.hpp>
#include <boost/asio.hpp>
#include "gmsh.hpp"

namespace spview{

class Client{
    public:
    Client(std::string pipe_name);
    ~Client();

    void get_messages();

    private:
    Gmsh* viewer;
    std::string pipe_name;
    boost::asio::io_service ios;
    boost::asio::io_service::strand strand;
    int pipe_file;
    boost::asio::posix::stream_descriptor pipe;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> worker;
    boost::thread thread;
    size_t buffer[defs::MESSAGE_SIZE];

    void get_next_message();
    void process_message();
    void loop();
};

}

#endif
