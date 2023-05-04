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

#include <boost/asio/io_service.hpp>
#include <iostream>
#include <boost/bind.hpp>
#include "data_queue.hpp"
#include "logger.hpp"

namespace spview{

DataQueue::DataQueue(boost::asio::io_service& ios, std::string pipe_name):
    pipe_name(pipe_name), ios(ios), strand(ios), pipe(ios, pipe_name),
    worker(boost::asio::make_work_guard(ios)){
    this->thread = boost::thread(boost::bind(
                        &DataQueue::loop,
                        this)
                    );
}

DataQueue::~DataQueue(){
    this->worker.reset();
    if(this->thread.joinable()){
        this->thread.join();
    }
    this->pipe.cancel();
    this->pipe.close();
}

void DataQueue::send_all(){
    if(this->sending){
        return;
    }
    if(this->size() == 0){
        return;
    }
    this->sending = true;

    boost::asio::post(
        this->strand,
        boost::bind(
            &DataQueue::send_next,
            this
        )
    );
    //this->ios.run();
}

void DataQueue::send_next(){
    size_t type = this->queue.front();

    if(type == STRING){
        const std::string& data = this->string_queue.front();
        boost::asio::async_write(
            this->pipe,
            boost::asio::buffer(data),
            boost::asio::bind_executor(
                this->strand,
                boost::bind(
                    &DataQueue::on_end,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                )
            )
        );
    } else if(type == SIZE_T){
        const std::vector<size_t>& data = this->size_t_queue.front();
        boost::asio::async_write(
            this->pipe,
            boost::asio::buffer(data),
            boost::asio::bind_executor(
                this->strand,
                boost::bind(
                    &DataQueue::on_end,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                )
            )
        );
    } else if(type == DOUBLE){
        const std::vector<double>& data = this->double_queue.front();
        boost::asio::async_write(
            this->pipe,
            boost::asio::buffer(data),
            boost::asio::bind_executor(
                this->strand,
                boost::bind(
                    &DataQueue::on_end,
                    this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred
                )
            )
        );
    }
}

void DataQueue::on_end(const boost::system::error_code& error, const size_t bytesTransferred){
    (void) bytesTransferred;

    size_t type = this->queue.front();
    this->queue.pop();

    if(type == STRING){
        this->string_queue.pop();
    } else if(type == SIZE_T){
        this->size_t_queue.pop();
    } else if(type == DOUBLE){
        this->double_queue.pop();
    }

    if(error){
        std::cout << "could not write: " << boost::system::system_error(error).what() << std::endl;
        return;
    }

    if(this->size() > 0){
        this->send_next();
    } else {
        this->sending = false;
    }
}

void DataQueue::loop(){
    this->ios.run();
}

}
