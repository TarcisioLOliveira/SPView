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
#include <boost/thread.hpp>
#include "data_queue.hpp"
#include "defs.hpp"

namespace spview{

class Server{
    public:
    Server(std::string name);
    ~Server();

    void start();
    void init_client(defs::InitData data, const std::vector<double>& points, const std::vector<size_t>& elem_nodes);
    void add_view(defs::ViewType view_type, defs::DataType data_type, const std::string& name);
    void update_data(size_t view_id, const std::vector<size_t>& tags, const std::vector<double>& data);
    void remove_view(size_t view_id);
    void close_client();

    inline bool is_running(){
        return this->proc.running();
    }

    inline void wait(){
        this->proc.wait();
    }

    private:
    boost::process::child proc;
    std::string pipe_name;
    boost::asio::streambuf buffer;
    boost::asio::io_service ios;
    boost::process::async_pipe client_output;
    DataQueue data_queue;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> worker;
    boost::thread thread;

    void loop();
    void stdout_read();
};

}

#endif
