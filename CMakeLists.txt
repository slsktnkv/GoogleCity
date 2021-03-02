cmake_minimum_required(VERSION 3.19)

project(GoogleCity LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wpedantic -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Ofast -march=native -flto")

include(FindOpenMP)
if (OpenMP_CXX_FOUND)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${OpenMP_CXX_FLAGS}")
    message(${CMAKE_CXX_FLAGS_DEBUG})
endif()

add_executable(GoogleCity main.cpp
    City.h City.cpp
    Simulation.h Simulation.cpp
    Genetics.h Genetics.cpp
    )

add_executable(GoogleCityAndrew GoogleCityAndrew.cpp)
