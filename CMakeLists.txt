cmake_minimum_required(VERSION 3.17)

# You can change the name of the project
project(celerytest VERSION 0.2.0.47)

# First we configure the project
configure_file(include/${PROJECT_NAME}_cfg.in
        ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}_cfg.hpp)

# Find OpenGL
find_package(OpenGL REQUIRED)
# Find GLEW
find_package(GLEW REQUIRED)
# Find Lua
include(FindLua)
find_package(Lua REQUIRED)
# Find the SDL2 package(s)
find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(sdl2-net CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)
# Find threading
find_package(Threads REQUIRED)

# If you add classes you should add them here
add_executable(${PROJECT_NAME}
        src/${PROJECT_NAME}_con.cpp
        src/${PROJECT_NAME}_lua.cpp
        src/${PROJECT_NAME}_sim.cpp
        src/${PROJECT_NAME}_gl.cpp
        src/${PROJECT_NAME}_glview.cpp
        src/${PROJECT_NAME}_gui.cpp
        src/${PROJECT_NAME}.cpp)

# Enable filthy C++17 extensions
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

# GLEW, Lua, SDL2 headers
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})

# Link to GLEW, Lua, and SDL2
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW ${LUA_LIBRARIES} Threads::Threads  SDL2::SDL2 SDL2::SDL2_image SDL2::SDL2_net SDL2::SDL2_ttf SDL2::SDL2main)
