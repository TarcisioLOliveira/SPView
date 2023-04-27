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

Client::Client(std::string pipe_name):
    pipe_name(pipe_name), strand(ios), pipe_file(open(("/tmp/"+pipe_name).c_str(), O_RDONLY)),
    pipe(ios, pipe_file),
    worker(boost::asio::make_work_guard(this->ios)){

    std::fill(this->buffer, this->buffer+BUFFER_SIZE, 0);

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
    logger::quick_log("reached");
    boost::asio::async_read(this->pipe, 
        boost::asio::buffer(this->buffer, BUFFER_SIZE*sizeof(size_t)),
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
    logger::quick_log("reached");
    if(this->buffer[0] == defs::UPDATE_DATA){
        std::vector<size_t> tags(this->buffer[2]);
        std::vector<double> data(this->buffer[3]);
        this->pipe.read_some(
                          boost::asio::buffer(tags, tags.size()*sizeof(size_t)));
        this->pipe.read_some(
                          boost::asio::buffer(data, data.size()*sizeof(double)));

        logger::quick_log("reached");
        logger::quick_log(tags);
        logger::quick_log(data);
    }
    std::fill(this->buffer, this->buffer+BUFFER_SIZE, 0);
}

void Client::loop(){
    this->ios.run();
}

}
