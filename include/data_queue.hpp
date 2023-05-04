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

#ifndef SPVIEW_DATA_QUEUE_HPP
#define SPVIEW_DATA_QUEUE_HPP

#include <string>
#include <queue>
#include <vector>
#include <boost/asio.hpp>
#include <boost/process/async_pipe.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/thread.hpp>

namespace spview{

class DataQueue{
    public:
    DataQueue(boost::asio::io_service& ios, std::string pipe_name);
    ~DataQueue();

    void send_all();

    inline void push(std::string data){
        this->queue.push(this->STRING);
        this->string_queue.push(std::move(data));
    }

    inline void push(std::vector<size_t> data){
        this->queue.push(this->SIZE_T);
        this->size_t_queue.push(std::move(data));
    }

    inline void push(std::vector<double> data){
        this->queue.push(this->DOUBLE);
        this->double_queue.push(std::move(data));
    }

    inline size_t size() const{
        return this->queue.size();
    }

    private:
    enum DataType{
        STRING,
        SIZE_T,
        DOUBLE
    };

    std::string pipe_name;
    boost::asio::io_service& ios;
    boost::asio::io_service::strand strand;
    boost::process::async_pipe pipe;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> worker;
    boost::thread thread;
    std::queue<DataType> queue;
    std::queue<std::string> string_queue;
    std::queue<std::vector<size_t>> size_t_queue;
    std::queue<std::vector<double>> double_queue;
    bool sending = false;

    void loop();

    void send_next();

    void on_end(const boost::system::error_code& error, const size_t bytesTransferred);
};

}

#endif
