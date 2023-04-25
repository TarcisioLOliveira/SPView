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

namespace spview::defs{

enum ModelType : size_t{
    MODEL_2D,
    MODEL_3D
};

enum ElementType : size_t{
    TRI3,
    Q4,
    TET4,
    ET_SIZE
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

enum MessageType : size_t{
    INIT_CLIENT,
    ADD_VIEW,
    REMOVE_VIEW,
    UPDATE_DATA,
    CLOSE_CLIENT
};

}

#endif
