cmake_minimum_required(VERSION 3.8)
project(qbf_abc CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-g -O3 -Wall)

file(GLOB FILENAMES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM FILENAMES "src/main.cpp")
add_library(qbf_abc ${FILENAMES})
target_include_directories(qbf_abc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(gen "src/main.cpp")
target_link_libraries(gen PUBLIC qbf_abc)

