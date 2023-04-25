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

#ifndef SPVIEW_HPP
#define SPVIEW_HPP

#include <boost/process.hpp>
#include <boost/asio.hpp>
#include "data_queue.hpp"

namespace spview{

class Server{
    public:
    Server(std::string name);
    ~Server();

    void update_data(size_t view_id, std::vector<size_t> tags, std::vector<double> data);

    private:
    template<typename T>
    void send_data(const std::vector<T>& data) const;

    boost::process::child proc;
    std::string pipe_name;
    std::string buffer;
    boost::asio::io_service ios;
    boost::process::async_pipe client_output;
    DataQueue data_queue;
};

}

#endif
