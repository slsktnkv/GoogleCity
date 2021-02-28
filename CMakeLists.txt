cmake_minimum_required(VERSION 3.19)

project(GoogleCity LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wpedantic -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Ofast -march=native -flto")

add_executable(GoogleCity main.cpp
    CityUtils.h CityUtils.cpp
    City.h City.cpp)
add_executable(GoogleCitySimple SimpleSimulation.cpp)
add_executable(GoogleCityAndrew GoogleCityAndrew.cpp)
