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

#include <boost/asio/bind_executor.hpp>
#include <cstdio>
#include <boost/asio/buffered_read_stream.hpp>
#include <boost/asio/handler_alloc_hook.hpp>
#include <boost/asio/read.hpp>
#include <boost/bind.hpp>
#include "client.hpp"
#include "defs.hpp"
#include "logger.hpp"

namespace spview{

Client::Client(std::string pipe_name, Gmsh* backend):
    viewer(backend),
    pipe_name(pipe_name), strand(ios), pipe_file(open(("/tmp/"+pipe_name).c_str(), O_RDONLY)),
    pipe(ios, pipe_file),
    worker(boost::asio::make_work_guard(this->ios)){

    this->thread = boost::thread(boost::bind(
                        &Client::loop,
                        this)
                    );
} 

Client::~Client(){
    this->pipe.cancel();
    this->pipe.close();
    close(this->pipe_file);
}

void Client::get_messages(){
    boost::asio::post(
        this->strand,
        boost::bind(
            &Client::get_next_message,
            this
        )
    );
}

void Client::get_next_message(){
    boost::asio::async_read(this->pipe, 
        boost::asio::buffer(this->buffer, defs::MESSAGE_SIZE*sizeof(size_t)),
        boost::asio::bind_executor(
            this->strand,
            boost::bind(
                &Client::process_message,
                this
            )
        )
    );
}

void Client::process_message(){
    if(this->buffer[0] == defs::INIT_CLIENT){
        defs::ModelType model_type = static_cast<defs::ModelType>(this->buffer[1]);
        defs::ElementType elem_type = static_cast<defs::ElementType>(this->buffer[2]);
        const size_t nodes_per_elem = defs::NODES_PER_ELEMENT[elem_type];
        std::vector<double> points(this->buffer[3]*3);
        std::vector<size_t> tags(this->buffer[4]*nodes_per_elem);
        this->pipe.read_some(
                          boost::asio::buffer(points, points.size()*sizeof(double)));
        this->pipe.read_some(
                          boost::asio::buffer(tags, tags.size()*sizeof(size_t)));

        this->viewer->load_mesh(points, tags, this->buffer[3], this->buffer[4], this->buffer[5], elem_type, model_type);
    } else if(this->buffer[0] == defs::ADD_VIEW){
        logger::quick_log("WOOOOOOOOOOOOOORKEEED");
        std::string name;
        name.resize(this->buffer[3]);
        defs::ViewType view_type = static_cast<defs::ViewType>(this->buffer[1]);
        defs::DataType data_type = static_cast<defs::DataType>(this->buffer[2]);
        this->pipe.read_some(
                          boost::asio::buffer(name.data(), name.size()));

        this->viewer->add_view(name, view_type, data_type);
    } else if(this->buffer[0] == defs::UPDATE_DATA){
        std::vector<size_t> tags(this->buffer[2]);
        std::vector<double> data(this->buffer[3]);
        this->pipe.read_some(
                          boost::asio::buffer(tags, tags.size()*sizeof(size_t)));
        this->pipe.read_some(
                          boost::asio::buffer(data, data.size()*sizeof(double)));

        this->viewer->update_view(this->buffer[1], tags, data);
    } else if(this->buffer[0] == defs::REMOVE_VIEW){
        this->viewer->remove_view(this->buffer[1]);
    } else if(this->buffer[0] == defs::CLOSE_CLIENT){
        this->viewer->end();
    }
    if(this->running){
        this->get_next_message();
    }
}

void Client::loop(){
    this->ios.run();
}

}
