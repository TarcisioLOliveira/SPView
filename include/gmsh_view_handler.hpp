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

#ifndef SPVIEW_GMSH_VIEW_HANDLER_HPP
#define SPVIEW_GMSH_VIEW_HANDLER_HPP

#include "defs.hpp"
#include <gmsh.h>
#include <string>
#include <vector>

namespace spview{

class GmshViewHandler{
    public:

    GmshViewHandler(const std::string& model_name, const std::string& view_name, size_t elem_num, size_t node_num, size_t mat_color_num, const defs::ViewType view_type, const defs::DataType data_type, defs::ModelType model_type, const size_t view_id);
    ~GmshViewHandler(){
        this->remove_view();
    }

    /**
     * The `geometries` parameter is currently unused for nodal views.
     */
    void update_view(const std::vector<double>& data, std::vector<size_t>& tags) const;

    inline bool is_removed() const{
        return this->removed;
    }
    inline void remove_view() const{
        if(!this->removed){
            gmsh::view::remove(view_id);
        }
    }

    const std::string model_name;
    const defs::ViewType view_type;
    const defs::DataType data_type;

    private:
    const size_t view_id;
    const size_t elem_num;
    const size_t node_num;
    const size_t mat_color_num;
    defs::ModelType model_type;
    const size_t num_components;
    const std::string data_type_str;
    bool removed = false;

    inline size_t get_num_components() const{
        if(this->view_type == defs::VECTOR){
            return 3;
        } else if(this->view_type == defs::TENSOR){
            return 9;
        } else {
            return 1;
        }
    }

    inline std::string get_data_type_str() const{
        if(this->view_type == defs::ELEMENTAL){
            return "ElementData";
        } else {
            return "NodeData";
        }
    }

    inline void gmsh_update_view(const std::vector<double>& data, const std::vector<size_t>& tags) const{
        gmsh::view::addHomogeneousModelData(this->view_id, 0, this->model_name, this->data_type_str, tags, data, 0, this->num_components);
    }
};

}

#endif
