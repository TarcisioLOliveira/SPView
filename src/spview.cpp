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

#include <boost/asio/read_until.hpp>
#include <iostream>
#include <filesystem>
#include "logger.hpp"
#include "spview.hpp"
#include "defs.hpp"

namespace spview{

namespace bp = boost::process;

Server::Server(std::string name):
    pipe_name(name),
    buffer(4096),
    client_output(this->ios),
    data_queue(this->ios, "/tmp/"+name),
    worker(boost::asio::make_work_guard(this->ios)){
}


void Server::start(){
    this->proc = bp::child("SPViewClient", this->pipe_name,
                           bp::std_out > client_output,
                           this->ios);

    this->stdout_read();

    this->thread = boost::thread(boost::bind(
                        &Server::loop,
                        this)
                    );
}

void Server::loop(){
    this->ios.run();
}

void Server::stdout_read(){
    boost::asio::async_read_until(client_output, this->buffer, '\n',
                    [this](const boost::system::error_code &ec, std::size_t size){
                        (void)ec;
                        (void)size;
                        std::istream ss(&this->buffer);
                        std::string line;
                        while(!ss.eof()){
                            std::getline(ss, line);
                            if(line.size() > 0)
                                std::cout << "SPView: " << line << std::endl;
                        }
                        if(this->is_running()){
                            this->stdout_read();
                        }
                    });
}

void Server::update_data(size_t view_id, std::vector<size_t> tags, std::vector<double> data){
    defs::UpdateViewData d{view_id, tags.size(), data.size()};

    this->data_queue.push(d.serialize());
    this->data_queue.push(tags);
    this->data_queue.push(data);

    this->data_queue.send_all();
}

}
