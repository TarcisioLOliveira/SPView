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
#include <filesystem>
#include "spview.hpp"
#include "defs.hpp"

namespace spview{

namespace bp = boost::process;

Server::Server(std::string name):
    pipe_name(name),
    buffer(4096, 0),
    client_output(this->ios),
    data_queue(this->ios, "/tmp/"+name),
    worker(boost::asio::make_work_guard(this->ios)){


void Server::start(){
    this->proc = bp::child("SPViewClient", this->pipe_name,
                           bp::std_out > client_output,
                           this->ios);

    boost::asio::async_read(client_output, boost::asio::buffer(this->buffer),
                    [this](const boost::system::error_code &ec, std::size_t size){
                        (void)ec;
                        (void)size;
                        std::cout << this->buffer;
                    });


    this->thread = boost::thread(boost::bind(
                        &Server::loop,
                        this)
                    );
}

Server::~Server(){
    this->client_output.cancel();
    this->client_output.close();
    this->proc.terminate();
}

void Server::loop(){
    this->ios.run();
}

void Server::update_data(size_t view_id, std::vector<size_t> tags, std::vector<double> data){
    defs::UpdateViewData d{view_id, tags.size(), data.size()};

    this->data_queue.push(d.serialize());
    this->data_queue.push(tags);
    this->data_queue.push(data);

    this->data_queue.send_all();
}

}
