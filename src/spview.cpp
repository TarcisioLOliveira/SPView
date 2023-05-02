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

Server::~Server(){
    this->client_output.cancel();
    this->client_output.close();
    this->proc.terminate();
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

void Server::init_client(defs::InitData data, const std::vector<double>& points, const std::vector<size_t>& elem_nodes){
    this->data_queue.push(data.serialize());
    this->data_queue.push(points);
    this->data_queue.push(elem_nodes);

    this->data_queue.send_all();
}
void Server::add_view(defs::ViewType view_type, defs::DataType data_type, const std::string& name){
    defs::ViewData d{view_type, data_type, name.size()};
    logger::quick_log("ADD VIEW SENT!!!!!!!!!!!!");

    this->data_queue.push(d.serialize());
    this->data_queue.push(name);

    this->data_queue.send_all();
}

void Server::update_data(size_t view_id, const std::vector<size_t>& tags, const std::vector<double>& data){
    defs::UpdateViewData d{view_id, tags.size(), data.size()};

    this->data_queue.push(d.serialize());
    this->data_queue.push(tags);
    this->data_queue.push(data);

    this->data_queue.send_all();
}

void Server::remove_view(size_t view_id){
    std::vector<size_t> d(defs::MESSAGE_SIZE, 0);
    d[0] = defs::REMOVE_VIEW;
    d[1] = view_id;

    this->data_queue.push(d);

    this->data_queue.send_all();
}

void Server::close_client(){
    std::vector<size_t> d(defs::MESSAGE_SIZE, 0);
    d[0] = defs::CLOSE_CLIENT;

    this->data_queue.push(d);

    this->data_queue.send_all();
}

}
