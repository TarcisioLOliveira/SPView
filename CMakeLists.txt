cmake_minimum_required(VERSION 3.9.0)
set(PROJECT_NAME SPViewClient)
set(LIB_NAME spview)

project(${PROJECT_NAME})

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)

set(NO_OPENMP OFF CACHE BOOL "Compile without OpenMP support")
set(THREADS_PREFER_PTHREAD_FLAG ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_library(GMSH_LIB gmsh REQUIRED PATHS /lib64/ /lib64/mpich/lib /lib64/openmpi/lib)
find_path(GMSH_INC gmsh.h REQUIRED PATHS /usr/include/ /usr/include/mpich-x86_64/)
find_package(Boost COMPONENTS filesystem thread REQUIRED)

if(WIN32 OR CYGWIN)
    list(APPEND GMSH_LIB winmm wsock32 ws2_32 psapi)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -m64")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")# -march=native -pipe -fomit-frame-pointer -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()

# Enable link time optimization if supported
include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)
if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        list(APPEND CMAKE_CXX_COMPILE_OPTIONS_IPO "-flto=auto")
    endif()
else()
    message(WARNING "IPO/LTO is not supported: ${output}")
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    SYSTEM
    ${GMSH_INC}
    ${Boost_INCLUDE_DIR}
)
   
add_subdirectory(src)
