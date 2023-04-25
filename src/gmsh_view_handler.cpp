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

#include <numeric>
#include "gmsh_view_handler.hpp"

namespace spview{

GmshViewHandler::GmshViewHandler(const std::string& model_name, const std::string& view_name, size_t elem_num, size_t node_num, size_t mat_color_num, const defs::ViewType view_type, const defs::DataType data_type, defs::ModelType model_type, const size_t view_id)
    : model_name(model_name), view_type(view_type), data_type(data_type), view_id(view_id),
      elem_num(elem_num),
      node_num(node_num),
      mat_color_num(mat_color_num),
      model_type(model_type){

    gmsh::view::add(view_name, view_id);
    if((this->mat_color_num == 2 && data_type == defs::MATERIAL) || data_type == defs::DENSITY){
        gmsh::view::option::setNumber(view_id, "ColormapNumber", 9); //grayscale
        gmsh::view::option::setNumber(view_id, "ColormapInvert", 1.0); //inverted
    }
}


void GmshViewHandler::update_view(const std::vector<double>& data, std::vector<size_t>& tags) const{
    if(this->removed){
        return;
    }
    for(size_t& t:tags){
        ++t;
    }
    // Elemental view shouldn't really be used for displacement
    if(this->view_type == defs::ELEMENTAL){
        this->update_elemental(data, tags);
    } else if(this->view_type == defs::NODAL){
        this->update_nodal(data, tags);
    } else if(this->view_type == defs::TENSOR){
        this->update_tensor(data, tags);
    } else if(this->view_type == defs::VECTOR){
        this->update_vector(data, tags);
    }
}

}
