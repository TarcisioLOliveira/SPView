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

#ifndef SPVIEW_DEFS_HPP
#define SPVIEW_DEFS_HPP

#include <cstddef>
#include <vector>

namespace spview::defs{

inline const size_t MESSAGE_SIZE = 7;

enum ModelType : size_t{
    MODEL_2D,
    MODEL_3D
};

enum ElementType : size_t{
    TRI3,
    Q4,
    TET4,
    ELEMENT_TYPE_SIZE
};

enum ViewType : size_t{
    ELEMENTAL,
    NODAL,
    VECTOR,
    TENSOR
};

enum DataType : size_t{
    STRESS,
    MATERIAL,
    DISPLACEMENT,
    DENSITY,
    OTHER
};

enum BackendOption : size_t{
    GMSH
};

enum MessageType : size_t{
    NOTHING,
    INIT_CLIENT,
    ADD_VIEW,
    REMOVE_VIEW,
    UPDATE_DATA,
    CLOSE_CLIENT
};

struct InitData{
    ModelType model_type;
    ElementType element;
    size_t node_num;
    size_t elem_num;
    size_t mat_num;
    BackendOption backend;

    inline std::vector<size_t> serialize() const{
        return {INIT_CLIENT, model_type, element, node_num, elem_num, mat_num, backend};
    }
};

struct ViewData{
    ViewType view_type;
    DataType data_type;
    size_t name_size;

    inline std::vector<size_t> serialize() const{
        return {ADD_VIEW, view_type, data_type, name_size, 0, 0, 0};
    }
};

struct UpdateViewData{
    size_t view_id;
    size_t tags_size;
    size_t data_size;

    inline std::vector<size_t> serialize() const{
        return {UPDATE_DATA, view_id, tags_size, data_size, 0, 0, 0};
    }
};

}

#endif
