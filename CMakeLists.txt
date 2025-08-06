cmake_minimum_required(VERSION 3.31)
project(RaytracerRevisited)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(RaytracerRevisited
        aabb.h
        bvh.h
        camera.h
        color.h
        interval.h
        main.cpp
        material.h
        model.h
        ray.h
        tlas.h
        triangle.h
        utilities.h
        vec3.h
        stb_image.h
        rtw_stb_image.h
        texture.h)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(RaytracerRevisited PRIVATE assimp::assimp)

target_compile_options(RaytracerRevisited PRIVATE -O3)