cmake_minimum_required(VERSION 3.15)

# You can change the name of the project
project(celerytest VERSION 0.1.0.9)

# First we configure the project
configure_file(include/${PROJECT_NAME}_cfg.in include/${PROJECT_NAME}_cfg.hpp)

# Find OpenGL
find_package(OpenGL REQUIRED)
# Find GLEW
find_package(GLEW REQUIRED)
# Find Lua
find_package(Lua REQUIRED)
# Find the SDL2 package
find_package(SDL2 REQUIRED)

# GLEW, Lua, SDL2 headers
include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${LUA_INCLUDE_DIRS}
    ${SDL2_INCLUDE_DIRS})

# If you add classes you should add them here
add_executable(${PROJECT_NAME}
    src/${PROJECT_NAME}_log.cpp
    src/${PROJECT_NAME}_interwork.cpp
    src/${PROJECT_NAME}_lua.cpp
    src/${PROJECT_NAME}_shader.cpp
    src/${PROJECT_NAME}_sim.cpp
    src/${PROJECT_NAME}.cpp)

# Enable filthy C++17 extensions
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# Link to GLEW, Lua, and SDL2
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${LUA_LIBRARIES}
    ${SDL2_LIBRARIES} SDL2.so) 