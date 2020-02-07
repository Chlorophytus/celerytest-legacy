cmake_minimum_required(VERSION 3.15)

# You can change the name of the project
project(sdl_template VERSION 0.1.0.2)

# You can change the C++ standard too
set(CMAKE_CXX_STANDARD 17)

# First we configure the project
configure_file(include/${PROJECT_NAME}_cfg.in include/${PROJECT_NAME}_cfg.hpp)

# Find the SDL2 package
find_package(SDL2 REQUIRED)

# SDL2 directories should be included
include_directories(${SDL2_INCLUDE_DIRS})

# If you add classes you should add them here
add_executable(${PROJECT_NAME} src/${PROJECT_NAME}_interwork.cpp src/${PROJECT_NAME}.cpp)

# Link to SDL2
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})