cmake_minimum_required(VERSION 2.8)

project(voxel)
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SRC "*.cpp" "*.c")
message(SRC=${SRC})

add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/glad/include
    /usr/include/freetype2)

target_link_libraries(${PROJECT_NAME} glfw noise freetype dl)
