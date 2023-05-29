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

#include "gmsh.hpp"
#include <gmsh.h>
#include <algorithm>
#include <numeric>
#include <vector>
#include "defs.hpp"
#include "logger.hpp"

namespace spview{

Gmsh::Gmsh():
    to_gmsh_type(defs::ELEMENT_TYPE_SIZE){

    to_gmsh_type[defs::TRI3] = 2;
    to_gmsh_type[defs::Q4] = 3;
    to_gmsh_type[defs::TET4] = 4;

}

void Gmsh::load_mesh(std::vector<double> points, std::vector<size_t> elem_nodes, size_t node_num, size_t elem_num, size_t mat_num, defs::ElementType elem_type, defs::ModelType type){
    gmsh::clear();
    gmsh::model::add(this->MODEL_NAME);

    this->type = type;
    this->elem_num = elem_num;
    this->node_num = node_num;
    this->mat_num = mat_num;

    std::vector<size_t> node_tags(node_num);
    std::iota(node_tags.begin(), node_tags.end(), 1);
    std::vector<size_t> elem_tags(elem_num);
    std::iota(elem_tags.begin(), elem_tags.end(), 1);

    if(type == defs::MODEL_2D){
        this->mesh_tag = gmsh::model::addDiscreteEntity(2);
        gmsh::model::mesh::addNodes(2, this->mesh_tag, node_tags, points);
    } else if(type == defs::MODEL_3D){
        this->mesh_tag = gmsh::model::addDiscreteEntity(3);
        gmsh::model::mesh::addNodes(3, this->mesh_tag, node_tags, points);
    }

    for(auto& e:elem_nodes){
        ++e;
    }

    gmsh::model::mesh::addElementsByType(this->mesh_tag, this->to_gmsh_type[elem_type], elem_tags, elem_nodes);

    gmsh::option::setNumber("Mesh.SurfaceEdges", 0);
    gmsh::option::setNumber("Mesh.VolumeEdges", 0);
    gmsh::option::setNumber("Mesh.Triangles", 0);
    gmsh::option::setNumber("General.ColorScheme", 3);
    gmsh::option::setNumber("General.FltkColorScheme", 1);
    gmsh::option::setString("General.GraphicsFontEngine", "StringTexture");

}

void Gmsh::add_view(const std::string& view_name, defs::ViewType view_type, defs::DataType data_type){
    this->updating = true;
    this->lock.lock();
    ++this->last_view_tag;
    this->handler_list.emplace_back(std::make_unique<GmshViewHandler>(this->MODEL_NAME, view_name, this->elem_num, this->node_num, this->mat_num, view_type, data_type, this->type, this->last_view_tag));
    gmsh::graphics::draw();
    this->lock.unlock();
    this->updating = false;
}

void Gmsh::update_view(size_t view_id, std::vector<size_t> tags, std::vector<double> data){
    this->updating = true;
    this->lock.lock();
    this->handler_list[view_id]->update_view(data, tags);
    gmsh::graphics::draw();
    this->lock.unlock();
    this->updating = false;
}
void Gmsh::remove_view(size_t view_id){
    this->updating = true;
    this->lock.lock();
    this->handler_list[view_id].reset();
    gmsh::graphics::draw();
    this->lock.unlock();
    this->updating = false;
}

void Gmsh::show(){
    this->shown = true;

    gmsh::fltk::initialize();
}
void Gmsh::get_events(){
    if(!this->updating){
        this->lock.lock();
        gmsh::fltk::wait();
        //std::vector<std::string> action;
        //gmsh::onelab::getString("ONELAB/Action", action);
        //if(action.size() && action[0] == "check") {
        //    gmsh::onelab::setString("ONELAB/Action", {""});
        //    gmsh::graphics::draw();
        //}
        if(!gmsh::fltk::isAvailable()){
            this->end();
        }
        this->lock.unlock();
    }
}

}
