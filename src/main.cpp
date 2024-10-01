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

#include <gmsh.h>
#include "client.hpp"
#include "gmsh.hpp"
#include "logger.hpp"

int main(int argc, char* argv[]){
    using namespace spview;

    if(argc == 1){
        logger::quick_log("Error: missing pipe name.");
        return 1; 
    }
    //std::cout << "pid: " << getpid() << std::endl;
    //sleep(15);


    logger::quick_log("client launched with pipe", argv[1]);
    Gmsh viewer;
    viewer.start();
    Client c(argv[1], &viewer);
    viewer.show();

    while(c.is_running() && viewer.is_running()){
        try{
            c.get_messages();
            viewer.get_events();
            viewer.redraw();
        } catch(std::exception& e){
            logger::quick_log(e.what());
        }
    }

    if(viewer.is_running()){
        viewer.end();
    }
    
    logger::quick_log("client exited successfully");

    return 0;
}
