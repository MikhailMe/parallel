cmake_minimum_required(VERSION 3.8)

project(parallel)

option(USE_MPI OFF)
if( USE_MPI )
    find_package( MPI )
    if( MPI_FOUND )
        include_directories( ${MPI_INCLUDE_PATH} )
    else( MPI_FOUND )
        set( USE_MPI OFF )
    endif( MPI_FOUND )
endif( USE_MPI )

include_directories("C:/Program Files (x86)/Microsoft SDKs/MPI/Include")
target_link_libraries(MPI "C:/Program Files (x86)/Microsoft SDKs/MPI/Lib/x64")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -lmpi -g2 -O0")

set(MATRIX_H matrix/Matrix.h helper.h)

add_executable(parallel main.cpp matrix/Matrix.cpp ${MATRIX_H})