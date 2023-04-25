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
    bool removed = false;

    inline void update_elemental(const std::vector<double>& data, const std::vector<size_t>& tags) const{
        this->gmsh_update_view(data, tags, 1, "ElementData");
    }
    inline void update_nodal(const std::vector<double>& data, const std::vector<size_t>& tags, const int num_components = 1) const{
        this->gmsh_update_view(data, tags, num_components, "NodeData");
    }
    inline void update_vector(const std::vector<double>& data, const std::vector<size_t>& tags) const{
        this->update_nodal(data, tags, 3);
    }
    inline void update_tensor(const std::vector<double>& data, const std::vector<size_t>& tags) const{
        this->update_nodal(data, tags, 9);
    }
    inline void gmsh_update_view(const std::vector<double>& data, const std::vector<size_t>& tags, const int num_components, const std::string& data_type) const{
        gmsh::view::addHomogeneousModelData(this->view_id, 0, this->model_name, data_type, tags, data, 0, num_components);
    }
};

}

#endif
