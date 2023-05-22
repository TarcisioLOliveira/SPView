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

#ifndef SPVIEW_GMSH_HPP
#define SPVIEW_GMSH_HPP

#include <vector>
#include <memory>
#include <boost/thread.hpp>
#include <gmsh.h>
#include "defs.hpp"
#include "gmsh_view_handler.hpp"
#include "logger.hpp"

namespace spview{

class Gmsh{
    public:

    Gmsh();
    inline void start() const {gmsh::initialize();}

    void load_mesh(std::vector<double> points, std::vector<size_t> elem_nodes, size_t node_num, size_t elem_num, size_t mat_num, defs::ElementType elem_type, defs::ModelType type);
    void add_view(const std::string& view_name, defs::ViewType view_type, defs::DataType data_type);
    void update_view(size_t view_id, std::vector<size_t> tags, std::vector<double> data);
    void remove_view(size_t view_id);

    void show();
    inline void hide(){
        this->shown = false;
    }
    inline void redraw(){
        if(this->shown){
            if(!this->updating){
                this->lock.lock();
                gmsh::graphics::draw();
                this->lock.unlock();
            }
        }
    }

    void get_events();
    inline void end(){
        if(!this->ended){
            this->handler_list.clear();
            this->hide();
            gmsh::finalize();
            this->ended = true;
        }
    }
    inline bool is_running() const{
        return !this->ended;
    }

    private:
    const std::string MODEL_NAME = "loaded_model";
    bool shown = false;
    bool ended = false;
    bool updating = false;
    int mesh_tag = 0;
    int last_view_tag = 0;
    defs::ModelType type = defs::MODEL_2D;
    boost::mutex lock;
    size_t node_num;
    size_t elem_num;
    size_t mat_num;
    std::vector<size_t> to_gmsh_type;
    std::vector<std::unique_ptr<GmshViewHandler>> handler_list;
};

}

#endif
